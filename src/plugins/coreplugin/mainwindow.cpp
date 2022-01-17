/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#include "mainwindow.h"

#include "coreicons.h"

#include "fancytabwidget.h"
#include "generalsettings.h"
#include "icore.h"
#include "id.h"
#include "manhattanstyle.h"

#include "modemanager.h"
#include "plugindialog.h"
#include "statusbarmanager.h"

#include "versiondialog.h"
#include "windowsupport.h"

#include <app/app_version.h>
#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/actionmanager_p.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/settingsdatabase.h>
#include <extensionsystem/pluginmanager.h>
#include <utils/algorithm.h>
#include <utils/historycompleter.h>
#include <utils/hostosinfo.h>
#include <utils/mimetypes/mimedatabase.h>
#include <utils/qtcassert.h>
#include <utils/stringutils.h>
#include <utils/stylehelper.h>
#include <utils/theme/theme.h>
#include <utils/utilsicons.h>

#include <QApplication>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QPrinter>
#include <QSettings>
#include <QStatusBar>
#include <QStyleFactory>
#include <QTimer>
#include <QToolButton>
#include <QUrl>

using namespace ExtensionSystem;
using namespace Utils;

namespace Core
{
namespace Internal
{

enum
{
    debugMainWindow = 0
};

MainWindow::MainWindow()
    : AppMainWindow()
    , m_coreImpl(new ICore(this))
    , m_lowPrioAdditionalContexts(Constants::C_GLOBAL)
    , m_settingsDatabase(
          new SettingsDatabase(QFileInfo(PluginManager::settings()->fileName()).path(),
                               QLatin1String(Constants::IDE_CASED_ID),
                               this))
    , m_modeStack(new FancyTabWidget(this))
    , m_generalSettings(new GeneralSettings) //, m_systemEditor(new SystemEditor)
    , m_toggleLeftSideBarButton(new QToolButton)
    , m_toggleRightSideBarButton(new QToolButton)
{
    //(void)new DocumentManager(this);

    HistoryCompleter::setSettings(PluginManager::settings());

    setWindowTitle(Constants::IDE_DISPLAY_NAME);
    //Remove title box
    //setWindowFlags(Qt::FramelessWindowHint);
    if (HostOsInfo::isLinuxHost())
        QApplication::setWindowIcon(Icons::QTCREATORLOGO_BIG.icon());
    QString baseName = QApplication::style()->objectName();
    // Sometimes we get the standard windows 95 style as a fallback
    if (HostOsInfo::isAnyUnixHost() && !HostOsInfo::isMacHost()
        && baseName == QLatin1String("windows"))
    {
        baseName = QLatin1String("fusion");
    }

    // if the user has specified as base style in the theme settings,
    // prefer that
    const QStringList available = QStyleFactory::keys();
    foreach (const QString& s, Utils::creatorTheme()->preferredStyles())
    {
        if (available.contains(s, Qt::CaseInsensitive))
        {
            baseName = s;
            break;
        }
    }

    QApplication::setStyle(new ManhattanStyle(baseName));
    m_generalSettings->setShowShortcutsInContextMenu(
        m_generalSettings->showShortcutsInContextMenu());

    setDockNestingEnabled(true);

    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    m_modeManager = new ModeManager(this, m_modeStack);
    connect(m_modeStack, &FancyTabWidget::topAreaClicked, this, [](Qt::MouseButton, Qt::KeyboardModifiers modifiers) {
        if (modifiers & Qt::ShiftModifier)
        {
            QColor color = QColorDialog::getColor(StyleHelper::requestedBaseColor(), ICore::dialogParent());
            if (color.isValid())
                StyleHelper::setBaseColor(color);
        }
    });

    registerDefaultContainers();
    registerDefaultActions();

    setCentralWidget(m_modeStack);

    connect(qApp, &QApplication::focusChanged, this, &MainWindow::updateFocusWidget);

    // Add small Toolbuttons for toggling the navigation widgets
    StatusBarManager::addStatusBarWidget(m_toggleLeftSideBarButton, StatusBarManager::First);
    int childsCount = statusBar()->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly).count();
    statusBar()->insertPermanentWidget(childsCount - 1, m_toggleRightSideBarButton); // before QSizeGrip

