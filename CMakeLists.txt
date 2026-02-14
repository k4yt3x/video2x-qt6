cmake_minimum_required(VERSION 3.19)
project(video2x-qt6 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(VIDEO2X_ENABLE_NATIVE "Enable native optimizations (-march=native)" OFF)
option(VIDEO2X_ENABLE_X86_64_V4 "Enable x86-64-v4 optimizations (-march=x86-64-v4)" OFF)
option(VIDEO2X_ENABLE_X86_64_V3 "Enable x86-64-v3 optimizations (-march=x86-64-v3)" OFF)

option(USE_EXTERNAL_SPDLOG "Use the system-provided spdlog library" ON)
option(USE_EXTERNAL_VIDEO2X "Use the system-provided video2x" OFF)
option(USE_SHARED_VIDEO2X "Use the shared video2x library" OFF)
option(VIDEO2X_BUILD_APPIMAGE "Prepare install layout for AppImage packaging" OFF)

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
elseif(VIDEO2X_ENABLE_X86_64_V3)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        add_compile_options(/arch:AVX2)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-march=x86-64-v3)
    endif()
endif()

# Define lists to store include directories and libraries
set(VIDEO2X_QT6_INCLUDE_DIRS)
set(VIDEO2X_QT6_LIBS)

# Find the required Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Svg Network LinguistTools)
qt_standard_project_setup()
list(APPEND VIDEO2X_QT6_LIBS Qt6::Core Qt6::Widgets Qt6::Svg Qt6::Network)

# Power management dependencies
if(WIN32)
    list(APPEND VIDEO2X_QT6_LIBS PowrProf.lib)
else()
    list(APPEND VIDEO2X_QT6_LIBS Qt6::DBus)
endif()

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
        BINARY_DIR ${CMAKE_BINARY_DIR}/video2x-build
        INSTALL_DIR ${CMAKE_BINARY_DIR}/video2x-install
        CMAKE_ARGS
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}

            # Shared library (LGPL compliance for FFmpeg)
            -DBUILD_SHARED_LIBS=ON

            # Build bundled ncnn, spdlog, boost
            -DVIDEO2X_USE_EXTERNAL_NCNN=OFF
            -DVIDEO2X_USE_EXTERNAL_SPDLOG=OFF
            -DVIDEO2X_USE_EXTERNAL_BOOST=OFF

            # rmsnorm depends on layernorm shaders in this ncnn version
            -DWITH_LAYER_layernorm=ON
        BUILD_ALWAYS ON
        INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install --config $<CONFIG>
    )
endif()

# Define the list of translation files
set(TS_FILES src/video2x-qt6_en_US.ts
             src/video2x-qt6_zh_CN.ts
             src/video2x-qt6_ja_JP.ts
             src/video2x-qt6_pt_PT.ts
             src/video2x-qt6_fr_FR.ts
             src/video2x-qt6_de_DE.ts
             src/video2x-qt6_ko_KR.ts
             src/video2x-qt6_ko_KR.ts
             src/video2x-qt6_uk_UA.ts)

# Define the project source files
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
    src/preferencesdialog.cpp
    src/preferencesdialog.h
    src/preferencesdialog.ui
    src/preferencesmanager.cpp
    src/preferencesmanager.h
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
    src/translations.qrc
    src/images.qrc
    src/utils.cpp
    src/utils.h
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

qt_add_resources(RESOURCES src/images.qrc)
target_sources(video2x-qt6 PRIVATE ${RESOURCES})

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
    ${CMAKE_BINARY_DIR}/video2x-install/include
)

