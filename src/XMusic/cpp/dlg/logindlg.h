#pragma once

#include "dialog.h"

class CLoginDlg : public CDialog
{
 public:
    CLoginDlg(QWidget& parent);

    void show(E_LoginReult eRet);
};
