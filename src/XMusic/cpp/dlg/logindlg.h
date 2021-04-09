#pragma once

#include "dialog.h"

class CLoginDlg : public CDialog
{
 public:
    CLoginDlg() : CDialog(false)
    {
    }

    void show(E_LoginReult eRet);
};
