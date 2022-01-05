#pragma once

#include "firecat_toolbar_global.h"
#include <extensionsystem/iplugin.h>

namespace firecat_Toolbar
{
namespace Internal
{

class firecat_ToolbarPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "firecat_toolbar.json")

public:
    firecat_ToolbarPlugin();
    ~firecat_ToolbarPlugin();

    bool initialize(const QStringList &arguments, QString *errorString);
    void extensionsInitialized();
    ShutdownFlag aboutToShutdown();

private:
    void triggerAction();
};

} // namespace Internal
} // namespace firecat_Toolbar
