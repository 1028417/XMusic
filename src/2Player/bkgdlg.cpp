#include "bkgdlg.h"
#include "ui_bkgdlg.h"

static Ui::BkgDlg ui;

CBkgDlg::CBkgDlg(QWidget *parent) :
    CDialog(parent)
{
    ui.setupUi(this);
}
