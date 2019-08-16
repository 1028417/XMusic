
#pragma once

#include "view.h"

#include "listview.h"

struct tagPlayingItem
{
    UINT uID = 0;
    QString qsTitle;
};

class CPlayingList : public CListView
{
public:
    CPlayingList(class CPlayerView& view) :
        m_view(view)
        , m_crShadow(128,128,128)
    {
        this->startTimer(1000);
    }

private:
    CPlayerView& m_view;

    ArrList<tagPlayingItem> m_alPlayingItems;

    UINT m_uPlayingItem = 0;

    int m_nActiveTime = 0;
    float m_fInactiveAlpha = 1;

    UINT m_uShadowWidth = 1;
    QColor m_crShadow;

public:
    void setInactiveAlpha(float fAlpha)
    {
        m_fInactiveAlpha = fAlpha;
    }

    void setShadow(UINT uWidth, UINT r, UINT g, UINT b)
    {
        m_uShadowWidth = uWidth;
        m_crShadow.setRgb(r,g,b);

        CWidget::update();
    }

    void setShadow(UINT uWidth)
    {
        m_uShadowWidth = uWidth;

        CWidget::update();
    }

    void updateList(UINT uPlayingItem);

    void updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem);

private:
    UINT getRowCount() override
    {
        return m_alPlayingItems.size();
    }

    void _onPaintRow(CPainter&, QRect&, const tagListViewRow&) override;

    void _handleRowDblClick(const tagListViewRow&, const QMouseEvent&) override;

    void _onMouseEnter() override;
    void _onMouseLeave() override;

    void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

    /*bool _onGesture(QGesture&) override
    {
        _updateActive(-1);
        return true;
    }*/

    void _updateActive(int nActiveTime=6);

    void _onAutoScrollBegin() override;
    void _onAutoScrollEnd() override;

    void timerEvent(QTimerEvent *);
};
