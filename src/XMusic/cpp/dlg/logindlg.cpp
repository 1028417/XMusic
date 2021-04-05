
#include "xmusic.h"

#include "logindlg.h"
#include "ui_logindlg.h"

static Ui::LoginDlg ui;

CLoginDlg::CLoginDlg(QWidget& parent) : CDialog(parent, false)
{
    ui.setupUi(this);

    connect(ui.btnX, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnLogin, &CButton::signal_clicked, [&]{
        cauto strUser = ui.editUser->text().trimmed().toStdWString();
        if (strUser.empty())
        {
            return;
        }

        cauto strPwd = ui.editPwd->text().trimmed().toStdString();

        close();

        __app.login(strUser, strPwd);
    });
}
