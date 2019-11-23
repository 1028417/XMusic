#pragma once

#include "dialog.h"

class CMsgBox : public CDialog
{
    Q_OBJECT

private:
    CMsgBox(QWidget& parent);

private:
    void _show(QWidget& parent, const QString& qsMsg, const fn_void& cbClose);

public:
    static void show(QWidget& parent, const QString& qsMsg, const fn_void& cbClose = NULL)
    {
        static CMsgBox inst(parent);
        inst._show(parent, qsMsg, cbClose);
    }
};
