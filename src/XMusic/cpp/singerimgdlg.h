
#pragma once

#include "dialog.h"

class CSingerImgDlg : public CDialog
{
public:
    CSingerImgDlg(class CMedialibDlg& medialibDlg, class CApp& app);

private:
    class CMedialibDlg& m_medialibDlg;
    class CApp& m_app;

private:
    void _relayout(int cx, int cy) override;

    void _onClosed() override
    {
    }
};
