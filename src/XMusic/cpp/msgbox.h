#pragma once

#include "dialog.h"

class CMsgBox : private CDialog
{
 public:
    CMsgBox(QWidget& parent);

public:
    void show(const QString& qsMsg, cfn_void cbClose);

private:
    cqcr bkgColor() const override
    {
        static QColor crBkg(200, 230, 255);
        return crBkg;
    }
};
