#include "cantoolmode.h"

#include "mainform.h"

#include <utils/icon.h>
using namespace Utils;

using namespace DDSMonitor;
using namespace DDSMonitor::Internal;

DDSMonitorMode::DDSMonitorMode(QObject *parent)
    : Core::IMode(parent)
{
    m_modeWidget = new MainForm;
    m_modeWidget->setAutoFillBackground(true);

    setWidget(m_modeWidget);
    setDisplayName(QString::fromLocal8Bit("Can工具"));

    const Icon CLASSIC(":/cantool/images/mode_wire.png");
    const Icon FLAT({{":/cantool/images/mode_wire_mask.png",Theme::IconsBaseColor}});
    const Icon FLAT_ACTIVE({{":/cantool/images/mode_wire_mask.png",Theme::IconsModeWelcomeActiveColor}});
    setIcon(Icon::modeIcon(CLASSIC, FLAT, FLAT_ACTIVE));
}
