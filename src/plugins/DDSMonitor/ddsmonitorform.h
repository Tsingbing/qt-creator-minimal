#pragma once

#include <QWidget>

namespace Ui {
class DDSMonitorForm;
}

class DDSMonitorForm : public QWidget
{
    Q_OBJECT

public:
    explicit DDSMonitorForm(QWidget *parent = nullptr);
    ~DDSMonitorForm();

private:
    Ui::DDSMonitorForm *ui;
};

