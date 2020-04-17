
#pragma once

#include "dialog.h"

class CWholeTrackDlg : public CDialog
{
    Q_OBJECT
public:
    CWholeTrackDlg(CDialog& parent, class CApp& app)
        : CDialog(parent)
        , m_app(app)
	{
	}

private:
    class CApp& m_app;
	
private:
    void _relayout(int cx, int cy) override;
};
