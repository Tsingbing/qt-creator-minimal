include(../../qtcreator.pri)

TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS  += \
    #canTool/cantool.pro \
    #DDSMonitor/ddsmonitor.pro \
    coreplugin \
#    welcome \
#    helloworld \
#    firecat_toolbar \

QMAKE_EXTRA_TARGETS += deployqt # dummy
