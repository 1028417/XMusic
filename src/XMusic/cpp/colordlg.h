
#pragma once

#include "dialog.h"

class CColorDlg : public CDialog
{
    Q_OBJECT
public:
    CColorDlg(class CBkgDlg& bkgDlg);

private:
    class CBkgDlg& m_bkgDlg;
	
private:
    void _relayout(int cx, int cy) override;

    void applyBkgColor();

private slots:
    void slot_buttonClicked(class CButton *pButton);

    void slot_barValueChanged(QWidget*, uint8_t uValue);

public:
    void init();

    void show();
};
