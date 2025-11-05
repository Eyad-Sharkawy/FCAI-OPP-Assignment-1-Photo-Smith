# Photo Smith v3.5.0 - CMake Build Configuration
# 
# This CMake configuration file defines the build system for Photo Smith,
# a Qt-based image processing application. It configures the project to use
# C++20, Qt 6, and includes all necessary source files and dependencies.
#
# Author: Photo Smith Development Team
# Institution: Faculty of Computers and Artificial Intelligence, Cairo University
# Version: 3.5.0
# Date: October 13, 2025

cmake_minimum_required(VERSION 3.20)
project(PhotoSmith VERSION 3.5.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable resource compiler for Windows
if(WIN32)
    enable_language(RC)
endif()

# Find Qt6 components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Multimedia MultimediaWidgets)

# Enable Qt's MOC, UIC, and RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Include directories
include_directories(src/core)
include_directories(src/core/image)
include_directories(src/core/filters)
include_directories(src/core/history)
include_directories(src/core/io)
include_directories(third_party/stb)

# Source files
set(SOURCES
    src/gui/photo_smith.cpp
    src/gui/ColorWheelDialog.cpp
    src/core/filters/ImageFilters.cpp
    src/core/image/Image_Class.cpp
)

# Header files
set(HEADERS
    src/core/image/Image_Class.h
    src/core/filters/ImageFilters.h
    src/core/history/HistoryManager.h
    src/core/io/ImageIO.h
    src/gui/ColorWheelDialog.h
)

# UI files
set(UI_FILES
    src/gui/mainwindow.ui
)

# Qt resource files
set(QT_RESOURCES
    resources.qrc
)

# Create executable
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${UI_FILES} ${QT_RESOURCES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${UI_FILES} ${QT_RESOURCES})
endif()

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} 
    Qt6::Core 
    Qt6::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
)

# Set output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Windows specific settings
if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
    
    
    # Set application icon using Qt's RC_ICONS property
    if(EXISTS "${CMAKE_SOURCE_DIR}/assets/icons/icon.ico")
        set_target_properties(${PROJECT_NAME} PROPERTIES
            RC_ICONS "${CMAKE_SOURCE_DIR}/assets/icons/icon.ico"
        )
    endif()
    
    # Copy the PNG icon to build directory for runtime access
    if(EXISTS "${CMAKE_SOURCE_DIR}/assets/icons/logo.png")
        configure_file(
            "${CMAKE_SOURCE_DIR}/assets/icons/logo.png"
            "${CMAKE_BINARY_DIR}/bin/logo.png"
            COPYONLY
        )
    endif()
endif()

# Compiler flags
if(MSVC)
    # MSVC does not support GCC/Clang style -Wno-* flags; keep defaults
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wno-missing-field-initializers
    )
endif()
