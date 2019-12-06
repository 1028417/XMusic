
#include "PlayingList.h"

#include "app.h"

CPlayingList::CPlayingList(class CXMusicApp& app)
    : CListView(NULL)
    , m_app(app)
{
    this->startTimer(1000);
}

size_t CPlayingList::getPageRowCount()
{
    size_t uRet = CListView::getPageRowCount();
    size_t uRowCount = getRowCount();
    if (uRowCount >= 7)
    {
        if (uRowCount < uRet)
        {
            uRet = uRowCount;
        }
    }
    else
    {
        uRet = 7;
    }

    return uRet;
}

void CPlayingList::_onPaintRow(CPainter& painter, const tagLVRow& lvRow)
{
    int cy = this->rect().bottom();

    float fAlpha = 1;
    if (0 == m_nActiveTime)
    {
        fAlpha = m_fInactiveAlpha;
    }

    QRect rc = lvRow.rc;
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

    bool bPlayingItem = lvRow.uRow == m_uPlayingItem;
    if (bPlayingItem)
    {
        float fPlayingFontSize = 0.95f;
#if __windows || __mac
        fPlayingFontSize = 1;
#endif
        painter.setFont(fPlayingFontSize, E_FontWeight::FW_SemiBold);

        painter.drawText(rc.left(), rc.top() + __size(3), rc.width()
            , rc.height(), Qt::AlignLeft|Qt::AlignVCenter, "*");
    }

    rc.setLeft(rc.left() + __size(35));

    m_alPlayingItems.get(lvRow.uRow, [&](tagPlayingItem& playingItem){
        if (!bPlayingItem && m_app.getPlayMgr().checkPlayedID(playingItem.uID))
        {
            painter.adjustFontItalic(true);
        }

        QString qsTitle = painter.fontMetrics().elidedText(playingItem.qsTitle
                , Qt::ElideRight, rc.width(), Qt::TextSingleLine | Qt::TextShowMnemonic);
        if (m_uShadowWidth > 0)
        {
            UINT uShadowAlpha = crText.alpha()*fAlpha * m_uShadowAlpha/255;
            QColor crShadow(128, 128, 128, uShadowAlpha);

            for (UINT uIdx=0; uIdx<=m_uShadowWidth; uIdx++)
            {
                if (uIdx > 1)
                {
                    crShadow.setAlpha(uShadowAlpha*(m_uShadowWidth-uIdx+1)/m_uShadowWidth);
                }

                painter.setPen(crShadow);

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
    m_app.getPlayMgr().getPlayingItems()([&](const CPlayItem& PlayItem){
        playingItem.uID = PlayItem.m_uID;
        playingItem.qsTitle = strutil::toQstr(PlayItem.GetTitle());
        m_alPlayingItems.add(playingItem);
    });

    CListView::reset();

    updatePlayingItem(uPlayingItem, true);
}

void CPlayingList::updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem)
{
    m_uPlayingItem = uPlayingItem;

    if (bHittestPlayingItem)
    {
        CListView::showRow(m_uPlayingItem);
    }

    if (bHittestPlayingItem && m_nActiveTime != -1)
    {
        _updateActive();
    }
    else
    {
        update();
    }
}

void CPlayingList::_onRowDblClick(const tagLVRow& lvRow, const QMouseEvent&)
{
    //_updateActive();

    if (lvRow.uRow < m_alPlayingItems.size())
    {
        //updatePlayingItem(lvRow.uRow, false);

        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(lvRow.uRow));
    }
}

void CPlayingList::_onMouseEnter()
{
    _updateActive(); //_updateActive(-1);
}

/*void CPlayingList::_onMouseLeave()
{
    if (0 != m_nActiveTime)
    {
        _updateActive(0);
    }
}*/

void CPlayingList::_onTouchEvent(E_TouchEventType type, const CTouchEvent& te)
{
    if (E_TouchEventType::TET_TouchBegin == type)
    {
        _updateActive(-1);
    }
    else if (E_TouchEventType::TET_TouchEnd == type)
    {
        _updateActive();
    }

    CListView::_onTouchEvent(type, te);
}

void CPlayingList::_updateActive(int nActiveTime)
{
    m_nActiveTime = nActiveTime;
    update();
}

void CPlayingList::_onAutoScrollBegin()
{
    _updateActive(-1);
}

void CPlayingList::_onAutoScrollEnd()
{
    _updateActive();
}

void CPlayingList::timerEvent(QTimerEvent *)
{
    if (isAutoScrolling())
    {
        return;
    }

    if (m_nActiveTime > 0)
    {
        m_nActiveTime--;
        if (0 == m_nActiveTime)
        {
            update();
        }
    }
}
