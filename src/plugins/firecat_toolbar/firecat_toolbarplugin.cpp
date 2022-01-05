#include "firecat_toolbarplugin.h"
#include "firecat_toolbarconstants.h"

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/actionmanager/command.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icontext.h>
#include <coreplugin/icore.h>

#include <QAction>
#include <QMessageBox>

#include <QProcess>
#include <coreplugin/modemanager.h>
#include <utils/icon.h>
#include <QDesktopServices>
#if defined(Q_OS_WIN32)
#include <windows.h>
#endif

namespace firecat_Toolbar
{
namespace Internal
{

firecat_ToolbarPlugin::firecat_ToolbarPlugin()
{
    // Create your members
}

firecat_ToolbarPlugin::~firecat_ToolbarPlugin()
{
    // Unregister objects from the plugin manager's object pool
    // Delete members
}

bool firecat_ToolbarPlugin::initialize(const QStringList &arguments, QString *errorString)
{
    // Register objects in the plugin manager's object pool
    // Load settings
    // Add actions to menus
    // Connect to other plugins' signals
    // In the initialize function, a plugin can be sure that the plugins it
    // depends on have initialized their members.

    Q_UNUSED(arguments)
    Q_UNUSED(errorString)

#if 0
    //我们把RunWithoutDeploy这个方法提携到工具栏,方便使用
    const char id1[] = "ProjectExplorer.RunWithoutDeploy";//这个id对应的是Qt Creator源码的const char RUNWITHOUTDEPLOY[]
    QAction *act1 = Core::ActionManager::command(id1)->action();//对应Qt Creator源码的m_runWithoutDeployAction
    if (act1 == NULL)
    {
        return false;
    }
    const Utils::Icon CLASSIC1(":/image/mode_run.png");//32位图片,34*34像素
    const Utils::Icon FLAT1({{":/image/mode_run_mask.png", Utils::Theme::IconsRunToolBarColor}});//8位图片,34*34像素
    const Utils::Icon FLAT_ACTIVE1({{":/image/mode_run_mask.png", Utils::Theme::IconsModeWelcomeActiveColor}});
    act1->setIcon(Utils::Icon::modeIcon(CLASSIC1, FLAT1, FLAT_ACTIVE1));
    //act1->setIcon(Utils::Icon::sideBarIcon(CLASSIC1, FLAT1));
    act1->setVisible(true);
    Core::ModeManager::addAction(act1, 130);

    //我们把DebugWithoutDeploy这个方法提携到工具栏,方便使用
    const char id2[] = "Debugger.DebugWithoutDeploy";
    QAction *act2 = Core::ActionManager::command(id2)->action();//对应Qt Creator源码的m_debugWithoutDeployAction
    if (act2 == NULL)
    {
        return false;
    }
    const Utils::Icon CLASSIC2(":/image/mode_debug.png");
    const Utils::Icon FLAT2({{":/image/mode_debug_mask.png", Utils::Theme::IconsRunToolBarColor}});
    const Utils::Icon FLAT_ACTIVE2({{":/image/mode_debug_mask.png", Utils::Theme::IconsModeWelcomeActiveColor}});
    act2->setIcon(Utils::Icon::modeIcon(CLASSIC2, FLAT2, FLAT_ACTIVE2));
    //act2->setIcon(Utils::Icon::sideBarIcon(CLASSIC2, FLAT2));
    act2->setVisible(true);
    Core::ModeManager::addAction(act2, 120);
#endif

    QAction *act1 = new QAction(tr("run"), this);
    const Utils::Icon CLASSIC1(":/image/mode_run.png");                                           //32位图片,34*34像素
    const Utils::Icon FLAT1({{":/image/mode_run_mask.png", Utils::Theme::IconsRunToolBarColor}}); //8位图片,34*34像素
    const Utils::Icon FLAT_ACTIVE1({{":/image/mode_run_mask.png", Utils::Theme::IconsModeWelcomeActiveColor}});
    act1->setIcon(Utils::Icon::modeIcon(CLASSIC1, FLAT1, FLAT_ACTIVE1));
    //act1->setIcon(Utils::Icon::sideBarIcon(CLASSIC1, FLAT1));
    act1->setVisible(true);
    connect(act1, &QAction::triggered, this, [=]() {
        QMessageBox::information(nullptr, QLatin1String("info"), "hello,run!");
    });
    Core::ModeManager::addAction(act1, 130);

    QAction *act2 = new QAction(tr("debug"), this);
    const Utils::Icon CLASSIC2(":/image/mode_debug.png");
    const Utils::Icon FLAT2({{":/image/mode_debug_mask.png", Utils::Theme::IconsRunToolBarColor}});
    const Utils::Icon FLAT_ACTIVE2({{":/image/mode_debug_mask.png", Utils::Theme::IconsModeWelcomeActiveColor}});
    act2->setIcon(Utils::Icon::modeIcon(CLASSIC2, FLAT2, FLAT_ACTIVE2));
    //act2->setIcon(Utils::Icon::sideBarIcon(CLASSIC2, FLAT2));
    act2->setVisible(true);
    connect(act2, &QAction::triggered, this, [=]() {
        QMessageBox::information(nullptr, QLatin1String("info"), "hello,debug!");
    });
    Core::ModeManager::addAction(act2, 120);
/**/
    //我们把微软计算器提携到工具栏,方便使用
#if defined(Q_OS_WIN32)
    QAction *act3 = new QAction(tr("FtpArm"), this);
    const Utils::Icon CLASSIC3(":/image/mode_run.png");
    const Utils::Icon FLAT3({{":/image/mode_run_mask.png", Utils::Theme::IconsRunToolBarColor}});
    const Utils::Icon FLAT_ACTIVE3({{":/image/mode_run_mask.png", Utils::Theme::IconsModeWelcomeActiveColor}});
    act3->setIcon(Utils::Icon::modeIcon(CLASSIC3, FLAT3, FLAT_ACTIVE3));
    //act3->setIcon(Utils::Icon::sideBarIcon(CLASSIC3, FLAT3));
    act3->setVisible(true);
    //QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);//Qt自身没有提供System32的路径
    wchar_t szPath[MAX_PATH] = {0};
    GetSystemDirectory(szPath, MAX_PATH);
    QString path = QString::fromWCharArray(szPath);
    connect(act3, &QAction::triggered, this, [=]() {
        QProcess *poc = new QProcess;
        //poc->start(path + "\\calc.exe"); //即"C:\\Windows\\system32\\calc.exe"
        poc->start("EM-Configer.exe");
        //QDesktopServices::openUrl(QUrl::fromLocalFile("D:\\HMIDeveloper\\EM-Configer.exe"));

    });
    Core::ModeManager::addAction(act3, 150);
#endif

    // 因为Qt Creator源码有定义位置摆放的优先级
    // Action priorities
    //const int  P_ACTION_RUN            = 100;
    //const int  P_ACTION_BUILDPROJECT   = 80;
    //const int  P_ACTION_DEBUG          = 90; // Priority for the modemanager.
    //ModeManager::addAction(cmd->action(), Constants::P_ACTION_RUN);

    return true;
}

void firecat_ToolbarPlugin::extensionsInitialized()
{
    // Retrieve objects from the plugin manager's object pool
    // In the extensionsInitialized function, a plugin can be sure that all
    // plugins that depend on it are completely initialized.
}

ExtensionSystem::IPlugin::ShutdownFlag firecat_ToolbarPlugin::aboutToShutdown()
{
    // Save settings
    // Disconnect from signals that are not needed during shutdown
    // Hide UI (if you add UI that is not in the main window directly)
    return SynchronousShutdown;
}

void firecat_ToolbarPlugin::triggerAction()
{
    /*
    QMessageBox::information(Core::ICore::mainWindow(),
                             tr("Action Triggered"),
                             tr("This is an action from firecat_Toolbar."));
                             */
}

} // namespace Internal
} // namespace firecat_Toolbar