    statusBar()->setProperty("p_styled", true);
}

void MainWindow::setSidebarVisible(bool visible, Side side)
{
    //if (NavigationWidgetPlaceHolder::current(side))
    //    navigationWidget(side)->setShown(visible);
}

void MainWindow::setOverrideColor(const QColor& color)
{
    m_overrideColor = color;
}

QStringList MainWindow::additionalAboutInformation() const
{
    return m_aboutInformation;
}

void MainWindow::appendAboutInformation(const QString& line)
{
    m_aboutInformation.append(line);
}

void MainWindow::addPreCloseListener(const std::function<bool()>& listener)
{
    m_preCloseListeners.append(listener);
}

MainWindow::~MainWindow()
{
    // explicitly delete window support, because that calls methods from ICore that call methods
    // from mainwindow, so mainwindow still needs to be alive
    delete m_windowSupport;
    m_windowSupport = nullptr;

    delete m_generalSettings;
    m_generalSettings = nullptr;

    delete m_printer;
    m_printer = nullptr;

    delete m_coreImpl;
    m_coreImpl = nullptr;

    delete m_modeManager;
    m_modeManager = nullptr;
}

void MainWindow::init()
{
    //MessageManager::init();
}

void MainWindow::extensionsInitialized()
{

    m_windowSupport = new WindowSupport(this, Context("Core.MainWindow"));
    m_windowSupport->setCloseActionEnabled(false);

    ModeManager::extensionsInitialized();

    readSettings();
    updateContext();

    emit m_coreImpl->coreAboutToOpen();
    // Delay restoreWindowState, since it is overridden by LayoutRequest event
    QTimer::singleShot(0, this, &MainWindow::restoreWindowState);
    QTimer::singleShot(0, m_coreImpl, &ICore::coreOpened);
}

static void setRestart(bool restart)
{
    qApp->setProperty("restart", restart);
}

void MainWindow::restart()
{
    setRestart(true);
    exit();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    const auto cancelClose = [ event ] {
        event->ignore();
        setRestart(false);
    };

    // work around QTBUG-43344
    static bool alreadyClosed = false;
    if (alreadyClosed)
    {
        event->accept();
        return;
    }

    ICore::saveSettings(ICore::MainWindowClosing);

    foreach (const std::function<bool()>& listener, m_preCloseListeners)
    {
        if (!listener())
        {
            cancelClose();
            return;
        }
    }

    emit m_coreImpl->coreAboutToClose();

    saveWindowSettings();

    event->accept();
    alreadyClosed = true;
}

IContext* MainWindow::currentContextObject() const
{
    return m_activeContext.isEmpty() ? nullptr : m_activeContext.first();
}

QStatusBar* MainWindow::statusBar() const
{
    return m_modeStack->statusBar();
}

InfoBar* MainWindow::infoBar() const
{
    return m_modeStack->infoBar();
}

void MainWindow::registerDefaultContainers()
{
    ActionContainer* menubar = ActionManager::createMenuBar(Constants::MENU_BAR);

    if (!HostOsInfo::isMacHost()) // System menu bar on Mac
        setMenuBar(menubar->menuBar());
    menubar->appendGroup(Constants::G_FILE);
    //menubar->appendGroup(Constants::G_EDIT);
    //menubar->appendGroup(Constants::G_VIEW);
    menubar->appendGroup(Constants::G_TOOLS);
    menubar->appendGroup(Constants::G_WINDOW);
    menubar->appendGroup(Constants::G_HELP);

    // File Menu
    ActionContainer* filemenu = ActionManager::createMenu(Constants::M_FILE);
    menubar->addMenu(filemenu, Constants::G_FILE);
    filemenu->menu()->setTitle(tr("&File"));
    filemenu->appendGroup(Constants::G_FILE_OTHER);

    // Tools Menu
    ActionContainer* ac = ActionManager::createMenu(Constants::M_TOOLS);
    menubar->addMenu(ac, Constants::G_TOOLS);
    ac->menu()->setTitle(tr("&Tools"));

    // Window Menu
    ActionContainer* mwindow = ActionManager::createMenu(Constants::M_WINDOW);
    menubar->addMenu(mwindow, Constants::G_WINDOW);
    mwindow->menu()->setTitle(tr("&Window"));
    mwindow->appendGroup(Constants::G_WINDOW_SIZE);
    mwindow->appendGroup(Constants::G_WINDOW_LIST);
    mwindow->appendGroup(Constants::G_WINDOW_OTHER);

    // Help Menu
    ac = ActionManager::createMenu(Constants::M_HELP);
    menubar->addMenu(ac, Constants::G_HELP);
    ac->menu()->setTitle(tr("&Help"));
    ac->appendGroup(Constants::G_HELP_HELP);
    ac->appendGroup(Constants::G_HELP_SUPPORT);
    ac->appendGroup(Constants::G_HELP_ABOUT);
    ac->appendGroup(Constants::G_HELP_UPDATES);

    // macOS touch bar
    ac = ActionManager::createTouchBar(Constants::TOUCH_BAR,
                                       QIcon(),
                                       "Main TouchBar" /*never visible*/);
    ac->appendGroup(Constants::G_TOUCHBAR_HELP);
    ac->appendGroup(Constants::G_TOUCHBAR_EDITOR);
    ac->appendGroup(Constants::G_TOUCHBAR_NAVIGATION);
    ac->appendGroup(Constants::G_TOUCHBAR_OTHER);
    ac->touchBar()->setApplicationTouchBar();
}

void MainWindow::registerDefaultActions()
{
    ActionContainer* mfile = ActionManager::actionContainer(Constants::M_FILE);
    //ActionContainer* medit   = ActionManager::actionContainer(Constants::M_EDIT);
    ActionContainer* mtools  = ActionManager::actionContainer(Constants::M_TOOLS);
    ActionContainer* mwindow = ActionManager::actionContainer(Constants::M_WINDOW);
    ActionContainer* mhelp   = ActionManager::actionContainer(Constants::M_HELP);

    // File menu separators
    mfile->addSeparator(Constants::G_FILE_OTHER);

    Command* cmd;

    // File->Recent Files Menu
    ActionContainer* ac;
    QAction*         tmpaction;
    QIcon            icon;
    // Exit Action
    icon         = QIcon::fromTheme(QLatin1String("application-exit"));
    m_exitAction = new QAction(icon, tr("E&xit"), this);
    m_exitAction->setMenuRole(QAction::QuitRole);
    cmd = ActionManager::registerAction(m_exitAction, Constants::EXIT);
    cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Q")));
    mfile->addAction(cmd, Constants::G_FILE_OTHER);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::exit);

