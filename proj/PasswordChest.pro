#-------------------------------------------------
#
# Project created by QtCreator 2021-05-12T21:16:28
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

#---------------------------------------------------------------------------------------------------
# Version
#---------------------------------------------------------------------------------------------------
DEFINES += MAJOR_VERSION=1
DEFINES += MINOR_VERSION=0

#---------------------------------------------------------------------------------------------------
# Target
#---------------------------------------------------------------------------------------------------
CONFIG(debug, debug|release) {
TARGET = PasswordChest_d
DEFINES += DEBUG
} else {
TARGET = PasswordChest
}

#---------------------------------------------------------------------------------------------------
# Destination folder
#---------------------------------------------------------------------------------------------------

DESTDIR=../bin

#---------------------------------------------------------------------------------------------------
# Source files
#---------------------------------------------------------------------------------------------------

SOURCES += \
    src/main.cpp \
    src/MainWindow.cpp \
    src/PasswordModel.cpp \
    src/PasswordDialog.cpp \
    src/AboutDialog.cpp \
    src/RecentFilesMenu.cpp \
    src/Languages.cpp

HEADERS += \
    src/MainWindow.h \
    src/PasswordModel.h \
    src/PasswordDialog.h \
    src/AboutDialog.h \
    src/RecentFilesMenu.h \
    src/Languages.h

FORMS += \
    src/MainWindow.ui \
    src/PasswordDialog.ui \
    src/AboutDialog.ui

#---------------------------------------------------------------------------------------------------
# Resource files
#---------------------------------------------------------------------------------------------------

RESOURCES += res/resource.qrc

win32:RC_FILE = res/resource_win32.rc


#---------------------------------------------------------------------------------------------------
# Translation files
#---------------------------------------------------------------------------------------------------

TRANSLATIONS =	translate/password_chest_en.ts \
                translate/password_chest_es.ts

