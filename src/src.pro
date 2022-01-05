TEMPLATE  = subdirs
CONFIG   += ordered

SUBDIRS += \
    libs \
    app \
    plugins \

# delegate deployqt target
deployqt.CONFIG += recursive
deployqt.recurse = shared libs app plugins tools
QMAKE_EXTRA_TARGETS += deployqt
