include(../../qtcreator.pri)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS  += \
    coreplugin \
    welcome \
    helloworld \
    firecat_toolbar \

QMAKE_EXTRA_TARGETS += deployqt # dummy
