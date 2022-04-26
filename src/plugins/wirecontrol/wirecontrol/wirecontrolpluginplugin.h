#ifndef WIRECONTROL_H
#define WIRECONTROL_H

#include "wirecontrolplugin_global.h"

#include <extensionsystem/iplugin.h>

namespace WireControlPlugin {
namespace Internal {

class WireControlPluginPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "WireControlPlugin.json")

public:
    WireControlPluginPlugin();
    ~WireControlPluginPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace WireControlPlugin

#endif // WIRECONTROL_H
