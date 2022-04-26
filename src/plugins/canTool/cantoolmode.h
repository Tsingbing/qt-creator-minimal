#pragma once
#include <coreplugin/imode.h>

#include <QString>
#include <QIcon>

namespace CanTool {
namespace Internal {

class CanToolMode : public Core::IMode
{
public:
    CanToolMode(QObject *parent = nullptr);

private:
    QWidget* m_modeWidget = nullptr;
};
}
}
