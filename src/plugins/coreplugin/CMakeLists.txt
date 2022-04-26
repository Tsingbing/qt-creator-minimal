add_qtc_plugin(Core
  DEPENDS Qt5::PrintSupport Qt5::Qml Qt5::Sql Qt5::Gui Qt5::GuiPrivate
  PUBLIC_DEPENDS Aggregation ExtensionSystem Utils app_version
  SOURCES
    actionmanager/actioncontainer.cpp actionmanager/actioncontainer.h actionmanager/actioncontainer_p.h
    actionmanager/actionmanager.cpp actionmanager/actionmanager.h actionmanager/actionmanager_p.h
    actionmanager/command.cpp actionmanager/command.h actionmanager/command_p.h
    actionmanager/commandbutton.cpp actionmanager/commandbutton.h
    actionmanager/commandmappings.cpp actionmanager/commandmappings.h
    actionmanager/commandsfile.cpp actionmanager/commandsfile.h
    core.qrc
    core_global.h
    coreconstants.h
    coreicons.cpp coreicons.h
    coreplugin.cpp coreplugin.h
    dialogs/ioptionspage.cpp dialogs/ioptionspage.h
    dialogs/promptoverwritedialog.cpp dialogs/promptoverwritedialog.h
    dialogs/restartdialog.cpp dialogs/restartdialog.h
    dialogs/settingsdialog.cpp dialogs/settingsdialog.h
    fancyactionbar.cpp fancyactionbar.h
    fancyactionbar.qrc
    fancytabwidget.cpp fancytabwidget.h
    featureprovider.cpp featureprovider.h
    fileiconprovider.cpp fileiconprovider.h
    fileutils.cpp fileutils.h
    generalsettings.cpp generalsettings.h generalsettings.ui
    helpitem.cpp helpitem.h
    helpmanager.cpp helpmanager.h helpmanager_implementation.h
    icontext.cpp icontext.h
    icore.cpp icore.h
    id.cpp id.h
    imode.cpp imode.h
    infobar.cpp infobar.h
    iwelcomepage.cpp iwelcomepage.h
    mainwindow.cpp mainwindow.h
    manhattanstyle.cpp manhattanstyle.h
    messagebox.cpp messagebox.h
    minisplitter.cpp minisplitter.h
    modemanager.cpp modemanager.h
    plugindialog.cpp plugindialog.h
    settingsdatabase.cpp settingsdatabase.h
    shellcommand.cpp shellcommand.h
    statusbarmanager.cpp statusbarmanager.h
    styleanimator.cpp styleanimator.h
    themechooser.cpp themechooser.h
    variablechooser.cpp variablechooser.h
    versiondialog.cpp versiondialog.h
    welcomepagehelper.cpp welcomepagehelper.h
    windowsupport.cpp windowsupport.h
  #EXPLICIT_MOC dialogs/filepropertiesdialog.h
)

extend_qtc_plugin(Core
  CONDITION WITH_TESTS
  SOURCES
    testdatadir.cpp testdatadir.h
)
