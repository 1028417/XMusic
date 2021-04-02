
#include "xmusic.h"

#include "logindlg.h"
#include "ui_logindlg.h"

static Ui::LoginDlg ui;

CLoginDlg::CLoginDlg(QWidget& parent) : CDialog(parent, false)
{
    ui.setupUi(this);

    connect(ui.btnCancel, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnLogin, &CButton::signal_clicked, [&]{
        close();
        cauto strUser = ui.editUser->text().trimmed().toStdWString();
        __app.asyncLogin(strUser);
    });
}
