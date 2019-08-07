#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(QWidget *parent) :
    CDialog(parent)
{
    ui.setupUi(this);
}
