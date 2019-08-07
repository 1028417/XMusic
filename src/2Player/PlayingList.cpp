
#include "PlayingList.h"

void CPlayingList::_onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem)
{
    int cy = this->rect().bottom();

    bool bPlayingItem = uItem == m_uPlayingItem;
    QFont font(m_font);
    QColor crText(m_crText);
    float fAlpha = 1;
    if (0 == m_nActiveTime)
    {
        fAlpha = 0.5;
    }

    if (bPlayingItem)
    {
        font.setBold(true);
        font.setPointSizeF(font.pointSizeF()+0.5);
    }

    if (rcItem.top() < 0)
    {
        fAlpha *= pow((double)rcItem.bottom()/rcItem.height(),3.3);
    }
    else if (rcItem.bottom() > cy)
    {
        fAlpha *= pow(double(cy - rcItem.top())/rcItem.height(),3.3);
    }
    crText.setAlpha(crText.alpha()*fAlpha);

    m_alPlayingItems.get(uItem, [&](tagPlayingItem& playingItem){
        if (bPlayingItem || m_view.getPlayMgr().checkPlayedID(playingItem.uID))
        {
            font.setItalic(true);
        }

        painter.setFont(font);
        painter.setPen(crText);

        if (bPlayingItem)
        {
            painter.drawText(rcItem.left(), rcItem.top() + 6, rcItem.width()
                , rcItem.height(), Qt::AlignLeft|Qt::AlignVCenter, "*");
        }
        rcItem.setLeft(rcItem.left() + 30);

        if (m_uShadowWidth != 0)
        {
            for (UINT uIdx=0; uIdx<m_uShadowWidth; uIdx++)
            {
                m_crShadow.setAlpha(255*fAlpha*(m_uShadowWidth-uIdx)/m_uShadowWidth);
                painter.setPen(m_crShadow);

                painter.drawText(QRectF(rcItem.left()+uIdx, rcItem.top()+uIdx, rcItem.width(), rcItem.height())
                                 , Qt::AlignLeft|Qt::AlignVCenter, playingItem.qsTitle);
            }

            painter.setPen(crText);
        }

        painter.drawText(rcItem, Qt::AlignLeft|Qt::AlignVCenter, playingItem.qsTitle);
    });
}

void CPlayingList::updateList(UINT uPlayingItem)
{
    m_alPlayingItems.clear();

    tagPlayingItem playingItem;
    m_view.getPlayMgr().getPlayingItems()([&](const CPlayItem& PlayItem){
        playingItem.uID = PlayItem.m_uID;
        playingItem.qsTitle = wsutil::toQStr(PlayItem.GetTitle());
        m_alPlayingItems.add(playingItem);
    });

    CListView::scroll(0);

    updatePlayingItem(uPlayingItem, true);
}

void CPlayingList::updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem)
{
    m_uPlayingItem = uPlayingItem;

    if (bHittestPlayingItem)
    {
        CListView::scroll(m_uPlayingItem);
    }

    if (bHittestPlayingItem && m_nActiveTime != -1)
    {
        _updateActive();
    }
    else
    {
        CWidget::update();
    }
}

void CPlayingList::_handleMouseDoubleClick(UINT uRowIdx)
{
    _updateActive();

    if (uRowIdx < m_alPlayingItems.size())
    {
        m_view.getPlayMgr().play(uRowIdx, true);
    }
}

void CPlayingList::_onMouseEnter()
{
    _updateActive(-1);
}

void CPlayingList::_onMouseLeave()
{
    if (0 != m_nActiveTime)
    {
        _updateActive(0);
    }
}

void CPlayingList::_onTouchBegin(const QPointF&)
{
    _updateActive(-1);
}

void CPlayingList::_onTouchEnd()
{
    _updateActive();
}

void CPlayingList::_onGesture(QGesture&)
{
//    _updateActive(-1);
}

void CPlayingList::_updateActive(int nActiveTime)
{
    m_nActiveTime = nActiveTime;
    QWidget::update();
}

void CPlayingList::timerEvent(QTimerEvent *)
{
   if (m_nActiveTime > 0)
   {
       m_nActiveTime--;
       if (0 == m_nActiveTime)
       {
           QWidget::update();
       }
   }
}

