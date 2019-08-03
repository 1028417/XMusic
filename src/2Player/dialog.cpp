#include "dialog.h"
#include "ui_dialog.h"

CDialog::CDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowState(Qt::WindowFullScreen);
}

CDialog::~CDialog()
{
    delete ui;
}
