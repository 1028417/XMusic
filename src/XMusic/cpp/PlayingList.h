
#pragma once

#include "listview.h"

struct tagPlayingItem
{
    UINT uID = 0;
    QString qsTitle;
};

class CPlayingList : public CListView
{
public:
    CPlayingList(class CXMusicApp& app);

private:
    class CXMusicApp& m_app;

    ArrList<tagPlayingItem> m_alPlayingItems;

    UINT m_uPlayingItem = 0;

    int m_nActiveTime = 0;
    float m_fInactiveAlpha = 1;

    UINT m_uShadowAlpha = 255;
    UINT m_uShadowWidth = 0;

public:
    void setInactiveAlpha(float fAlpha)
    {
        m_fInactiveAlpha = fAlpha;
    }

    void setShadow(UINT uAlpha, UINT uWidth = 1)
    {
        m_uShadowAlpha = uAlpha;
        m_uShadowWidth = uWidth;

        update();
    }

    void updateList(UINT uPlayingItem);

    void updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem);

private:
    size_t getRowCount() override
    {
        return m_alPlayingItems.size();
    }

    size_t getPageRowCount() override;

    void _onPaintRow(CPainter&, const tagLVRow&) override;

    void _onRowDblClick(const tagLVRow&, const QMouseEvent&) override;

    void _onMouseEnter() override;
    //void _onMouseLeave() override;

    void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

    /*bool _onGesture(QGesture&) override
    {
        _updateActive(-1);
        return true;
    }*/

    void _updateActive(int nActiveTime=10);

    void _onAutoScrollBegin() override;
    void _onAutoScrollEnd() override;

    void timerEvent(QTimerEvent *) override;
};
