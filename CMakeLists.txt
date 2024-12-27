cmake_minimum_required(VERSION 3.19)
project(video2x-qt6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VIDEO2X_ENABLE_NATIVE "Enable native optimizations (-march=native)" OFF)
option(VIDEO2X_ENABLE_X86_64_V4 "Enable x86-64-v4 optimizations (-march=x86-64-v4)" OFF)
option(VIDEO2X_ENABLE_AVX512F "Enable AVX-512 foundation optimizations (-march=avx512f)" OFF)
option(VIDEO2X_ENABLE_X86_64_V3 "Enable x86-64-v3 optimizations (-march=x86-64-v3)" OFF)
option(VIDEO2X_ENABLE_AVX2 "Enable AVX2 optimizations (-march=avx2)" OFF)

option(USE_EXTERNAL_SPDLOG "Use the system-provided spdlog library" ON)
option(USE_EXTERNAL_VIDEO2X "Use the system-provided video2x" OFF)
option(USE_SHARED_VIDEO2X "Use the shared video2x library" OFF)

# Set global compile options for all targets
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_compile_options(/W4 /permissive-)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()

# Set the default optimization flags for Release builds
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Set the optimization flags for each compiler
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/Ox /Ot /GL /DNDEBUG)
        add_link_options(/LTCG /OPT:REF /OPT:ICF)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O3 -ffunction-sections -fdata-sections)
        add_link_options(-Wl,-s -flto -Wl,--gc-sections)
    endif()
endif()

# Enable the requested architecture-specific optimizations
if(VIDEO2X_ENABLE_NATIVE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:NATIVE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=native)
    endif()
elseif(VIDEO2X_ENABLE_X86_64_V4)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:AVX2)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=x86-64-v4)
    endif()
elseif(VIDEO2X_ENABLE_AVX512F)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:AVX512)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-mavx512f)
    endif()
elseif(VIDEO2X_ENABLE_X86_64_V3)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:AVX2)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=x86-64-v3)
    endif()
elseif(VIDEO2X_ENABLE_AVX2)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:AVX2)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-mavx2)
    endif()
endif()

# Define lists to store include directories and libraries
set(VIDEO2X_QT6_INCLUDE_DIRS)
set(VIDEO2X_QT6_LIBS)

# Find the required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network LinguistTools)
qt_standard_project_setup()
list(APPEND VIDEO2X_QT6_LIBS Qt6::Core Qt6::Widgets Qt6::Network)

# Find the Vulkan SDK
find_package(Vulkan REQUIRED)
list(APPEND VIDEO2X_QT6_LIBS Vulkan::Vulkan)

# Find FFmpeg libraries and includes
if(WIN32)
    # Add FFmpeg libraries and headers
    set(FFMPEG_BASE_PATH ${CMAKE_SOURCE_DIR}/third_party/ffmpeg-shared)
    list(APPEND VIDEO2X_QT6_LIBS
        ${FFMPEG_BASE_PATH}/lib/avutil.lib
        ${FFMPEG_BASE_PATH}/lib/avcodec.lib
    )
    list(APPEND VIDEO2X_QT6_INCLUDE_DIRS ${FFMPEG_BASE_PATH}/include)
else()
    # Find the required packages using pkg-config
    find_package(PkgConfig REQUIRED)
    set(REQUIRED_PKGS libavutil libavcodec)

    # Loop through each package to find and collect include dirs and libraries
    foreach(PKG ${REQUIRED_PKGS})
        pkg_check_modules(${PKG} REQUIRED ${PKG})
        list(APPEND VIDEO2X_QT6_INCLUDE_DIRS ${${PKG}_INCLUDE_DIRS})
        list(APPEND VIDEO2X_QT6_LIBS ${${PKG}_LIBRARIES})
    endforeach()
endif()

# Find the spdlog library
if(USE_EXTERNAL_SPDLOG)
    find_package(spdlog REQUIRED)
    set(SPDLOG_LIB spdlog::spdlog)
else()
    add_subdirectory(third_party/spdlog)
    set(SPDLOG_LIB spdlog::spdlog_header_only)
endif()
list(APPEND VIDEO2X_QT6_LIBS ${SPDLOG_LIB})

# Find the libvideo2x library
if(USE_EXTERNAL_VIDEO2X)
    find_package(Video2X REQUIRED)
    list(APPEND VIDEO2X_QT6_LIBS Video2X::libvideo2x)
elseif(USE_SHARED_VIDEO2X)
    # Add libvideo2x shared libraries and headers
    set(LIBVIDEO2X_BASE_PATH ${CMAKE_SOURCE_DIR}/third_party/video2x-install)
    list(APPEND VIDEO2X_QT6_LIBS ${LIBVIDEO2X_BASE_PATH}/libvideo2x.lib)
    list(APPEND VIDEO2X_QT6_INCLUDE_DIRS ${LIBVIDEO2X_BASE_PATH}/include)
else()
    # Include ExternalProject module
    include(ExternalProject)

    # Add libvideo2x as an external project
    ExternalProject_Add(
        Video2X
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/third_party/video2x
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/video2x-install
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_ALWAYS ON
        INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install --config ${CMAKE_BUILD_TYPE}
    )
endif()

# Define the list of translation files
set(TS_FILES src/video2x-qt6_en_US.ts
             src/video2x-qt6_zh_CN.ts
             src/video2x-qt6_ja_JP.ts
             src/video2x-qt6_pt_PT.ts
             src/video2x-qt6_fr_FR.ts)

# Define the project source files
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
    src/qttexteditsink.cpp
    src/qttexteditsink.h
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
    src/translations.qrc
    ${TS_FILES}
)

# Define the application resource file for Windows
if(WIN32)
    set(APP_RESOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/resources.rc")
endif()

qt_add_executable(video2x-qt6
    WIN32 MACOSX_BUNDLE
    ${PROJECT_SOURCES}
    ${APP_RESOURCE_FILE}
)

qt_add_translations(video2x-qt6
    TS_FILES ${TS_FILES}
    LUPDATE_OPTIONS -no-obsolete
    RESOURCE_PREFIX "/i18n"
)

target_include_directories(video2x-qt6 PRIVATE
    ${spdlog_INCLUDE_DIRS}
    ${VIDEO2X_QT6_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third_party/video2x/include
)

if(NOT USE_EXTERNAL_VIDEO2X AND NOT USE_SHARED_VIDEO2X)
    add_dependencies(video2x-qt6 Video2X)
    if(WIN32)
        set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/video2x-install/libvideo2x.lib)
    else()
        set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/video2x-install/libvideo2x.so)
    endif()
    list(APPEND VIDEO2X_QT6_LIBS ${LIBVIDEO2X_LIB})
endif()

target_link_libraries(video2x-qt6 PRIVATE ${VIDEO2X_QT6_LIBS})

include(GNUInstallDirs)

install(TARGETS video2x-qt6
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(WIN32)
    qt_generate_deploy_app_script(
        TARGET video2x-qt6
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR
    )
    install(SCRIPT ${deploy_script})
elseif(LINUX)
    install(FILES resources/video2x-qt6.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
    )
    install(FILES resources/video2x.png
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/1024x1024/apps
    )
endif()
