#include "bkgdlg.h"
#include "ui_bkgdlg.h"

static Ui::BkgDlg ui;

CBkgDlg::CBkgDlg(QWidget *parent) :
    QDialog(parent)
{
    ui.setupUi(this);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowState(Qt::WindowFullScreen);
}

bool CBkgDlg::event(QEvent *ev)
{
    switch (ev->type())
    {
    case QEvent::Move:
    case QEvent::Resize:
    case QEvent::Show:
        _relayout();

        break;
    default:
        break;
    }

    return QDialog::event(ev);
}

void CBkgDlg::_relayout()
{
}
