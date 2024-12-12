cmake_minimum_required(VERSION 3.16)

project(video2x-qt6 VERSION 6.1.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

option(USE_SYSTEM_NCNN "Use sytem ncnn library" OFF)

set(ALL_INCLUDE_DIRS)
set(ALL_LIBRARIES)

find_package(Vulkan REQUIRED)
list(APPEND ALL_LIBRARIES Vulkan::Vulkan)

if(WIN32)
    # Add FFmpeg libraries and headers
    set(FFMPEG_BASE_PATH ${CMAKE_SOURCE_DIR}/third_party/ffmpeg-shared)
    list(APPEND ALL_LIBRARIES
        ${FFMPEG_BASE_PATH}/lib/avformat.lib
        ${FFMPEG_BASE_PATH}/lib/avcodec.lib
        ${FFMPEG_BASE_PATH}/lib/avfilter.lib
        ${FFMPEG_BASE_PATH}/lib/avutil.lib
        ${FFMPEG_BASE_PATH}/lib/swscale.lib
        ${FFMPEG_BASE_PATH}/lib/avdevice.lib
    )
    list(APPEND ALL_INCLUDE_DIRS ${FFMPEG_BASE_PATH}/include)

    # Add libvideo2x libraries and headers
    set(LIBVIDEO2X_BASE_PATH ${CMAKE_SOURCE_DIR}/third_party/libvideo2x-shared)
    list(APPEND ALL_LIBRARIES ${LIBVIDEO2X_BASE_PATH}/libvideo2x.lib)
    list(APPEND ALL_INCLUDE_DIRS ${LIBVIDEO2X_BASE_PATH}/include)
else()
    # Find the required packages using pkg-config
    find_package(PkgConfig REQUIRED)
    set(REQUIRED_PKGS
        libavformat
        libavcodec
        libavfilter
        libavutil
        libswscale
        libavdevice
    )

    # Loop through each package to find and collect include dirs and libraries
    foreach(PKG ${REQUIRED_PKGS})
        pkg_check_modules(${PKG} REQUIRED ${PKG})
        list(APPEND ALL_INCLUDE_DIRS ${${PKG}_INCLUDE_DIRS})
        list(APPEND ALL_LIBRARIES ${${PKG}_LIBRARIES})
    endforeach()
endif()

# Remove duplicate entries
list(REMOVE_DUPLICATES ALL_INCLUDE_DIRS)
list(REMOVE_DUPLICATES ALL_LIBRARIES)

# Include ExternalProject module
include(ExternalProject)

if(NOT WIN32)
    # Add libreal-esrgan-ncnn-vulkan as an external project
    ExternalProject_Add(
        libvideo2x
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/video2x
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libvideo2x_install
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DUSE_SYSTEM_NCNN=${USE_SYSTEM_NCNN}
        BUILD_ALWAYS ON
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config ${CMAKE_BUILD_TYPE}
    )
endif()

set(TS_FILES src/video2x-qt6_en_US.ts
             src/video2x-qt6_zh_CN.ts
             src/video2x-qt6_ja_JP.ts
             src/video2x-qt6_pt_PT.ts)

set(PROJECT_SOURCES
    src/aboutdialog.cpp
    src/aboutdialog.h
    src/aboutdialog.ui
    src/filedroptableview.cpp
    src/filedroptableview.h
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/taskconfig.h
    src/taskconfigdialog.cpp
    src/taskconfigdialog.h
    src/taskconfigdialog.ui
    src/taskprocessor.cpp
    src/taskprocessor.h
    src/timer.cpp
    src/timer.h
    src/utils.cpp
    src/utils.h
    ${TS_FILES}
)

if(WIN32)
    set(APP_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/resources.rc")
endif()

qt_add_executable(video2x-qt6
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${APP_RESOURCE_FILE}
)

# qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
qt_add_translations(video2x-qt6 TS_FILES
    ${TS_FILES}
    LUPDATE_OPTIONS -no-obsolete
)

include_directories(
    ${ALL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/video2x/include
)

if(WIN32)
    set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/libvideo2x_install/lib/libvideo2x.lib)
else()
    add_dependencies(video2x-qt6 libvideo2x)
    set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/libvideo2x_install/lib/libvideo2x.so)
endif()

target_link_libraries(video2x-qt6 PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${ALL_LIBRARIES})

set_target_properties(video2x-qt6 PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS video2x-qt6
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_finalize_executable(video2x-qt6)