    // Options Action
    mtools->appendGroup(Constants::G_TOOLS_OPTIONS);
    mtools->addSeparator(Constants::G_TOOLS_OPTIONS);

    m_optionsAction = new QAction(tr("&Options..."), this);
    m_optionsAction->setMenuRole(QAction::PreferencesRole);
    cmd = ActionManager::registerAction(m_optionsAction, Constants::OPTIONS);
    cmd->setDefaultKeySequence(QKeySequence::Preferences);
    mtools->addAction(cmd, Constants::G_TOOLS_OPTIONS);
    connect(m_optionsAction, &QAction::triggered, this, [] { ICore::showOptionsDialog(Id()); });

    mwindow->addSeparator(Constants::G_WINDOW_LIST);

    if (useMacShortcuts)
    {
        // Minimize Action
        QAction* minimizeAction = new QAction(tr("Minimize"), this);
        minimizeAction->setEnabled(false); // actual implementation in WindowSupport
        cmd = ActionManager::registerAction(minimizeAction, Constants::MINIMIZE_WINDOW);
        cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+M")));
        mwindow->addAction(cmd, Constants::G_WINDOW_SIZE);

        // Zoom Action
        QAction* zoomAction = new QAction(tr("Zoom"), this);
        zoomAction->setEnabled(false); // actual implementation in WindowSupport
        cmd = ActionManager::registerAction(zoomAction, Constants::ZOOM_WINDOW);
        mwindow->addAction(cmd, Constants::G_WINDOW_SIZE);
    }

    // Full Screen Action
    QAction* toggleFullScreenAction = new QAction(tr("Full Screen"), this);
    toggleFullScreenAction->setCheckable(true);
    toggleFullScreenAction->setEnabled(false); // actual implementation in WindowSupport
    cmd = ActionManager::registerAction(toggleFullScreenAction, Constants::TOGGLE_FULLSCREEN);
    cmd->setDefaultKeySequence(QKeySequence(useMacShortcuts ? tr("Ctrl+Meta+F") : tr("Ctrl+Shift+F11")));
    if (HostOsInfo::isMacHost())
        cmd->setAttribute(Command::CA_UpdateText);
    mwindow->addAction(cmd, Constants::G_WINDOW_SIZE);

