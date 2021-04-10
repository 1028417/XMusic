
#include "xmusic.h"

#include "logindlg.h"
#include "ui_logindlg.h"

static Ui::LoginDlg ui;

void CLoginDlg::show(cwstr strUser, const string& strPwd, E_LoginReult eRet)
{
    static bool s_bFlag = false;
    if (!s_bFlag)
    {
        s_bFlag = true;

        ui.setupUi(this);

        this->connect_dlgClose(ui.btnX);

        ui.btnLogin->onClicked([&]{
            cauto strUser = ui.editUser->text().trimmed().toStdWString();
            if (strUser.empty())
            {
                return;
            }

            cauto strPwd = ui.editPwd->text().trimmed().toStdString();

            close();

            (void)__app.asyncLogin(strUser, strPwd);
        });
    }


    ui.editUser->setText(__WS2Q(strUser));
    ui.editPwd->setText(__WS2Q(strutil::fromAsc(strPwd)));

    QString qsTitle;
    switch (eRet)
    {
    case E_LoginReult::LR_NetworkError:
        qsTitle = "网络异常，请重试";
        break;
    case E_LoginReult::LR_UserInvalid:
        qsTitle = "账号不存在，请重新输入";
        break;
    case E_LoginReult::LR_ProfileInvalid:
        qsTitle = "账号异常，请使用其他账号";
        break;
    case E_LoginReult::LR_PwdWrong:
        qsTitle = "密码错误，请重新输入";
        break;
    case E_LoginReult::LR_MultiLogin:
        qsTitle = "账号已在其他设备登录";
        break;
    default:
        break;
    }
    ui.labelTitle->setText(qsTitle);

    CDialog::show();
}
