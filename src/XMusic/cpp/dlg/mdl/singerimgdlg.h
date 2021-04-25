
#pragma once

#include "dlg/dialog.h"

#include "label.h"

class CSingerImgDlg : public CDialog
{
public:
    CSingerImgDlg();

private:
    CSingerImgMgr& m_singerImgMgr;

    UINT m_uSingerID = 0;

    UINT m_uImgCount = 0;
    UINT m_uImgIdx = 0;
    int m_nSwitchingOffset = 0;

    QPixmap m_pmImg;

    CLoadingLabel m_loadingLabel;

public:
    void init();

    void show(UINT uSingerID);

    void relayoutTitle(cqrc rcBtnReturn);

    void updateSingerImg();

private:
    void _relayout(int cx, int cy) override;

    void _onPaint(CPainter&, cqrc) override;

    void _onTouchEvent(E_TouchEventType eType, const CTouchEvent& te) override;

    void _showImg(int nOffset);
};
