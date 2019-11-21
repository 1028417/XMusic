#pragma once

#include "dialog.h"

class CMsgBox : public CDialog
{
    Q_OBJECT

public:
    CMsgBox(QWidget& parent);

    void show(const QString& qsMsg);
};
