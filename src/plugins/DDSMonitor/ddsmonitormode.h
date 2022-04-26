#pragma once
#include <coreplugin/imode.h>

#include <QString>
#include <QIcon>

namespace DDSMonitor {
namespace Internal {

class DDSMonitorMode : public Core::IMode
{
public:
    DDSMonitorMode(QObject *parent = nullptr);

private:
    QWidget* m_modeWidget = nullptr;
};
}
}
