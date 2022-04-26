#include "wireform.h"
#include "ui_wireform.h"

WireForm::WireForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WireForm)
{
    ui->setupUi(this);
}

WireForm::~WireForm()
{
    delete ui;
}
