
#include "PlayingList.h"

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

void CPlayingList::_onTouchMove(int dy)
{
    m_fScrollPos -= (float)dy / m_uRowHeight;
    if (dy < 0)
    {
        m_fScrollPos = MIN(m_fScrollPos, m_uMaxScrollPos);
    }
    else
    {
        m_fScrollPos = MAX(0, m_fScrollPos);
    }

    this->update();
}

void CPlayingList::_onGesture(QGesture&)
{
//    _updateActive(-1);
}

void CPlayingList::mouseDoubleClickEvent(QMouseEvent *ev)
{
    QWidget::mouseDoubleClickEvent(ev);

    float fRowIdx = (float)ev->pos().y()/m_uRowHeight + m_fScrollPos;
    if (fRowIdx < m_uItemCount)
    {
        m_view.getPlayMgr().play((UINT)fRowIdx, true);
    }

    _updateActive();
}

void CPlayingList::_onPaint(QPainter& painter, const QRect& rcPos)
{
    painter.setPen(m_crText);
    UINT uRowCount = rcPos.height()/m_uMinRowHeight;
    if (0 == uRowCount)
    {
        return;
    }
    m_uRowHeight = rcPos.height()/uRowCount;

    if (m_uItemCount > uRowCount)
    {
        m_uMaxScrollPos = m_uItemCount - uRowCount;
    }
    else
    {
        m_uMaxScrollPos = 0;
    }

    if (m_bHittestPlayingItem)
    {
        m_bHittestPlayingItem = false;

        if (m_uPlayingItem < m_fScrollPos)
        {
            m_fScrollPos = m_uPlayingItem;
        }
        else if (m_uPlayingItem+1 > m_fScrollPos+uRowCount)
        {
            m_fScrollPos = m_uPlayingItem+1-uRowCount;
        }
    }
    m_fScrollPos = MIN(m_fScrollPos, m_uMaxScrollPos);

    UINT uItem = m_fScrollPos;
    int y = rcPos.top() + (-m_fScrollPos+uItem)*m_uRowHeight;
    for (UINT uRowIdx = 0; uItem < m_uItemCount; uItem++, uRowIdx++)
    {
        QRect rcItem(0, y, rcPos.width(), m_uRowHeight);

        _onPaintItem(painter, uItem, rcItem);

        y += m_uRowHeight;
        if (y >= rcPos.bottom())
        {
            break;
        }
    }
}

void CPlayingList::_onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem)
{
    bool bPlayingItem = uItem == m_uPlayingItem;
    QFont font(m_font);
    QColor crText(m_crText);
    if (0 == m_nActiveTime)
    {
        crText.setAlpha(crText.alpha()*0.5);
    }

    if (bPlayingItem)
    {
        font.setBold(true);
        font.setPointSizeF(font.pointSizeF()+0.5);
    }

    if (rcItem.top() < 0)
    {
        int nAlpha = crText.alpha() * pow((double)rcItem.bottom()/m_uRowHeight,3.3);
        crText.setAlpha(nAlpha);
    }
    else if (rcItem.bottom() > this->height())
    {
        int nAlpha = crText.alpha() * pow(double(this->height()-rcItem.top())/m_uRowHeight,3.3);
        crText.setAlpha(nAlpha);
    }

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

    m_uItemCount = m_alPlayingItems.size();

    m_fScrollPos = 0;

    updatePlayingItem(uPlayingItem, true);
}

void CPlayingList::updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem)
{
    m_uPlayingItem = uPlayingItem;

    m_bHittestPlayingItem = bHittestPlayingItem;

    if (bHittestPlayingItem && m_nActiveTime != -1)
    {
        _updateActive();
    }
    else
    {
        CWidget::update();
    }
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
