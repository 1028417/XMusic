
#include "PlayingList.h"

#include "app.h"

CPlayingList::CPlayingList(class CApp& app, QWidget *parent)
    : CListView(parent)
    , m_app(app)
{
#if __windows || __mac
    this->setMouseTracking(true);
#endif

    this->startTimer(1000);
}

size_t CPlayingList::getRowCount() const
{
    size_t uPageRowCount = m_uPageRowCount;
    size_t uRowCount = getItemCount();
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

void CPlayingList::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    m_alPlayingItems.get(lvItem.uItem, [&](tagPlayingItem& playingItem){
        _onPaintItem(painter, lvItem, playingItem);
    });
}

void CPlayingList::_onPaintItem(CPainter& painter, tagLVItem& lvItem, const tagPlayingItem& playingItem)
{
    auto& rc = lvItem.rc;

    int cy = height();

    float fAlpha = 1;
    if (0 == m_nActiveTime)
    {
        fAlpha = 0.4f;
        //if (m_app.getOption().bUseThemeColor) fAlpha += 0.1f;
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

    bool bPlayingItem = lvItem.uItem == m_uPlayingItem;
    if (bPlayingItem)
    {
#if __android || __ios
        CPainterFontGuard fontGuard(painter, 0.6f, QFont::Weight::Thin);
#endif
        painter.drawTextEx(rc, Qt::AlignLeft|Qt::AlignVCenter, "▶"
                           , m_uShadowWidth, uShadowAlpha, uTextAlpha);
    }

    rc.setLeft(__size(35));

    auto nMaxRight = rc.right()-__size(35);
    rc.setRight(nMaxRight);

    int nElidedWidth = nMaxRight;
    if (bPlayingItem)
    {
#if __windows || __mac
        if (rc.bottom() < __size(150))
        {
            nMaxRight -= __size(90);
        }
#endif
        nElidedWidth = nMaxRight-__size(50);

        painter.adjustFont(1.05f, QFont::Weight::Normal);
    }
    else if (m_app.getPlayMgr().checkPlayedID(playingItem.uID))
    {
        painter.adjustFont(true);
    }

    auto eTextFlag = Qt::TextSingleLine | Qt::TextHideMnemonic; // | Qt::TextShowMnemonic);
    QString qsTitle = painter.fontMetrics().elidedText(playingItem.qsTitle
            , Qt::ElideRight, nElidedWidth, eTextFlag);
    auto rcPos = painter.drawTextEx(rc, Qt::AlignLeft|Qt::AlignVCenter, qsTitle
                       , m_uShadowWidth, uShadowAlpha, uTextAlpha);

    if (bPlayingItem)
    {
        painter.adjustFont(0.65, QFont::Weight::Thin);

        cauto qsQuality = m_app.mainWnd().playingInfo().qsQuality;
        auto xOffset = __size(35);
#if __android || __ios
        xOffset += __size(10);
#endif
        if (qsQuality.length() > 2)
        {
            xOffset += __size(20);
        }

        rcPos.setRight(rcPos.right() + xOffset);
        if (rcPos.right() > nMaxRight)
        {
            rcPos.setRight(nMaxRight);
        }
        rcPos.setTop(rcPos.top() - __size(8));

        painter.drawTextEx(rcPos, Qt::AlignRight|Qt::AlignTop, qsQuality, 1, uShadowAlpha, uTextAlpha);
    }
}

void CPlayingList::updateList(UINT uPlayingItem)
{
    m_alPlayingItems.clear();

    tagPlayingItem playingItem;
    m_app.getPlayMgr().playingItems()([&](const CPlayItem& PlayItem){
        playingItem.uID = PlayItem.m_uID;
        playingItem.qsTitle = __WS2Q(PlayItem.GetTitle());
        m_alPlayingItems.add(playingItem);
    });

    updatePlayingItem(uPlayingItem, false);
}

void CPlayingList::updatePlayingItem(UINT uPlayingItem, bool bHittestPlayingItem)
{
    m_uPlayingItem = uPlayingItem;

    if (bHittestPlayingItem)
    {
        CListView::showItem(m_uPlayingItem);

        if (m_nActiveTime != -1)
        {
            _updateActive();
        }
    }
    else
    {
        update();
    }
}

void CPlayingList::_onRowDblClick(tagLVItem& lvItem, const QMouseEvent&)
{
    //_updateActive();

    if (lvItem.uItem < m_alPlayingItems.size())
    {
        //updatePlayingItem(lvItem.uItem, false);

        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(lvItem.uItem));
    }
}

void CPlayingList::_onBlankDblClick(const QMouseEvent&)
{
    m_app.mainWnd().switchFullScreen();
}

void CPlayingList::_onTouchEvent(E_TouchEventType type, const CTouchEvent& te)
{
    static bool bFlag = false;
    if (E_TouchEventType::TET_TouchMove == type)
    {
        if (abs(te.dy()) < abs(te.dx())
                //|| (te.dy() > 0 && scrollPos() == 0)
                //|| (te.dy() < 0 && scrollPos() + getRowCount() == getItemCount())
                || getItemCount() <= getRowCount())
        {
            ((MainWindow*)parent())->handleTouchMove(te);
            bFlag = true;
            return;
        }
    }
    else if (E_TouchEventType::TET_TouchBegin == type)
    {
        _updateActive(-1);
    }
    else if (E_TouchEventType::TET_TouchEnd == type)
    {
        _updateActive();

        if (bFlag)
        {
            bFlag = false;

            auto dx = te.x() - m_teBegin.x();
            auto dy = te.y() - m_teBegin.y();
            if (abs(dx) > abs(dy))
            {
                ((MainWindow*)parent())->handleTouchEnd(m_teBegin, te);
            }

            return;
        }
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
