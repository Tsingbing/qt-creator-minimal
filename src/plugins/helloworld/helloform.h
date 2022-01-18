#ifndef HELLOFORM_H
#define HELLOFORM_H

#include <QWidget>

namespace Ui {
class helloForm;
}

class helloForm : public QWidget
{
    Q_OBJECT

public:
    explicit helloForm(QWidget *parent = nullptr);
    ~helloForm();

private:
    Ui::helloForm *ui;
};

#endif // HELLOFORM_H
