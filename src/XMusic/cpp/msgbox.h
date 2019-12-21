#pragma once

#include "dialog.h"

class CMsgBox : private CDialog
{
    Q_OBJECT
 public:
    CMsgBox();

public:
    static void show(QWidget& parent, const QString& qsMsg, const fn_void& cbClose);

private:
    const QColor& bkgColor() const override
    {
        static QColor crBkg(200, 230, 255);
        return crBkg;
    }
};
