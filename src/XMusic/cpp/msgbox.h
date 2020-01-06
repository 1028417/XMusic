#pragma once

#include "dialog.h"

class CMsgBox : private CDialog
{
    Q_OBJECT
 public:
    CMsgBox(QWidget *parent = NULL);

public:
    void show(QWidget& parent, const QString& qsMsg, cfn_void cbClose);
    void show(const QString& qsMsg, cfn_void cbClose);

private:
    cqcr bkgColor() const override
    {
        static QColor crBkg(200, 230, 255);
        return crBkg;
    }
};
