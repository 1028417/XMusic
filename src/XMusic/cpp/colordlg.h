
#pragma once

#include "dialog.h"

#include "colorbar.h"

class CColorDlg : public CDialog
{
    Q_OBJECT
public:
    CColorDlg(class CApp& app, class CBkgDlg& bkgDlg);

private:
    class CApp& m_app;
    class CBkgDlg& m_bkgDlg;
	
private:
    void _relayout(int cx, int cy) override;

    void modifyColor(CColorBar *pBar, int8_t offset);

private slots:
    void slot_buttonClicked(class CButton *pButton);

    void slot_barValueChanged(CColorBar*, uint8_t uValue);

public:
    void init();

    void show();
};
