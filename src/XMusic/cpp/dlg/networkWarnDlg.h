#pragma once

#if __android
#include "dialog.h"

class CNetworkWarnDlg : public CDialogEx
{
public:
    static CNetworkWarnDlg& inst() //_checkUnMobileConnected要求持久化的对象
    {
        static CNetworkWarnDlg inst;
        return inst;
    }

private:
    CNetworkWarnDlg() = default;

public:
    void show(cfn_void cb);

private:
    void _setupUi();

    cqcr bkgColor() const override;

    void _onPaint(CPainter& painter, cqrc rc) override;

    bool _handleReturn() override {return true;}

    void _checkWifiConnected();
};
#endif
