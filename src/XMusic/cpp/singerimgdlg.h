
#pragma once

#include "dialog.h"

class CSingerImgDlg : public CDialog
{
public:
    CSingerImgDlg(class CMedialibDlg& medialibDlg, class CApp& app);

private:
    class CMedialibDlg& m_medialibDlg;
    class CApp& m_app;

    wstring m_strSingerName;
    UINT m_uSingerImgIdx = 0;
    QPixmap m_pm;

private:
    void _onPaint(CPainter& painter, cqrc rc) override;

public:
    void show(cwstr strSingerName);
};
