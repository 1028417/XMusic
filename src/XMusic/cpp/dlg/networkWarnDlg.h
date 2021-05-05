#pragma once

#if __android
#include "dialog.h"

class CNetworkWarnDlg : public CDialogEx
{
public:
    CNetworkWarnDlg() = default;

public:
    void show(cfn_void cb);

private:
    void _setupUi();

    void _setPos() override;

    cqcr bkgColor() const override;

    void _onPaint(CPainter& painter, cqrc rc) override;

    bool _handleReturn() override {return true;}

    void _checkWifiConnected();
};
#endif
