#pragma once

#include "dialog.h"

class CMsgBox : private CDialog
{
 public:
    CMsgBox(QWidget& parent);

public:
    void show(cqstr qsMsg, cfn_void cbClose);    
    void show(QWidget& parent, cqstr qsMsg, cfn_void cbClose);

private:
    cqcr bkgColor() const override;
};
