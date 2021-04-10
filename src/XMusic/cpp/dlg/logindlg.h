#pragma once

#include "dialog.h"

class CLoginDlg : public CDialogEx
{
 public:
    CLoginDlg() = default;

    void show(cwstr strUser, const string& strPwd, E_LoginReult eRet);

private:
    void _setupUi();
};
