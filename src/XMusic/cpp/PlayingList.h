
#pragma once

#include "widget/listview.h"

#define __ActiveTime 10

struct tagPlayingItem
{
    UINT uID = 0;
    CSinger *pSinger = NULL;
    QString qsTitle;
    UINT uDuration = 0;
    E_MediaQuality eQuality = E_MediaQuality::MQ_None;
};

class CPlayingList : public CListView
{
public:
    CPlayingList(QWidget *parent=NULL)
        : CListView(parent)
    {
    }

private:
    QPixmap m_pmPlaying;

    UINT m_uPageRowCount = 0;

    vector<tagPlayingItem> m_vecPlayingItems;

    UINT m_uPlayingItem = 0;

    int m_nActiveTime = 0;

    UINT m_uShadowWidth = 0;

public:
    inline tagPlayingItem* playingItem(UINT uIdx)
    {
        if (uIdx >= m_vecPlayingItems.size())
        {
            return NULL;
        }
        return &m_vecPlayingItems[uIdx];
    }

    void init();

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
    size_t getRowCount() const override;

    size_t getItemCount() const override
    {
        return m_vecPlayingItems.size();
    }

    void _onPaintItem(CPainter&, tagLVItem&) override;
    void _onPaintItem(CPainter&, tagLVItem&, const tagPlayingItem&);

    void _onItemDblClick(tagLVItem&, const QMouseEvent&) override;

    void _onBlankDblClick(const QMouseEvent&) override;

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