    if (useMacShortcuts)
    {
        mwindow->addSeparator(Constants::G_WINDOW_SIZE);

        QAction* closeAction = new QAction(tr("Close Window"), this);
        closeAction->setEnabled(false);
        cmd = ActionManager::registerAction(closeAction, Constants::CLOSE_WINDOW);
        cmd->setDefaultKeySequence(QKeySequence(tr("Ctrl+Meta+W")));
        mwindow->addAction(cmd, Constants::G_WINDOW_SIZE);

        mwindow->addSeparator(Constants::G_WINDOW_SIZE);
    }

    registerModeSelectorStyleActions();

    // Window->Views
    ActionContainer* mviews = ActionManager::createMenu(Constants::M_WINDOW_VIEWS);
    mwindow->addMenu(mviews, Constants::G_WINDOW_VIEWS);
    mviews->menu()->setTitle(tr("&Views"));

    // "Help" separators
    mhelp->addSeparator(Constants::G_HELP_SUPPORT);
    if (!HostOsInfo::isMacHost())
        mhelp->addSeparator(Constants::G_HELP_ABOUT);

    // About IDE Action
    icon = QIcon::fromTheme(QLatin1String("help-about"));
    if (HostOsInfo::isMacHost())
        tmpaction = new QAction(icon, tr("About &%1").arg(Constants::IDE_DISPLAY_NAME), this); // it's convention not to add dots to the about menu
    else
        tmpaction = new QAction(icon, tr("About &%1...").arg(Constants::IDE_DISPLAY_NAME), this);
    tmpaction->setMenuRole(QAction::AboutRole);
    cmd = ActionManager::registerAction(tmpaction, Constants::ABOUT_QTCREATOR);
    mhelp->addAction(cmd, Constants::G_HELP_ABOUT);
    tmpaction->setEnabled(true);
    connect(tmpaction, &QAction::triggered, this, &MainWindow::aboutQtCreator);

    //About Plugins Action
    tmpaction = new QAction(tr("About &Plugins..."), this);
    tmpaction->setMenuRole(QAction::ApplicationSpecificRole);
    cmd = ActionManager::registerAction(tmpaction, Constants::ABOUT_PLUGINS);
    mhelp->addAction(cmd, Constants::G_HELP_ABOUT);
    tmpaction->setEnabled(true);
    connect(tmpaction, &QAction::triggered, this, &MainWindow::aboutPlugins);

    // About sep
    if (!HostOsInfo::isMacHost())
    { // doesn't have the "About" actions in the Help menu
        tmpaction = new QAction(this);
        tmpaction->setSeparator(true);
        cmd = ActionManager::registerAction(tmpaction, "QtCreator.Help.Sep.About");
        mhelp->addAction(cmd, Constants::G_HELP_ABOUT);
    }
}

void MainWindow::registerModeSelectorStyleActions()
{
    ActionContainer* mwindow = ActionManager::actionContainer(Constants::M_WINDOW);

    // Cycle Mode Selector Styles
    m_cycleModeSelectorStyleAction = new QAction(tr("Cycle Mode Selector Styles"), this);
    ActionManager::registerAction(m_cycleModeSelectorStyleAction, Constants::CYCLE_MODE_SELECTOR_STYLE);
    connect(m_cycleModeSelectorStyleAction, &QAction::triggered, this, [ this ] {
        ModeManager::cycleModeStyle();
        updateModeSelectorStyleMenu();
    });

    // Mode Selector Styles
    ActionContainer* mmodeLayouts = ActionManager::createMenu(Constants::M_WINDOW_MODESTYLES);
    mwindow->addMenu(mmodeLayouts, Constants::G_WINDOW_VIEWS);
    QMenu* styleMenu = mmodeLayouts->menu();
    styleMenu->setTitle(tr("Mode Selector Style"));
    auto* stylesGroup = new QActionGroup(styleMenu);
    stylesGroup->setExclusive(true);

    m_setModeSelectorStyleIconsAndTextAction = stylesGroup->addAction(tr("Icons and Text"));
    connect(m_setModeSelectorStyleIconsAndTextAction, &QAction::triggered,
            [] { ModeManager::setModeStyle(ModeManager::Style::IconsAndText); });
    m_setModeSelectorStyleIconsAndTextAction->setCheckable(true);
    m_setModeSelectorStyleIconsOnlyAction = stylesGroup->addAction(tr("Icons Only"));
    connect(m_setModeSelectorStyleIconsOnlyAction, &QAction::triggered,
            [] { ModeManager::setModeStyle(ModeManager::Style::IconsOnly); });
    m_setModeSelectorStyleIconsOnlyAction->setCheckable(true);
    m_setModeSelectorStyleHiddenAction = stylesGroup->addAction(tr("Hidden"));
    connect(m_setModeSelectorStyleHiddenAction, &QAction::triggered,
            [] { ModeManager::setModeStyle(ModeManager::Style::Hidden); });
    m_setModeSelectorStyleHiddenAction->setCheckable(true);

    styleMenu->addActions(stylesGroup->actions());
}

