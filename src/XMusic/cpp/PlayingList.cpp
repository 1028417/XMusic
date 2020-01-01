
#include "PlayingList.h"

#include "app.h"

CPlayingList::CPlayingList(class CApp& app)
    : CListView(NULL)
    , m_app(app)
{
#if __windows || __mac
    this->setMouseTracking(true);
#endif

    this->startTimer(1000);
}

size_t CPlayingList::getPageRowCount()
{
    size_t uPageRowCount = m_uPageRowCount;
    size_t uRowCount = getRowCount();
    if (uRowCount >= 7)
    {
        if (uRowCount < uPageRowCount)
        {
            uPageRowCount = uRowCount;
        }
    }
    else
    {
        uPageRowCount = 7;
    }

    return uPageRowCount;
}

void CPlayingList::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{
    auto& rc = lvRow.rc;

    int cy = height();

    float fAlpha = 1;
    if (0 == m_nActiveTime)
    {
        if (m_app.getOption().bUseThemeColor)
        {
            fAlpha = 0.5f;
        }
        else
        {
            fAlpha = 0.3f;
        }
    }
    if (rc.top() < 0)
    {
        fAlpha *= pow((double)rc.bottom()/rc.height(),3.3);
    }
    else if (rc.bottom() > cy)
    {
        fAlpha *= pow(double(cy - rc.top())/rc.height(),3.3);
    }
    UINT uTextAlpha = 255*fAlpha;
    UINT uShadowAlpha = __ShadowAlpha* pow(fAlpha,2.5);

    bool bPlayingItem = lvRow.uRow == m_uPlayingItem;
    if (bPlayingItem)
    {
        float fPlayingFontSize = 0.95f;
#if __windows || __mac
        fPlayingFontSize = 1;
#endif
        painter.setFont(fPlayingFontSize, E_FontWeight::FW_SemiBold);

        painter.drawTextEx(QRect(rc.left(), rc.top() + __size(3), rc.width()
            , rc.height()), Qt::AlignLeft|Qt::AlignVCenter, "*"
                           , m_uShadowWidth, uShadowAlpha, uTextAlpha);
    }

    rc.setLeft(rc.left() + __size(35));

    m_alPlayingItems.get(lvRow.uRow, [&](tagPlayingItem& playingItem){
        if (!bPlayingItem && m_app.getPlayMgr().checkPlayedID(playingItem.uID))
        {
            painter.adjustFontItalic(true);
        }

        QString qsTitle = painter.fontMetrics().elidedText(playingItem.qsTitle
                , Qt::ElideRight, rc.width(), Qt::TextSingleLine | Qt::TextShowMnemonic);
        painter.drawTextEx(rc, Qt::AlignLeft|Qt::AlignVCenter, qsTitle
                           , m_uShadowWidth, uShadowAlpha, uTextAlpha);
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

void CPlayingList::_onRowDblClick(tagLVRow& lvRow, const QMouseEvent&)
{
    //_updateActive();

    if (lvRow.uRow < m_alPlayingItems.size())
    {
        //updatePlayingItem(lvRow.uRow, false);

        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(lvRow.uRow));
    }
}

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
