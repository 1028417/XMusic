#pragma once

#include "dialog.h"

class CMsgBox : private CDialog
{
 public:
    CMsgBox(QWidget& parent);

public:
    void show(QWidget& parent, cqstr qsMsg, cfn_void cbClose);    
    void show(cqstr qsMsg, cfn_void cbClose)
    {
        show(m_parent, qsMsg, cbClose);
    }

private:
    cqcr bkgColor() const override;
};
