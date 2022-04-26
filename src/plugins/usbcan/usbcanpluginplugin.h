#ifndef USBCAN_H
#define USBCAN_H

#include "usbcanplugin_global.h"

#include <extensionsystem/iplugin.h>

namespace UsbCanPlugin {
namespace Internal {

class UsbCanPluginPlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "UsbCanPlugin.json")

public:
    UsbCanPluginPlugin();
    ~UsbCanPluginPlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace UsbCanPlugin

#endif // USBCAN_H
