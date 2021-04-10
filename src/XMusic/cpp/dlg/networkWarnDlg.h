#pragma once

#if __android
#include "dialog.h"

class CNetworkWarnDlg : private CDialog
{
public:
    static CNetworkWarnDlg& inst()
    {
        static CNetworkWarnDlg inst;
        return inst;
    }

private:
    CNetworkWarnDlg() : CDialog(false)
    {
    }

public:
    void show(cfn_void cb);

private:
    cqcr bkgColor() const override;

    bool _handleReturn() override {return true;}
};
#endif
