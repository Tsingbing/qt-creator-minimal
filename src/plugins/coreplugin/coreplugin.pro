DEFINES += CORE_LIBRARY
QT += \
    network \
    printsupport \
    qml \
    sql

# embedding build time information prevents repeatedly binary exact versions from same source code
isEmpty(QTC_SHOW_BUILD_DATE): QTC_SHOW_BUILD_DATE = $$(QTC_SHOW_BUILD_DATE)
!isEmpty(QTC_SHOW_BUILD_DATE): DEFINES += QTC_SHOW_BUILD_DATE

include(../../qtcreatorplugin.pri)
msvc: QMAKE_CXXFLAGS += -wd4251 -wd4290 -wd4250
SOURCES += mainwindow.cpp \
    shellcommand.cpp \
    editmode.cpp \
    fancyactionbar.cpp \
    fancytabwidget.cpp \
    generalsettings.cpp \
    themechooser.cpp \
    id.cpp \
    icontext.cpp \
    statusbarmanager.cpp \
    versiondialog.cpp \
    editormanager/editorarea.cpp \
    editormanager/editormanager.cpp \
    editormanager/editorview.cpp \
    editormanager/editorwindow.cpp \
    editormanager/documentmodel.cpp \
    editormanager/ieditorfactory.cpp \
    editormanager/iexternaleditor.cpp \
    actionmanager/actionmanager.cpp \
    actionmanager/command.cpp \
    actionmanager/commandbutton.cpp \
    actionmanager/actioncontainer.cpp \
    actionmanager/commandsfile.cpp \
    dialogs/settingsdialog.cpp \
    actionmanager/commandmappings.cpp \
    reaper.cpp \
    coreplugin.cpp \
    modemanager.cpp \
    generatedfile.cpp \
    plugindialog.cpp \
    manhattanstyle.cpp \
    minisplitter.cpp \
    styleanimator.cpp \
    fileiconprovider.cpp \
    icore.cpp \
    infobar.cpp \
    editormanager/ieditor.cpp \
    dialogs/ioptionspage.cpp \
    settingsdatabase.cpp \
    imode.cpp \
    editormanager/systemeditor.cpp \
    editortoolbar.cpp \
    helpitem.cpp \
    helpmanager.cpp \
    variablechooser.cpp \
    dialogs/promptoverwritedialog.cpp \
    fileutils.cpp \
    featureprovider.cpp \
    idocument.cpp \
    idocumentfactory.cpp \
    textdocument.cpp \
    documentmanager.cpp \
    dialogs/restartdialog.cpp \
    windowsupport.cpp \
    messagebox.cpp \
    iwelcomepage.cpp \
    coreicons.cpp \
    diffservice.cpp \
    welcomepagehelper.cpp

HEADERS += mainwindow.h \
    shellcommand.h \
    editmode.h \
    fancyactionbar.h \
    fancytabwidget.h \
    generalsettings.h \
    themechooser.h \
    id.h \
    statusbarmanager.h \
    editormanager/editorarea.h \
    editormanager/editormanager.h \
    editormanager/editormanager_p.h \
    editormanager/editorview.h \
    editormanager/editorwindow.h \
    editormanager/documentmodel.h \
    editormanager/ieditor.h \
    editormanager/iexternaleditor.h \
    editormanager/ieditorfactory.h \
    actionmanager/actioncontainer.h \
    actionmanager/actionmanager.h \
    actionmanager/command.h \
    actionmanager/commandbutton.h \
    actionmanager/actionmanager_p.h \
    actionmanager/command_p.h \
    actionmanager/actioncontainer_p.h \
    actionmanager/commandsfile.h \
    dialogs/settingsdialog.h \
    actionmanager/commandmappings.h \
    dialogs/ioptionspage.h \
    reaper.h \
    reaper_p.h \
    icontext.h \
    icore.h \
    infobar.h \
    imode.h \
    coreconstants.h \
    versiondialog.h \
    core_global.h \
    coreplugin.h \
    modemanager.h \
    generatedfile.h \
    plugindialog.h \
    manhattanstyle.h \
    minisplitter.h \
    styleanimator.h \
    fileiconprovider.h \
    settingsdatabase.h \
    editormanager/systemeditor.h \
    editortoolbar.h \
    helpitem.h \
    helpmanager.h \
    helpmanager_implementation.h \
    variablechooser.h \
    dialogs/promptoverwritedialog.h \
    fileutils.h \
    generatedfile.h \
    featureprovider.h \
    idocument.h \
    idocumentfactory.h \
    textdocument.h \
    documentmanager.h \
    dialogs/restartdialog.h \
    windowsupport.h \
    messagebox.h \
    iwelcomepage.h \
    coreicons.h \
    editormanager/documentmodel_p.h \
    diffservice.h \
    editormanager/ieditorfactory_p.h \
    welcomepagehelper.h

FORMS +=  generalsettings.ui 


RESOURCES += core.qrc \
    fancyactionbar.qrc

win32 {
    QT += gui-private # Uses QPlatformNativeInterface.
    LIBS += -lole32 -luser32
}
else:macx {
    OBJECTIVE_SOURCES += \
    LIBS += -framework AppKit
}
else:unix {

    IMAGE_SIZE_LIST = 16 24 32 48 64 128 256 512

    for(imagesize, IMAGE_SIZE_LIST) {
        eval(image$${imagesize}.files = images/logo/$${imagesize}/QtProject-qtcreator.png)
        eval(image$${imagesize}.path = $$QTC_PREFIX/share/icons/hicolor/$${imagesize}x$${imagesize}/apps)
        INSTALLS += image$${imagesize}
    }
}

equals(TEST, 1) {
    SOURCES += testdatadir.cpp
    HEADERS += testdatadir.h
}
