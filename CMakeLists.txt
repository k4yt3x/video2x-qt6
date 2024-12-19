cmake_minimum_required(VERSION 3.19)
project(video2x-qt6 VERSION 6.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network LinguistTools)

qt_standard_project_setup()

find_package(Vulkan REQUIRED)

# Find FFmpeg libraries and includes
set(FFMPEG_INCLUDE_DIRS)
set(FFMPEG_LIBS)
if(WIN32)
    # Add FFmpeg libraries and headers
    set(FFMPEG_BASE_PATH ${CMAKE_SOURCE_DIR}/third_party/ffmpeg-shared)
    list(APPEND FFMPEG_LIBS
        ${FFMPEG_BASE_PATH}/lib/avformat.lib
        ${FFMPEG_BASE_PATH}/lib/avcodec.lib
        ${FFMPEG_BASE_PATH}/lib/avfilter.lib
        ${FFMPEG_BASE_PATH}/lib/avutil.lib
        ${FFMPEG_BASE_PATH}/lib/swscale.lib
        ${FFMPEG_BASE_PATH}/lib/avdevice.lib
    )
    list(APPEND FFMPEG_INCLUDE_DIRS ${FFMPEG_BASE_PATH}/include)

    # Add libvideo2x libraries and headers
    set(LIBVIDEO2X_BASE_PATH ${CMAKE_SOURCE_DIR}/third_party/libvideo2x-shared)
    list(APPEND FFMPEG_LIBS ${LIBVIDEO2X_BASE_PATH}/libvideo2x.lib)
    list(APPEND FFMPEG_INCLUDE_DIRS ${LIBVIDEO2X_BASE_PATH}/include)
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
        list(APPEND FFMPEG_INCLUDE_DIRS ${${PKG}_INCLUDE_DIRS})
        list(APPEND FFMPEG_LIBS ${${PKG}_LIBRARIES})
    endforeach()
endif()

if(NOT WIN32)
    # Include ExternalProject module
    include(ExternalProject)

    # Add libreal-esrgan-ncnn-vulkan as an external project
    ExternalProject_Add(
        libvideo2x
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/video2x
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libvideo2x_install
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_ALWAYS ON
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config ${CMAKE_BUILD_TYPE}
    )
endif()

set(TS_FILES src/video2x-qt6_en_US.ts
             src/video2x-qt6_zh_CN.ts
             src/video2x-qt6_ja_JP.ts
             src/video2x-qt6_pt_PT.ts
             src/video2x-qt6_fr_FR.ts)

set(PROJECT_SOURCES
    src/aboutdialog.cpp
    src/aboutdialog.h
    src/aboutdialog.ui
    src/configmanager.cpp
    src/configmanager.h
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
    WIN32 MACOSX_BUNDLE
    ${PROJECT_SOURCES}
    ${APP_RESOURCE_FILE}
)

qt_add_translations(video2x-qt6 TS_FILES
    ${TS_FILES}
    LUPDATE_OPTIONS -no-obsolete
)

target_include_directories(video2x-qt6 PRIVATE
    ${FFMPEG_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/video2x/include
)

if(WIN32)
    set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/libvideo2x_install/lib/libvideo2x.lib)
else()
    add_dependencies(video2x-qt6 libvideo2x)
    set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/libvideo2x_install/lib/libvideo2x.so)
endif()

target_link_libraries(video2x-qt6 PRIVATE
    Qt::Core
    Qt::Widgets
    Qt::Network
    Vulkan::Vulkan
    ${FFMPEG_LIBS}
)

include(GNUInstallDirs)

install(TARGETS video2x-qt6
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

qt_generate_deploy_app_script(
    TARGET video2x-qt6
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
