#pragma once

#include "dialog.h"

class CNetworkWarnDlg : private CDialog
{
 public:
    CNetworkWarnDlg(QWidget& parent);

public:
    void show(cfn_void cb);

private:
    cqcr bkgColor() const override;

    bool _handleReturn() override {return true;}
};
