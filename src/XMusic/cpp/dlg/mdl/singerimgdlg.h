
#pragma once

#include "dlg/dialog.h"

class CSingerImgDlg : public CDialog
{
public:
    CSingerImgDlg(class CMedialibDlg& medialibDlg);

private:
    CSingerImgMgr& m_singerImgMgr;

    wstring m_strSingerName;

    UINT m_uImgCount = 0;
    UINT m_uImgIdx = 0;
    int m_nSwitchingOffset = 0;

    UINT m_cxImg = 0;
    UINT m_cyImg = 0;
    QBrush m_brush;

public:
    void init();

    void show(cwstr strSingerName);

    void relayout(cqrc rcBtnReturn);

    void updateSingerImg();

private:
    void _relayout(int cx, int cy) override;

    void _onPaint(CPainter& painter, cqrc rc) override;

    void _onTouchEvent(E_TouchEventType eType, const CTouchEvent& te) override;

    void _showImg(int nOffset);
};
