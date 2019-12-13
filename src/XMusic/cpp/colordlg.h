
#pragma once

#include "dialog.h"

#include "button.h"

#include "colorbar.h"

class CColorDlg : public CDialog
{
    Q_OBJECT
public:
    CColorDlg(class CXMusicApp& app, class CBkgDlg& bkgDlg);

private:    
    class CXMusicApp& m_app;
	
private:
    void _relayout(int cx, int cy) override;

    void modifyColor(CColorBar *pBar, int8_t offset);

private slots:
    void slot_buttonClicked(CButton *pButton);

    void slot_barValueChanged(CColorBar*, uint8_t uValue);

public:
    void show();
};
