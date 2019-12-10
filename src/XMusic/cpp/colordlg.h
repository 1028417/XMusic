
#pragma once

#include "dialog.h"

class CColorDlg : public CDialog
{
    Q_OBJECT
public:
    CColorDlg(class CXMusicApp& app, class CBkgDlg& bkgDlg);

private:    
    class CXMusicApp& m_app;
	
private:
    void _relayout(int cx, int cy) override;

public:
    void show();
};
