
#pragma once

#include "dialog.h"

class CSingerImgDlg : public CDialog
{
public:
    CSingerImgDlg(class CMedialibDlg& medialibDlg, class CApp& app);

private:
    class CApp& m_app;

    wstring m_strSingerName;
    UINT m_uSingerImgIdx = 0;

    UINT m_cxImg = 0;
    UINT m_cyImg = 0;
    QBrush m_brush;

private:
    void _onPaint(CPainter& painter, cqrc rc) override;

    void _onTouchEvent(E_TouchEventType eType, const CTouchEvent& te) override;

    void _switchImg(int nOffset);

public:
    void show(cwstr strSingerName);

    void relayout(cqrc rcBtnReturn);

    void updateSingerImg();
};
