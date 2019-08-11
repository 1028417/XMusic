
#include "PlayingList.h"

void CPlayingList::_onPaintItem(QPainter& painter, QRect& rc, const tagListViewItem& item)
{
    int cy = this->rect().bottom();

    float fAlpha = 1;
    if (0 == m_nActiveTime)
    {
        fAlpha = m_fInactiveAlpha;
    }

    if (rc.top() < 0)
    {
        fAlpha *= pow((double)rc.bottom()/rc.height(),3.3);
    }
    else if (rc.bottom() > cy)
    {
        fAlpha *= pow(double(cy - rc.top())/rc.height(),3.3);
    }
    QColor crText(m_crText);
    crText.setAlpha(crText.alpha()*fAlpha);    
    painter.setPen(crText);

    QFont font = painter.font();

    bool bPlayingItem = item.uItem == m_uPlayingItem;
    if (bPlayingItem)
    {
        font.setBold(true);
        font.setPointSizeF(font.pointSizeF()+0.5);

        painter.setFont(font);
        painter.drawText(rc.left(), rc.top() + 6, rc.width()
            , rc.height(), Qt::AlignLeft|Qt::AlignVCenter, "*");
    }

    rc.setLeft(rc.left() + 30);

    m_alPlayingItems.get(item.uItem, [&](tagPlayingItem& playingItem){
        if (bPlayingItem || m_view.getPlayMgr().checkPlayedID(playingItem.uID))
        {
            font.setItalic(true);
            painter.setFont(font);
        }

        QString qsTitle = painter.fontMetrics().elidedText(playingItem.qsTitle
                , Qt::ElideRight, rc.width(), Qt::TextSingleLine | Qt::TextShowMnemonic);
        if (m_uShadowWidth != 0)
        {
            UINT uShadowAlpha = crText.alpha()*fAlpha;
            QColor crShadow = m_crShadow;
            crShadow.setAlpha(uShadowAlpha);
            painter.setPen(crShadow);

            for (UINT uIdx=0; uIdx<=m_uShadowWidth; uIdx++)
            {
                if (uIdx > 1)
                {
                    crShadow.setAlpha(uShadowAlpha*(m_uShadowWidth-uIdx+1)/m_uShadowWidth);
                    painter.setPen(crShadow);
                }

                QRectF rcShadow(rc);
                rcShadow.setLeft(rcShadow.left()+uIdx);
                rcShadow.setTop(rcShadow.top()+uIdx);
                rcShadow.setRight(rcShadow.right()+uIdx);
                rcShadow.setBottom(rcShadow.bottom()+uIdx);
                painter.drawText(rcShadow, Qt::AlignLeft|Qt::AlignVCenter, qsTitle);
            }

            painter.setPen(crText);
        }

        painter.drawText(rc, Qt::AlignLeft|Qt::AlignVCenter, qsTitle);
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

    CListView::showItem(0);

    updatePlayingItem(uPlayingItem, true);
}

void CPlayingList::updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem)
{
    m_uPlayingItem = uPlayingItem;

    if (bHittestPlayingItem)
    {
        CListView::showItem(m_uPlayingItem);
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

void CPlayingList::_handleRowDblClick(UINT uRowIdx, QMouseEvent&)
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

