
#include "xmusic.h"

#include "logindlg.h"
#include "ui_logindlg.h"

static Ui::LoginDlg ui;

void CLoginDlg::_setupUi()
{
    if (m_bInit)
    {
        return;
    }

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

void CLoginDlg::show(cwstr strUser, const string& strPwd, E_LoginReult eRet)
{
    _setupUi();

    ui.editUser->setText(__WS2Q(strUser));
    ui.editPwd->setText(__WS2Q(strutil::fromAsc(strPwd)));

    QString qsTitle;
    switch (eRet)
    {
    case E_LoginReult::LR_NetworkError:
        qsTitle = "登录失败：网络异常";
        break;
    case E_LoginReult::LR_ResponseError:
        qsTitle = "登录失败：服务器异常";
        break;
    case E_LoginReult::LR_UserNotExist:
        qsTitle = "登录失败：账号不存在";
        break;
    case E_LoginReult::LR_ProfileInvalid:
        qsTitle = "登录失败：账号异常";
        break;
    case E_LoginReult::LR_PwdWrong:
        qsTitle = "登录失败：密码错误";
        break;
    case E_LoginReult::LR_MultiLogin:
        qsTitle = "账号已在其他设备登录";
        break;
    default:
        break;
    }
    ui.labelTitle->setText(qsTitle);

    CDialogEx::show();
}
