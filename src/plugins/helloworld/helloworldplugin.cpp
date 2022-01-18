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

#include "helloworldplugin.h"

#include <coreplugin/actionmanager/actioncontainer.h>
#include <coreplugin/actionmanager/actionmanager.h>
#include <coreplugin/coreconstants.h>
#include <coreplugin/icore.h>
#include <coreplugin/id.h>
#include <coreplugin/imode.h>
#include <coreplugin/modemanager.h>

#include <utils/styledbar.h>
#include <utils/theme/theme.h>

#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>

#include "helloform.h"

using namespace Core;
using namespace ExtensionSystem;
using namespace Utils;

namespace HelloWorld
{
namespace Internal
{

static QColor themeColor(Theme::Color role)
{
    return Utils::creatorTheme()->color(role);
}

/*!  A mode with a push button based on BaseMode.  */

class HelloMode : public Core::IMode
{
public:
    HelloMode()
    {
        QPalette palette = creatorTheme()->palette();
        palette.setColor(QPalette::Window, themeColor(Theme::Welcome_BackgroundColor));

        //m_modeWidget = new QWidget;
        m_modeWidget = new helloForm;
        m_modeWidget->setAutoFillBackground(true);
        m_modeWidget->setPalette(palette);
        /*
        auto divider = new QWidget(m_modeWidget);
        divider->setMaximumWidth(1);
        divider->setMinimumWidth(1);
        divider->setAutoFillBackground(true);
        divider->setPalette(themeColor(Theme::Welcome_DividerColor));

        auto pubtton = new QPushButton(tr("Hello World PushButton!"));
        //pubtton->setAutoFillBackground(true);

        auto hbox = new QHBoxLayout;
        hbox->addWidget(divider);
        hbox->addWidget(pubtton);
        //hbox->setStretchFactor(pubtton, 10);

        auto layout = new QVBoxLayout(m_modeWidget);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->setSpacing(0);
        layout->addWidget(new StyledBar(m_modeWidget));
        layout->addItem(hbox);
        //layout->addWidget(pubtton);
        */
        setWidget(m_modeWidget);
        setContext(Core::Context("HelloWorld.MainView"));
        setDisplayName(tr("Hello"));

        /*
        const Icon CLASSIC(":/welcome/images/mode_welcome.png");
        const Icon FLAT({{":/welcome/images/mode_welcome_mask.png",
                      Theme::IconsBaseColor}});
        const Icon FLAT_ACTIVE({{":/welcome/images/mode_welcome_mask.png",
                             Theme::IconsModeWelcomeActiveColor}});
        setIcon(Icon::modeIcon(CLASSIC, FLAT, FLAT_ACTIVE));*/
        setIcon(QIcon());
        setPriority(0);
        setId("HelloWorld.HelloWorldMode");
    }

private:
    QWidget* m_modeWidget;
};

/*! Constructs the Hello World plugin. Normally plugins don't do anything in
    their constructor except for initializing their member variables. The
    actual work is done later, in the initialize() and extensionsInitialized()
    functions.
*/
HelloWorldPlugin::HelloWorldPlugin()
{
}

/*! Plugins are responsible for deleting objects they created on the heap, and
    to unregister objects from the plugin manager that they registered there.
*/
HelloWorldPlugin::~HelloWorldPlugin()
{
    delete m_helloMode;
}

/*! Initializes the plugin. Returns true on success.
    Plugins want to register objects with the plugin manager here.

    \a errorMessage can be used to pass an error message to the plugin system,
       if there was any.
*/
bool HelloWorldPlugin::initialize(const QStringList& arguments, QString* errorMessage)
{
    Q_UNUSED(arguments)
    Q_UNUSED(errorMessage)

    // Create a unique context for our own view, that will be used for the
    // menu entry later.
    Core::Context context("HelloWorld.MainView");

    // Create an action to be triggered by a menu entry
    auto helloWorldAction = new QAction(tr("Say \"&Hello World!\""), this);
    connect(helloWorldAction, &QAction::triggered, this, &HelloWorldPlugin::sayHelloWorld);

    // Register the action with the action manager
    Core::Command* command =
        Core::ActionManager::registerAction(
            helloWorldAction, "HelloWorld.HelloWorldAction", context);

    // Create our own menu to place in the Tools menu
    Core::ActionContainer* helloWorldMenu =
        Core::ActionManager::createMenu("HelloWorld.HelloWorldMenu");
    QMenu* menu = helloWorldMenu->menu();
    menu->setTitle(tr("&Hello World"));
    menu->setEnabled(true);

    // Add the Hello World action command to the menu
    helloWorldMenu->addAction(command);

    // Request the Tools menu and add the Hello World menu to it
    Core::ActionContainer* toolsMenu =
        Core::ActionManager::actionContainer(Core::Constants::M_TOOLS);
    toolsMenu->addMenu(helloWorldMenu);

    // Add a mode with a push button based on BaseMode.
    m_helloMode = new HelloMode;

    return true;
}

/*! Notification that all extensions that this plugin depends on have been
    initialized. The dependencies are defined in the plugins .json(.in) file.

    Normally this function is used for things that rely on other plugins to have
    added objects to the plugin manager, that implement interfaces that we're
    interested in. These objects can now be requested through the
    PluginManager.

    The HelloWorldPlugin doesn't need things from other plugins, so it does
    nothing here.
*/
void HelloWorldPlugin::extensionsInitialized()
{
}

void HelloWorldPlugin::sayHelloWorld()
{
    // When passing nullptr for the parent, the message box becomes an
    // application-global modal dialog box
    QMessageBox::information(
        nullptr, tr("Hello World!"), tr("Hello World! Beautiful day today, isn't it?"));
}

} // namespace Internal
} // namespace HelloWorld
