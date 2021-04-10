#pragma once

#if __android
#include "dialog.h"

class CNetworkWarnDlg : private CDialogEx
{
public:
    static CNetworkWarnDlg& inst()
    {
        static CNetworkWarnDlg inst;
        return inst;
    }

private:
    CNetworkWarnDlg() = default;o

public:
    void show(cfn_void cb);

private:
    cqcr bkgColor() const override;

    void _onPaint(CPainter& painter, cqrc rc) override;

    bool _handleReturn() override {return true;}
};
#endif