if(NOT USE_EXTERNAL_VIDEO2X AND NOT USE_SHARED_VIDEO2X)
    add_dependencies(video2x-qt6 Video2X)
    if(WIN32)
        set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/video2x-install/lib/libvideo2x.lib)
    else()
        set(LIBVIDEO2X_LIB ${CMAKE_BINARY_DIR}/video2x-install/lib/libvideo2x.so)

        # Create a symlink to models directory next to the executable
        add_custom_command(TARGET video2x-qt6 POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink
                "${CMAKE_BINARY_DIR}/video2x-install/share/video2x/models"
                "$<TARGET_FILE_DIR:video2x-qt6>/models"
            COMMENT "Creating symlink to video2x models"
        )
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
    # --- Desktop integration files ---
    install(FILES resources/video2x-qt6.desktop
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications
    )

    # Install all icon sizes
    foreach(SIZE 16x16 24x24 32x32 48x48 64x64 128x128 256x256 512x512 1024x1024)
        install(DIRECTORY resources/icons/${SIZE}/
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/${SIZE}/apps
            FILES_MATCHING PATTERN "*.png"
        )
    endforeach()

    # --- libvideo2x shared library + models (bundled build only) ---
    if(NOT USE_EXTERNAL_VIDEO2X AND NOT USE_SHARED_VIDEO2X)
        # Install libvideo2x.so and any other .so from the ExternalProject
        install(DIRECTORY ${CMAKE_BINARY_DIR}/video2x-install/lib/
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
            FILES_MATCHING
                PATTERN "*.so"
                PATTERN "*.so.*"
            PATTERN "cmake" EXCLUDE
            PATTERN "pkgconfig" EXCLUDE
        )

        # Install models alongside the data files
        install(DIRECTORY ${CMAKE_BINARY_DIR}/video2x-install/share/video2x/models
            DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/video2x
        )

        # Create a models symlink next to the binary so libvideo2x can find them
        install(CODE "
            execute_process(COMMAND \${CMAKE_COMMAND} -E create_symlink
                \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATAROOTDIR}/video2x/models\"
                \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_BINDIR}/models\"
            )
        ")
    endif()
endif()

