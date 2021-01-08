
#include "xmusic.h"

#include "PlayingList.h"

static void _genPlayingItem(ArrList<tagPlayingItem>& alPlayingItems)
{
    auto& singerMgr = __app.getSingerMgr();
    tagPlayingItem playingItem;
    for (cauto PlayItem : __app.getPlayMgr().playingItems())
    {
        playingItem.uID = PlayItem.m_uID;

        auto strTitle = PlayItem.GetTitle();
        auto pSinger = singerMgr.checkSingerDir(PlayItem.GetPath());
        if (pSinger)
        {
            CFileTitle::genDisplayTitle(strTitle, &pSinger->m_strName);
        }
        else
        {
            CFileTitle::genDisplayTitle(strTitle);
        }
        playingItem.qsTitle = __WS2Q(strTitle);

        alPlayingItems.add(playingItem);
    }
}

void CPlayingList::init()
{
    (void)m_pmPlaying.load(__png(btnPlay));

    auto uPlayingItem = __app.getOption().uPlayingItem;

    ArrList<tagPlayingItem> alPlayingItems;
    _genPlayingItem(alPlayingItems);
    _updateList(alPlayingItems, uPlayingItem);

    CListView::showItem(uPlayingItem, true); // 规避后续歌手图片出现挤压的问题

    this->startTimer(1000);

#if __windows || __mac
    this->setMouseTracking(true);
#endif
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
        //if (__app.getOption().bUseThemeColor) fAlpha += 0.1f;
    }
    if (rc.top() < 0)
    {
        fAlpha *= pow((float)rc.bottom()/rc.height(),3.3);
    }
    else if (rc.bottom() > cy)
    {
        fAlpha *= pow(float(cy - rc.top())/rc.height(),3.3);
    }
    UINT uTextAlpha = 255*fAlpha;
    UINT uShadowAlpha = __ShadowAlpha* pow(fAlpha,2.5);

    bool bPlayingItem = lvItem.uItem == m_uPlayingItem;
    if (bPlayingItem)
    {
/*#if __android || __ios
        CPainterFontGuard fontGuard(painter, 0.6f, QFont::Weight::Thin);
#endif
        painter.drawTextEx(rc, Qt::AlignLeft|Qt::AlignVCenter, "▶"
                           , m_uShadowWidth, uShadowAlpha, uTextAlpha);*/
//❥
#define __szIcon __size(18)
        QRect rcIcon(rc.x(), rc.center().y()+1-__szIcon/2, __szIcon, __szIcon);
        painter.drawPixmap(rcIcon, m_pmPlaying);
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
    else if (__app.getPlayMgr().checkPlayedID(playingItem.uID))
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

        cauto qsQuality = __app.mainWnd().playingInfo().qsQuality;

        int nRight = rcPos.right() + __size(35);
#if __android || __ios
        nRight += __size(6);
#endif
        if (qsQuality.length() > 2)
        {
            nRight += __size(20);
        }
        rcPos.setRight(MIN(nRight, nMaxRight));

        rcPos.setTop(rcPos.top() - __size(8));

        painter.drawTextEx(rcPos, Qt::AlignRight|Qt::AlignTop, qsQuality, 1, uShadowAlpha, uTextAlpha);
    }
}

void CPlayingList::updateList(UINT uPlayingItem) //工作线程
{
    ArrList<tagPlayingItem> alPlayingItems;
    _genPlayingItem(alPlayingItems);

    __app.sync([=]()mutable{
        _updateList(alPlayingItems, uPlayingItem);
    });
}

void CPlayingList::_updateList(ArrList<tagPlayingItem>& alPlayingItems, UINT uPlayingItem)
{
    m_alPlayingItems.swap(alPlayingItems);
    updatePlayingItem(uPlayingItem, true);
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

void CPlayingList::_onItemDblClick(tagLVItem& lvItem, const QMouseEvent&)
{
    //_updateActive();

    if (lvItem.uItem < m_alPlayingItems.size())
    {
        //updatePlayingItem(lvItem.uItem, false);

        __app.getCtrl().callPlayCmd(tagPlayIndexCmd(lvItem.uItem));
    }
}

void CPlayingList::_onBlankDblClick(const QMouseEvent&)
{
    __app.mainWnd().switchFullScreen();
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
            ((MainWindow*)parent())->handleTouchEvent(type, te);
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

            if (abs(te.dx()) > abs(te.dy()))
            {
                ((MainWindow*)parent())->handleTouchEvent(type, te);
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
