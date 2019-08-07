
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
    CPlayingList(class CPlayerView& view, QWidget *parent=NULL) :
        CListView(parent, {Qt::TapAndHoldGesture})
        , m_view(view)
        , m_crShadow(128,128,128)
    {
        this->startTimer(1000);
    }

private:
    class CPlayerView& m_view;

    ArrList<tagPlayingItem> m_alPlayingItems;

    UINT m_uPlayingItem = 0;

    int m_nActiveTime = 0;

    UINT m_uShadowWidth = 0;
    QColor m_crShadow;

public:
    void setShadow(UINT uWidth, const QColor& crShadow)
    {
        m_uShadowWidth = uWidth;
        m_crShadow = crShadow;

        CWidget::update();
    }

    void setShadowWidth(UINT uWidth)
    {
        m_uShadowWidth = uWidth;

        CWidget::update();
    }

    void setShadowColor(const QColor& crShadow)
    {
        m_crShadow = crShadow;

        CWidget::update();
    }

    void updateList(UINT uPlayingItem);

    void updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem);

private:
    UINT getItemCount() override
    {
        return m_alPlayingItems.size();
    }

    void _onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem) override;

    void _handleMouseDoubleClick(UINT uRowIdx) override;

    void _onMouseEnter() override;
    void _onMouseLeave() override;

    void _onTouchBegin(const QPointF&) override;
    void _onTouchEnd() override;

    void _onGesture(QGesture&) override;

    void _updateActive(int nActiveTime=6);

    void timerEvent(QTimerEvent *);
};
