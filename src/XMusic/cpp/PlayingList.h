
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
    CPlayingList(class CApp& app);

private:
    class CApp& m_app;

    UINT m_uPageRowCount = 0;

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

    void setPageRowCount(UINT uPageRowCount)
    {
        m_uPageRowCount = uPageRowCount;
    }

private:
    size_t getPageRowCount() override;

    size_t getRowCount() override
    {
        return m_alPlayingItems.size();
    }

    void _onPaintRow(CPainter&, tagLVRow&) override;

    void _onRowDblClick(tagLVRow&, const QMouseEvent&) override;

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
