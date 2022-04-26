#include "ddsmonitorform.h"
#include "ui_ddsmonitorform.h"

DDSMonitorForm::DDSMonitorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DDSMonitorForm)
{
    ui->setupUi(this);
}

DDSMonitorForm::~DDSMonitorForm()
{
    delete ui;
}