void MainWindow::exit()
{
    // this function is most likely called from a user action
    // that is from an event handler of an object
    // since on close we are going to delete everything
    // so to prevent the deleting of that object we
    // just append it
    QTimer::singleShot(0, this, &QWidget::close);
}

IContext* MainWindow::contextObject(QWidget* widget)
{
    return m_contextWidgets.value(widget);
}

void MainWindow::addContextObject(IContext* context)
{
    if (!context)
        return;
    QWidget* widget = context->widget();
    if (m_contextWidgets.contains(widget))
        return;

    m_contextWidgets.insert(widget, context);
}

void MainWindow::removeContextObject(IContext* context)
{
    if (!context)
        return;

    QWidget* widget = context->widget();
    if (!m_contextWidgets.contains(widget))
        return;

    m_contextWidgets.remove(widget);
    if (m_activeContext.removeAll(context) > 0)
        updateContextObject(m_activeContext);
}

void MainWindow::updateFocusWidget(QWidget* old, QWidget* now)
{
    Q_UNUSED(old)

    // Prevent changing the context object just because the menu or a menu item is activated
    if (qobject_cast<QMenuBar*>(now) || qobject_cast<QMenu*>(now))
        return;

    QList<IContext*> newContext;
    if (QWidget* p = QApplication::focusWidget())
    {
        IContext* context = nullptr;
        while (p)
        {
            context = m_contextWidgets.value(p);
            if (context)
                newContext.append(context);
            p = p->parentWidget();
        }
    }

    // ignore toplevels that define no context, like popups without parent
    if (!newContext.isEmpty() || QApplication::focusWidget() == focusWidget())
        updateContextObject(newContext);
}

void MainWindow::updateContextObject(const QList<IContext*>& context)
{
    emit m_coreImpl->contextAboutToChange(context);
    m_activeContext = context;
    updateContext();
    if (debugMainWindow)
    {
        qDebug() << "new context objects =" << context;
        foreach (IContext* c, context)
            qDebug() << (c ? c->widget() : nullptr) << (c ? c->widget()->metaObject()->className() : nullptr);
    }
}

void MainWindow::aboutToShutdown()
{
    disconnect(qApp, &QApplication::focusChanged, this, &MainWindow::updateFocusWidget);
    m_activeContext.clear();
    hide();
}

static const char settingsGroup[]         = "MainWindow";
static const char colorKey[]              = "Color";
static const char windowGeometryKey[]     = "WindowGeometry";
static const char windowStateKey[]        = "WindowState";
static const char modeSelectorLayoutKey[] = "ModeSelectorLayout";

void MainWindow::readSettings()
{
    QSettings* settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));

    if (m_overrideColor.isValid())
    {
        StyleHelper::setBaseColor(m_overrideColor);
        // Get adapted base color.
        m_overrideColor = StyleHelper::baseColor();
    }
    else
    {
        StyleHelper::setBaseColor(settings->value(QLatin1String(colorKey),
                                                  QColor(StyleHelper::DEFAULT_BASE_COLOR))
                                      .value<QColor>());
    }

    {
        ModeManager::Style modeStyle =
            ModeManager::Style(settings->value(modeSelectorLayoutKey, int(ModeManager::Style::IconsAndText)).toInt());

        // Migrate legacy setting from Qt Creator 4.6 and earlier
        static const char modeSelectorVisibleKey[] = "ModeSelectorVisible";
        if (!settings->contains(modeSelectorLayoutKey) && settings->contains(modeSelectorVisibleKey))
        {
            bool visible = settings->value(modeSelectorVisibleKey, true).toBool();
            modeStyle    = visible ? ModeManager::Style::IconsAndText : ModeManager::Style::Hidden;
        }

        ModeManager::setModeStyle(modeStyle);
        updateModeSelectorStyleMenu();
    }

    settings->endGroup();

    //EditorManagerPrivate::readSettings();
    //m_leftNavigationWidget->restoreSettings(settings);
    //m_rightNavigationWidget->restoreSettings(settings);
    //m_rightPaneWidget->readSettings(settings);
}

