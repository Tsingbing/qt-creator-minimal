#pragma once

#include "cantool_global.h"

#include <extensionsystem/iplugin.h>


namespace CanTool {
namespace Internal {

class CanToolMode;

class CanToolPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "CanTool.json")

public:
    CanToolPlugin();
    ~CanToolPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();

    CanToolMode *m_cantoolMode = nullptr;
};

} // namespace Internal
} // namespace CanTool

