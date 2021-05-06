
#include "xmusic.h"

#include "PlayingList.h"

cqstr tagPlayingItem::title()
{
    if (qsTitle.isEmpty())
    {
        cauto strTitle = fsutil::getFileTitle(strPath);
        auto pSinger = g_app.getSingerMgr().checkSingerDir(strPath);
        qsTitle = CFileTitle::genDisplayTitle_r(strTitle, pSinger);
    }
    return qsTitle;
}

static void _genPlayingItem(vector<tagPlayingItem>& vecPlayingItems)
{
    auto& playingItems = g_app.getPlayMgr().playingItems();
    auto uCount = playingItems.size();
    if (0 == uCount)
    {
        return;
    }

    vecPlayingItems.resize(uCount);
    auto pPlayingItem = &vecPlayingItems.front();

    for (cauto PlayItem : playingItems)
    {
        pPlayingItem->uID = PlayItem.m_uID;

        pPlayingItem->strPath = PlayItem.GetPath();

        if (PlayItem.isAbsPath())
        {
            pPlayingItem->qsTitle = __WS2Q(PlayItem.GetTitle());
        }

        pPlayingItem->uDuration = PlayItem.duration();

        pPlayingItem->eQuality = PlayItem.quality();

        pPlayingItem++;
    }
}

void CPlayingList::init()
{
    (void)m_pmPlaying.load(__png(btnPlay));

    auto uPlayingItem = g_app.getOption().uPlayingItem;

    _genPlayingItem(m_vecPlayingItems);
    _updatePlayingItem(uPlayingItem, true);

    CListView::showItem(uPlayingItem, true); // 规避后续歌手图片出现挤压的问题

    this->startTimer(1000);

#if __windows || __mac
    this->setMouseTracking(true);
#endif

    //this->grabGesture({Qt::TapAndHoldGesture});
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
    auto pPlayingItem = _playingItem(lvItem.uItem);
    if (pPlayingItem)
    {
        _onPaintItem(painter, lvItem, *pPlayingItem);
    }
}

