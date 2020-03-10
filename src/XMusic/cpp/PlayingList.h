
#pragma once

#include "listview.h"

#define __ActiveTime 10

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

    UINT m_uShadowWidth = 0;

public:
    void setShadow(UINT uWidth=1)
    {
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
    void _onPaintRow(CPainter& painter, QRect& rc, const tagPlayingItem& playingItem, bool bPlayingItem, UINT uIdx);
;
    void _onRowDblClick(tagLVRow&, const QMouseEvent&) override;

#if __windows || __mac
    void _onMouseEnter() override
    {
        _updateActive(); //_updateActive(-1);
    }

    /*void _onMouseLeave() override
    {
        if (0 != m_nActiveTime)
        {
            _updateActive(0);
        }
    }*/

    void _onMouseEvent(E_MouseEventType eType, const QMouseEvent& me) override
    {
        if (0 == m_nActiveTime)
        {
            update();
        }

        m_nActiveTime = __ActiveTime;

        CListView::_onMouseEvent(eType, me);
    }
#endif

    void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

    /*bool _onGesture(QGesture&) override
    {
        _updateActive(-1);
        return true;
    }*/

    void _updateActive(int nActiveTime=__ActiveTime);

    void _onAutoScrollBegin() override;
    void _onAutoScrollEnd() override;

    void timerEvent(QTimerEvent *) override;
};
