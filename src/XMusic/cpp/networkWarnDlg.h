#pragma once

#include "dialog.h"

class CNetworkWarnDlg : private CDialog
{
 public:
    CNetworkWarnDlg(QWidget& parent, class CApp& app);

private:
    class CApp& m_app;

public:
    void show(cfn_void cb);

private:
    cqcr bkgColor() const override
    {
        static QColor crBkg(200, 230, 255);
        return crBkg;
    }
};