void CPlayingList::_onPaintItem(CPainter& painter, tagLVItem& lvItem, tagPlayingItem& playingItem)
{
    auto& rc = lvItem.rc;
    int cy = height();

    float fAlpha = 1;

    bool bPlayingItem = lvItem.uItem == m_uPlayingItem;
    if (bPlayingItem)
    {
/*#if __android || __ios
        CPainterFontGuard fontGuard(painter, 0.6f, TD_FontWeight::Thin);
#endif
        painter.drawTextEx(rc, Qt::AlignLeft|Qt::AlignVCenter, "▶"
                           , m_uShadowWidth, uShadowAlpha, uTextAlpha);*/
//❥
#define __szIcon __size(18)
        QRect rcIcon(rc.x(), rc.center().y()+1-__szIcon/2, __szIcon, __szIcon);
        painter.drawImg(rcIcon, m_pmPlaying);
    }
    else
    {
        if (0 == m_nActiveTime)
        {
            fAlpha = 0.4f;
            //if (g_app.getOption().bUseThemeColor) fAlpha += 0.1f;
        }
    }

    if (rc.top() < 0)
    {
        fAlpha *= pow((float)rc.bottom()/rc.height(),3.3);
    }
    else if (rc.bottom() > cy)
    {
        fAlpha *= pow(float(cy - rc.top())/rc.height(),3.3);
    }

    UINT uTextAlpha = 255 * fAlpha;
    UINT uShadowAlpha = __ShadowAlpha *  pow(fAlpha,2.5);

#define __xMargin __size(40)
    rc.setLeft(__xMargin);

    auto nMaxRight = rc.right();// - __xMargin;
    //rc.setRight(nMaxRight);

    UINT uAlphaDiv = bPlayingItem?1:2;
    if (!bPlayingItem && playingItem.uDuration > __wholeTrackDuration)
    {
        cauto qsDuration = __WS2Q(IMedia::genDurationString(playingItem.uDuration));

        painter.adjustFontWeight(TD_FontWeight::Thin);

        auto rcPos = painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter, qsDuration
                           , m_uShadowWidth, uShadowAlpha/uAlphaDiv, uTextAlpha/uAlphaDiv);
        rc.setRight(rcPos.x() - __size(20));
    }

    int nElidedWidth = nMaxRight;
    if (bPlayingItem)
    {
        if (g_bHLayout && rc.top() < __size(120)) //右上角按钮预留
        {
            nMaxRight -= __size(90);
        }
        nElidedWidth = nMaxRight-__size(50); //音质文本预留

#if __windows || __mac
#define __fontSizeOffset 1.06f
#else
#define __fontSizeOffset 1.1f
#endif
        painter.adjustFont(__fontSizeOffset, TD_FontWeight::Normal);
    }
    else if (g_app.getPlayMgr().checkPlayedID(playingItem.uID))
    {
        painter.adjustFontItalic(true);
    }

    auto eTextFlag = Qt::TextSingleLine | Qt::TextHideMnemonic; // | Qt::TextShowMnemonic);
    cauto qsTitle = painter.fontMetrics().elidedText(playingItem.title()
            , Qt::ElideRight, nElidedWidth, eTextFlag);
    auto rcPos = painter.drawTextEx(rc, Qt::AlignLeft|Qt::AlignVCenter, qsTitle
                       , m_uShadowWidth, uShadowAlpha, uTextAlpha);
    if (//bPlayingItem &&
            playingItem.eQuality != E_MediaQuality::MQ_None)
    {
        cauto qsQuality = mediaQualityString(playingItem.eQuality);

        int yOffset = __size(8);
        int nRight = rcPos.right() + __size(35);
#if __android || __ios
        nRight += __size(6);
#endif
        if (qsQuality.length() > 2)
        {
            nRight += __size(20);
        }
        if (nRight > nMaxRight)
        {
            nRight = nMaxRight;
            yOffset *= 3;
        }
        rcPos.setRight(nRight);

        rcPos.setTop(rcPos.top() - yOffset);

        painter.adjustFont(0.66, TD_FontWeight::Thin, false);

        painter.drawTextEx(rcPos, Qt::AlignRight|Qt::AlignTop, qsQuality, m_uShadowWidth
                           , uShadowAlpha/uAlphaDiv, uTextAlpha/uAlphaDiv);
    }
}

void CPlayingList::updateList(UINT uPlayingItem) //工作线程
{
    vector<tagPlayingItem> vecPlayingItems;
    _genPlayingItem(vecPlayingItems);

    g_app.sync([=]()mutable{
        m_vecPlayingItems.swap(vecPlayingItems);
        _updatePlayingItem(uPlayingItem, true);
    });
}

void CPlayingList::_updatePlayingItem(UINT uPlayingItem, bool bHittest)
{
    m_uPlayingItem = uPlayingItem;

    if (bHittest)
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

    if (lvItem.uItem < m_vecPlayingItems.size())
    {
        //_updatePlayingItem(lvItem.uItem, false);

        g_app.getCtrl().callPlayCmd(tagPlayIndexCmd(lvItem.uItem));
    }
}

void CPlayingList::_onBlankDblClick(const QMouseEvent&)
{
    g_app.mainWnd().switchFullScreen();
}

void CPlayingList::_onTouchEvent(E_TouchEventType type, const CTouchEvent& te)
{
    static bool bFlag = false;
    if (E_TouchEventType::TET_TouchMove == type)
    {
        if (getItemCount() <= getRowCount()
                || abs(te.dy()) < abs(te.dx())
                //|| (te.dy() > 0 && scrollPos() == 0)
                //|| (te.dy() < 0 && scrollPos() + getRowCount() == getItemCount())
        ) {
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

void CPlayingList::updatePlayingItem(UINT uPlayingItem, bool bHittest, cqstr qsTitle
                       , UINT uDuration, E_MediaQuality eQuality)
{
    auto pPlayingItem = _playingItem(uPlayingItem);
    if (pPlayingItem)
    {
        pPlayingItem->qsTitle = qsTitle;
        pPlayingItem->uDuration = uDuration;
        pPlayingItem->eQuality = eQuality;
    }

    _updatePlayingItem(uPlayingItem, bHittest);
}
