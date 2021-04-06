#pragma once

#include "dialog.h"

class CLoginDlg : public CDialog
{
 public:
    CLoginDlg();

    void show(E_LoginReult eRet);
};
