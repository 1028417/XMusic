
#pragma once

#include "dialog.h"

class CWholeTrackDlg : public CDialog
{
    Q_OBJECT
public:
    CWholeTrackDlg(class CMedialibDlg& medialibDlg, class CApp& app);

private:
    class CMedialibDlg& m_medialibDlg;

    class CApp& m_app;

    class CMediaRes *m_pMediaRes;

public:
    void relayout(cqrc rcBtnReturn, cqrc rcBtnPlay);

    bool tryShow(class CMediaRes& mediaRes);
};
