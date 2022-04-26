#ifndef WIRE_H
#define WIRE_H

#include "wire_global.h"

#include <extensionsystem/iplugin.h>

namespace Wire {
namespace Internal {

class WirePlugin : public ExtensionSystem::IPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QtCreatorPlugin" FILE "Wire.json")

public:
    WirePlugin();
    ~WirePlugin() override;

    bool initialize(const QStringList &arguments, QString *errorString) override;
    void extensionsInitialized() override;
    ShutdownFlag aboutToShutdown() override;

private:
    void triggerAction();
};

} // namespace Internal
} // namespace Wire

#endif // WIRE_H
