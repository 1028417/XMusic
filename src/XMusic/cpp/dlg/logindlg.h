#pragma once

#include "dialog.h"

class CLoginDlg : public CDialog
{
 public:
    CLoginDlg() : CDialog(false)
    {
    }

    void show(cwstr strUser, const string& strPwd, E_LoginReult eRet);
};
