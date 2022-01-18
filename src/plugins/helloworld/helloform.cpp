#include "helloform.h"
#include "ui_helloform.h"

helloForm::helloForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::helloForm)
{
    ui->setupUi(this);
}

helloForm::~helloForm()
{
    delete ui;
}
