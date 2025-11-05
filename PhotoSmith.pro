# Photo Smith v3.5.0 - Qt Project File (qmake)
# 
# This qmake project file defines the build configuration for Photo Smith
# using Qt's qmake build system. It configures the project to use C++20,
# Qt 6, and includes all necessary source files and dependencies.
#
# Note: This is the legacy build system. CMake is the recommended build system.
#
# Author: Photo Smith Development Team
# Institution: Faculty of Computers and Artificial Intelligence, Cairo University
# Version: 3.5.0
# Date: October 13, 2025

QT += core widgets multimedia multimediawidgets


CONFIG += c++20

TARGET = PhotoSmith
TEMPLATE = app

SOURCES += src/gui/photo_smith.cpp \
           src/gui/ColorWheelDialog.cpp \
           src/core/filters/ImageFilters.cpp \
           src/core/image/Image_Class.cpp

HEADERS += src/core/image/Image_Class.h \
           src/core/filters/ImageFilters.h \
           src/gui/ColorWheelDialog.h

FORMS += src/gui/mainwindow.ui

# Include paths for headers and third-party libraries
INCLUDEPATH += src/core \
               third_party/stb

QMAKE_CXXFLAGS += -std=c++20

# Disable warnings for STB library
QMAKE_CXXFLAGS += -Wno-missing-field-initializers

# Windows: embed executable icon if .ico exists
win32:exists(assets/icons/logo.ico) {
    ICON = assets/icons/logo.ico
}

RESOURCES += \
    resources.qrc
