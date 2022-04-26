#pragma once

#include "ddsmonitor_global.h"

#include <extensionsystem/iplugin.h>

namespace DDSMonitor {
namespace Internal {

class DDSMonitorPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "DDSMonitor.json")

public:
    DDSMonitorPlugin();
    ~DDSMonitorPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace DDSMonitor

