#include "dialog.h"
#include "ui_dialog.h"

CDialog::CDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

CDialog::~CDialog()
{
    delete ui;
}