void MainWindow::saveSettings()
{
    QSettings* settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));

    if (!(m_overrideColor.isValid() && StyleHelper::baseColor() == m_overrideColor))
        settings->setValue(QLatin1String(colorKey), StyleHelper::requestedBaseColor());

    settings->endGroup();

    //DocumentManager::saveSettings();
    ActionManager::saveSettings();
    //EditorManagerPrivate::saveSettings();
    //m_leftNavigationWidget->saveSettings(settings);
    //m_rightNavigationWidget->saveSettings(settings);
}

void MainWindow::saveWindowSettings()
{
    QSettings* settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));

    // On OS X applications usually do not restore their full screen state.
    // To be able to restore the correct non-full screen geometry, we have to put
    // the window out of full screen before saving the geometry.
    // Works around QTBUG-45241
    if (Utils::HostOsInfo::isMacHost() && isFullScreen())
        setWindowState(windowState() & ~Qt::WindowFullScreen);
    settings->setValue(QLatin1String(windowGeometryKey), saveGeometry());
    settings->setValue(QLatin1String(windowStateKey), saveState());
    settings->setValue(modeSelectorLayoutKey, int(ModeManager::modeStyle()));

    settings->endGroup();
}

void MainWindow::updateModeSelectorStyleMenu()
{
    switch (ModeManager::modeStyle())
    {
        case ModeManager::Style::IconsAndText:
            m_setModeSelectorStyleIconsAndTextAction->setChecked(true);
            break;
        case ModeManager::Style::IconsOnly:
            m_setModeSelectorStyleIconsOnlyAction->setChecked(true);
            break;
        case ModeManager::Style::Hidden:
            m_setModeSelectorStyleHiddenAction->setChecked(true);
            break;
    }
}

void MainWindow::updateAdditionalContexts(const Context& remove, const Context& add,
                                          ICore::ContextPriority priority)
{
    foreach (const Id id, remove)
    {
        if (!id.isValid())
            continue;
        int index = m_lowPrioAdditionalContexts.indexOf(id);
        if (index != -1)
            m_lowPrioAdditionalContexts.removeAt(index);
        index = m_highPrioAdditionalContexts.indexOf(id);
        if (index != -1)
            m_highPrioAdditionalContexts.removeAt(index);
    }

    foreach (const Id id, add)
    {
        if (!id.isValid())
            continue;
        Context& cref = (priority == ICore::ContextPriority::High ? m_highPrioAdditionalContexts
                                                                  : m_lowPrioAdditionalContexts);
        if (!cref.contains(id))
            cref.prepend(id);
    }

    updateContext();
}

void MainWindow::updateContext()
{
    Context contexts = m_highPrioAdditionalContexts;

    foreach (IContext* context, m_activeContext)
        contexts.add(context->context());

    contexts.add(m_lowPrioAdditionalContexts);

    Context uniquecontexts;
    for (const Id& id : qAsConst(contexts))
    {
        if (!uniquecontexts.contains(id))
            uniquecontexts.add(id);
    }

    ActionManager::setContext(uniquecontexts);
    emit m_coreImpl->contextChanged(uniquecontexts);
}

void MainWindow::aboutQtCreator()
{
    if (!m_versionDialog)
    {
        m_versionDialog = new VersionDialog(this);
        connect(m_versionDialog, &QDialog::finished,
                this, &MainWindow::destroyVersionDialog);
        ICore::registerWindow(m_versionDialog, Context("Core.VersionDialog"));
        m_versionDialog->show();
    }
    else
    {
        ICore::raiseWindow(m_versionDialog);
    }
}

void MainWindow::destroyVersionDialog()
{
    if (m_versionDialog)
    {
        m_versionDialog->deleteLater();
        m_versionDialog = nullptr;
    }
}

void MainWindow::aboutPlugins()
{
    PluginDialog dialog(this);
    dialog.exec();
}

QPrinter* MainWindow::printer() const
{
    if (!m_printer)
        m_printer = new QPrinter(QPrinter::HighResolution);
    return m_printer;
}

void MainWindow::restoreWindowState()
{
    QSettings* settings = PluginManager::settings();
    settings->beginGroup(QLatin1String(settingsGroup));
    if (!restoreGeometry(settings->value(QLatin1String(windowGeometryKey)).toByteArray()))
        resize(1260, 700); // size without window decoration
    restoreState(settings->value(QLatin1String(windowStateKey)).toByteArray());
    settings->endGroup();
    show();
    StatusBarManager::restoreSettings();
}

} // namespace Internal
} // namespace Core
