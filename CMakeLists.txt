cmake_minimum_required(VERSION 3.16)

project(video2x-qt6 VERSION 6.0.0 LANGUAGES CXX)

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
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/videoprocessingworker.h
    src/videoprocessingworker.cpp
    src/filedroplistview.h
    src/filedroplistview.cpp
    src/aboutdialog.h
    src/aboutdialog.cpp
    src/aboutdialog.ui
    src/timer.h
    src/timer.cpp
    ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    if(WIN32)
        set(APP_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/resources.rc")
    endif()
    qt_add_executable(video2x-qt6
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${APP_RESOURCE_FILE}
    )

    # qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(video2x-qt6 SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(video2x-qt6
            ${PROJECT_SOURCES}
        )
    endif()

    # qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

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

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.video2x-qt6)
endif()
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

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(video2x-qt6)
endif()
