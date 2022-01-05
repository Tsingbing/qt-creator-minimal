include(../../qtcreator.pri)

TEMPLATE = subdirs
CONFIG   += ordered

SUBDIRS  += \
    utils \
    aggregation \
    extensionsystem \


QMAKE_EXTRA_TARGETS += deployqt # dummy
