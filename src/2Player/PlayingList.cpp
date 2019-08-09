
#include "PlayingList.h"

void CPlayingList::_onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem)
{
    int cy = this->rect().bottom();

    float fAlpha = 1;
    if (0 == m_nActiveTime)
    {
        fAlpha = 0.33;
    }

    bool bOutside = false;
    if (rcItem.top() < 0)
    {
        fAlpha *= pow((double)rcItem.bottom()/rcItem.height(),3.3);
        bOutside = true;
    }
    else if (rcItem.bottom() > cy)
    {
        fAlpha *= pow(double(cy - rcItem.top())/rcItem.height(),3.3);
        bOutside = true;
    }
    QColor crText(m_crText);
    crText.setAlpha(crText.alpha()*fAlpha);    
    painter.setPen(crText);

    QFont font = painter.font();

    bool bPlayingItem = uItem == m_uPlayingItem;
    if (bPlayingItem)
    {
        font.setBold(true);
        font.setPointSizeF(font.pointSizeF()+0.5);

        painter.setFont(font);
        painter.drawText(rcItem.left(), rcItem.top() + 6, rcItem.width()
            , rcItem.height(), Qt::AlignLeft|Qt::AlignVCenter, "*");
    }

    rcItem.setLeft(rcItem.left() + 30);

    m_alPlayingItems.get(uItem, [&](tagPlayingItem& playingItem){
        if (bPlayingItem || m_view.getPlayMgr().checkPlayedID(playingItem.uID))
        {
            font.setItalic(true);
            painter.setFont(font);
        }

        if (m_uShadowWidth != 0)
        {
            QColor crShadow = m_crShadow;
            UINT uShadowAlpha = crShadow.alpha();
            if (0 == m_nActiveTime)
            {
                uShadowAlpha = MIN(uShadowAlpha, crText.alpha());
                crShadow.setAlpha(uShadowAlpha);
            }

            if (bOutside)
            {
                uShadowAlpha *= fAlpha*fAlpha;
                crShadow.setAlpha(uShadowAlpha);
            }
            painter.setPen(crShadow);

            for (UINT uIdx=0; uIdx<=m_uShadowWidth; uIdx++)
            {
                if (uIdx > 1)
                {
                    crShadow.setAlpha(uShadowAlpha*(m_uShadowWidth-uIdx+1)/m_uShadowWidth);
                    painter.setPen(crShadow);
                }

                QRectF rcShadow(rcItem);
                rcShadow.setLeft(rcShadow.left()+uIdx);
                rcShadow.setTop(rcShadow.top()+uIdx);
                rcShadow.setRight(rcShadow.right()+uIdx);
                rcShadow.setBottom(rcShadow.bottom()+uIdx);
                painter.drawText(rcShadow, Qt::AlignLeft|Qt::AlignVCenter, playingItem.qsTitle);
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

void CPlayingList::_handleRowDblClick(UINT uRowIdx)
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