# ============================================================================
# AppImage packaging target (Linux only)
# ============================================================================
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND VIDEO2X_BUILD_APPIMAGE)
    # Resolve the Qt6 installation prefix from the imported target
    get_target_property(_qt6_core_loc Qt6::Core LOCATION)
    get_filename_component(_qt6_lib_dir "${_qt6_core_loc}" DIRECTORY)
    get_filename_component(QT6_PREFIX "${_qt6_lib_dir}/.." ABSOLUTE)
    message(STATUS "Qt6 prefix for AppImage: ${QT6_PREFIX}")

    set(APPDIR "${CMAKE_BINARY_DIR}/AppDir")

    # Allow specifying local paths for linuxdeploy tools
    set(LINUXDEPLOY_LOCAL "" CACHE FILEPATH "Path to local linuxdeploy-x86_64.AppImage")
    set(LINUXDEPLOY_QT_LOCAL "" CACHE FILEPATH "Path to local linuxdeploy-plugin-qt-x86_64.AppImage")

    set(LINUXDEPLOY "${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage")
    set(LINUXDEPLOY_QT "${CMAKE_BINARY_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage")

    # Copy from local path if specified, otherwise download
    if(LINUXDEPLOY_LOCAL AND EXISTS "${LINUXDEPLOY_LOCAL}")
        message(STATUS "Using local linuxdeploy from ${LINUXDEPLOY_LOCAL}")
        file(COPY "${LINUXDEPLOY_LOCAL}" DESTINATION "${CMAKE_BINARY_DIR}")
        file(RENAME "${CMAKE_BINARY_DIR}/linuxdeploy-x86_64.AppImage" "${LINUXDEPLOY}")
        file(CHMOD "${LINUXDEPLOY}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
    elseif(NOT EXISTS "${LINUXDEPLOY}")
        message(STATUS "Downloading linuxdeploy...")
        file(DOWNLOAD
            "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
            "${LINUXDEPLOY}"
        )
        file(CHMOD "${LINUXDEPLOY}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
    endif()

    if(LINUXDEPLOY_QT_LOCAL AND EXISTS "${LINUXDEPLOY_QT_LOCAL}")
        message(STATUS "Using local linuxdeploy-plugin-qt from ${LINUXDEPLOY_QT_LOCAL}")
        file(COPY "${LINUXDEPLOY_QT_LOCAL}" DESTINATION "${CMAKE_BINARY_DIR}")
        file(RENAME "${CMAKE_BINARY_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage" "${LINUXDEPLOY_QT}")
        file(CHMOD "${LINUXDEPLOY_QT}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
    elseif(NOT EXISTS "${LINUXDEPLOY_QT}")
        message(STATUS "Downloading linuxdeploy-plugin-qt...")
        file(DOWNLOAD
            "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"
            "${LINUXDEPLOY_QT}"
        )
        file(CHMOD "${LINUXDEPLOY_QT}" PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE)
    endif()

    # Create the AppRun script as a file at configure time
    file(WRITE "${CMAKE_BINARY_DIR}/AppRun.in"
"#!/bin/bash
HERE=\"$(dirname \"$(readlink -f \"$0\")\")\"\nexport PATH=\"$HERE/usr/bin:$PATH\"\nexport LD_LIBRARY_PATH=\"$HERE/usr/lib:$LD_LIBRARY_PATH\"\nexport QT_PLUGIN_PATH=\"$HERE/usr/plugins\"\nexec \"$HERE/usr/bin/video2x-qt6\" \"$@\"\n")



    # Allow manual override of libtiff5 path
    set(LIBTIFF5_LIB_PATH "" CACHE PATH "Path to libtiff5 if needed for Qt 6.10+ Kubuntu 24.04")

    if(LIBTIFF5_LIB_PATH AND EXISTS "${LIBTIFF5_LIB_PATH}")
        set(LIBTIFF5_PATH ":${LIBTIFF5_LIB_PATH}")
    elseif(NOT EXISTS "${LIBTIFF5_LIB_PATH}")
        set(LIBTIFF5_PATH "")
    endif()


    # Custom target: build the AppImage
    add_custom_target(appimage
        DEPENDS video2x-qt6
        COMMENT "Building AppImage..."

        # Step 1: Clean and create AppDir via cmake --install
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${APPDIR}"
        COMMAND ${CMAKE_COMMAND} --install "${CMAKE_BINARY_DIR}"
            --prefix "${APPDIR}/usr"
            --config $<CONFIG>

        # Step 2: Ensure models symlink inside AppDir
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "../share/video2x/models"
            "${APPDIR}/usr/bin/models"

        # Step 3: Copy AppRun and make executable
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_BINARY_DIR}/AppRun.in"
            "${APPDIR}/AppRun"
        COMMAND chmod +x "${APPDIR}/AppRun"

        # Step 4: Copy .desktop and icon to AppDir root
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/resources/video2x-qt6.desktop"
            "${APPDIR}/video2x-qt6.desktop"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_SOURCE_DIR}/resources/icons/256x256/video2x.png"
            "${APPDIR}/video2x.png"

            # Step 5: Run linuxdeploy with Qt plugin
            #   LD_LIBRARY_PATH order matters:
            #   1. AppDir/usr/lib - already installed libs
            #   2. Qt6 lib dir - so linuxdeploy bundles the CORRECT Qt version
            #   3. video2x-install/lib - bundled ncnn/realesrgan/etc .so files
            #   4. libtiff5 - extra dependency not in system
            COMMAND ${CMAKE_COMMAND} -E echo
                "=== AppImage linuxdeploy command ==="
            COMMAND ${CMAKE_COMMAND} -E echo
                "QMAKE=${QT6_PREFIX}/bin/qmake"
            COMMAND ${CMAKE_COMMAND} -E echo
                "LD_LIBRARY_PATH=${APPDIR}/usr/lib:${QT6_PREFIX}/lib:${CMAKE_BINARY_DIR}/video2x-install/lib${LIBTIFF5_PATH}"
            COMMAND ${CMAKE_COMMAND} -E echo
                "LINUXDEPLOY=${LINUXDEPLOY}"
            COMMAND ${CMAKE_COMMAND} -E echo
                "APPDIR=${APPDIR}"
            COMMAND ${CMAKE_COMMAND} -E echo
                "DESKTOP=${APPDIR}/video2x-qt6.desktop"
            COMMAND ${CMAKE_COMMAND} -E echo
                "ICON=${APPDIR}/video2x.png"
            COMMAND ${CMAKE_COMMAND} -E echo
                "==================================="
            COMMAND ${CMAKE_COMMAND} -E env
                --unset=LD_LIBRARY_PATH
                "LD_LIBRARY_PATH=${APPDIR}/usr/lib:${QT6_PREFIX}/lib:${CMAKE_BINARY_DIR}/video2x-install/lib${LIBTIFF5_PATH}"
                "QMAKE=${QT6_PREFIX}/bin/qmake"
                "${LINUXDEPLOY}"
                    --appdir "${APPDIR}"
                    --plugin qt
                    --desktop-file "${APPDIR}/video2x-qt6.desktop"
                    --icon-file "${APPDIR}/video2x.png"
                    --output appimage

        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
endif()
