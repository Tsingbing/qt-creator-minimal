#pragma once

#include <QWidget>

namespace Ui {
class WireForm;
}

class WireForm : public QWidget
{
    Q_OBJECT

public:
    explicit WireForm(QWidget *parent = nullptr);
    ~WireForm();

private:
    Ui::WireForm *ui;
};

