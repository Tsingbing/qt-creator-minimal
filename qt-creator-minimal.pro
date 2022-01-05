include(qtcreator.pri)

#version check qt
!minQtVersion(5, 11, 0) {
    message("Cannot build $$IDE_DISPLAY_NAME with Qt version $${QT_VERSION}.")
    error("Use at least Qt 5.11.0.")
}

TEMPLATE = subdirs
CONFIG   += ordered

SUBDIRS += \
    share \
    src \



