
#include "app.h"

#include "MedialibView.h"

#define __XMusicDirName L"XMusic"
#define __OuterDirName  L" 本地"

#define __FlashingAlpha 170

#define __RemarkAlpha 200

CMedialibView::CMedialibView(CMedialibDlg& medialibDlg, class CApp& app, CMediaDir &OuterDir) :
    CListViewEx(medialibDlg, E_LVScrollBar::LVSB_Left)
    , m_medialibDlg(medialibDlg)
    , m_app(app)
    , m_SingerLib(app.getSingerMgr())
    , m_PlaylistLib(app.getPlaylistMgr())
    , m_OuterDir(OuterDir)
{
    connect(this, &CMedialibView::signal_update, this, [&](){
        update();
    });
}

void CMedialibView::initpm()
{
    (void)m_pmSSFile.load(__mediaPng(media));

    (void)m_pmSingerGroup.load(__mediaPng(singergroup));
    (void)m_pmDefaultSinger.load(__mediaPng(singerdefault));
    (void)m_pmAlbum.load(__mediaPng(album));

    (void)m_pmPlaylistSet.load(__mediaPng(playlistset));
    (void)m_pmPlaylist.load(__mediaPng(playlist));
    (void)m_pmPlayItem.load(__mediaPng(playitem));

    (void)m_pmXmusicDir.load(__mediaPng(xmusicdir));
    (void)m_pmSSDir.load(__mediaPng(ssdir));

    (void)m_pmHires.load(__mediaPng(hires));
    (void)m_pmDSD.load(__mediaPng(dsd));

    (void)m_pmDir.load(__mediaPng(dir));
    (void)m_pmDirLink.load(__mediaPng(dirLink));
    (void)m_pmFile.load(__mediaPng(file));

    (void)m_pmPlay.load(":/img/btnPlay.png");
    m_pmPlayOpacity = CPainter::alphaPixmap(m_pmPlay, 128);
    (void)m_pmAddPlay.load(":/img/btnAddplay.png");
    m_pmAddPlayOpacity = CPainter::alphaPixmap(m_pmAddPlay, 128);
}

void CMedialibView::_onShowRoot()
{
    m_medialibDlg.updateHead(L"媒体库");
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    WString strTitle;
    _getTitle(MediaSet, strTitle);
    m_medialibDlg.updateHead(strTitle);

    if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
    {
        m_thrAsyncTask.start([&](const bool& bRunSignal){
            ((CPlaylist&)MediaSet).playItems()([&](const CPlayItem& playItem){
                ((CPlayItem&)playItem).findRelatedMedia(E_MediaSetType::MST_Album);

                mtutil::usleep(10);

                return bRunSignal;
            });

            if (bRunSignal)
            {
                emit signal_update();
            }
        });
    }
}

void CMedialibView::_onShowDir(CPath& dir)
{
    WString strTitle;
    _getTitle(dir, strTitle);
    m_medialibDlg.updateHead(strTitle);
}

void CMedialibView::_getTitle(CMediaSet& MediaSet, WString& strTitle)
{
    if (&MediaSet == &m_SingerLib || &MediaSet == &m_PlaylistLib)
    {
        strTitle << MediaSet.m_strName;
    }
    else if (E_MediaSetType::MST_Album == MediaSet.m_eType)
    {
        strTitle << MediaSet.m_pParent->m_strName << __CNDot << MediaSet.m_strName;
    }
    else
    {
        if (MediaSet.m_pParent)
        {
            strTitle << MediaSet.m_pParent->m_strName; //_getTitle(*MediaSet.m_pParent, strTitle);
            strTitle << __CNDot;
        }

        strTitle << MediaSet.m_strName;
    }
}

void CMedialibView::_getTitle(CPath& dir, WString& strTitle)
{
    if (&dir == &__medialib)
    {
        strTitle << __XMusicDirName;
    }
    else if (&dir == &m_OuterDir)
    {
        strTitle << __OuterDirName;
    }
    else
    {
        strTitle << dir.fileName();

        /*auto pParent = dir.parent();
        if (NULL == pParent)
        {
            pParent = &m_MediaLib;
        }

        _getTitle(*pParent, strTitle);

        strTitle << __wcDirSeparator << dir.GetName();*/
    }
}

size_t CMedialibView::getPageRowCount() const
{
    if (isInRoot())
    {
        return _getRootRowCount();
    }

    return CMedialibDlg::getPageRowCount(m_medialibDlg.height());
}

size_t CMedialibView::getColumnCount() const
{
    if (isInRoot() && m_medialibDlg.isHLayout())
    {
        return 2;
    }

    return 1;
}

size_t CMedialibView::_getRootRowCount() const
{
    if (m_medialibDlg.isHLayout())
    {
        return 6;
    }
    else
    {
        return 10;
    }
}

bool CMedialibView::_genRootRowContext(tagMLItemContext& context)
{
    context.eStyle = E_LVItemStyle::IS_CenterAlign;

    bool bHLayout = m_medialibDlg.isHLayout();
    if (bHLayout)
    {
        context.fIconMargin /= 1.3;
    }

    context.uIconRound = 0;

    cauto uRow = context->uRow;
    cauto uCol = context->uCol;
    if ((bHLayout && 1 == uRow && 0 == uCol) || (!bHLayout && 1 == uRow))
    {
        context.pmImg = &m_pmSingerGroup;
        context.strText = L" 歌手";
        context.pMediaSet = &m_SingerLib;
        return true;
    }
    else if ((bHLayout && 1 == uRow && 1 == uCol) || (!bHLayout && 3 == uRow))
    {
        context.pmImg = &m_pmPlaylistSet;
        context.strText = L" 歌单";
        context.pMediaSet = &m_PlaylistLib;
        return true;
    }
    else if ((bHLayout && 3 == uRow && 0 == uCol) || (!bHLayout && 5 == uRow))
    {
        context.pmImg = &m_pmXmusicDir;
        context.strText = __XMusicDirName;
        context.pDir = &__medialib;
        return true;
    }
    else if ((bHLayout && 3 == uRow && 1 == uCol) || (!bHLayout && 7 == uRow))
    {
        context.pmImg = &m_pmDir;
        context.strText << ' ' << __OuterDirName;
        context.pDir = &m_OuterDir;
        return true;
    }

    return false;
}

cqpm CMedialibView::_getSingerPixmap(UINT uSingerID, cwstr strSingerName)
{
    auto& pSingerPixmap = m_mapSingerPixmap[uSingerID];
    if (pSingerPixmap)
    {
        return *pSingerPixmap;
    }

    cauto strSingerImg = m_app.getSingerImgMgr().getSingerImg(strSingerName, 0);
    if (!strSingerImg.empty())
    {
        m_lstSingerPixmap.emplace_back();
        QPixmap& pm = m_lstSingerPixmap.back();
        if (pm.load(__WS2Q(strSingerImg)))
        {
#define __singerimgZoomout 160
            auto&& temp = pm.width() < pm.height()
                    ? pm.scaledToWidth(__singerimgZoomout, Qt::SmoothTransformation)
                    : pm.scaledToHeight(__singerimgZoomout, Qt::SmoothTransformation);
            pm.swap(temp);

            pSingerPixmap = &pm;
            return pm;
        }
        else
        {
            m_lstSingerPixmap.pop_back();
        }
    }

    return m_pmDefaultSinger;
}

void CMedialibView::_genMediaContext(tagMLItemContext& context)
{
    context.eStyle |= E_LVItemStyle::IS_MultiLine;
    if (context.pMediaSet)
    {
        switch (context.pMediaSet->m_eType)
        {
        case E_MediaSetType::MST_Playlist:
            context.pmImg = &m_pmPlaylist;
            break;
        case E_MediaSetType::MST_Album:
            context.pmImg = &m_pmAlbum;
            break;
        case E_MediaSetType::MST_Singer:
            context.pmImg = &_getSingerPixmap(context.pMediaSet->m_uID,
                                               context.pMediaSet->m_strName);
            break;
        case E_MediaSetType::MST_SingerGroup:
            context.pmImg = &m_pmSingerGroup;

            context.eStyle |= E_LVItemStyle::IS_RightTip;
        default:
            break;
        };
    }
    else if (context.pMedia)
    {
        if (context.pMedia->GetMediaSetType() == E_MediaSetType::MST_Playlist)
        {
            context.pmImg = &m_pmPlayItem;

            UINT uSingerID = context.pMedia->GetRelatedMediaSetID(E_MediaSetType::MST_Singer);
            if (uSingerID > 0)
            {
                cauto strSingerName = context.pMedia->GetRelatedMediaSetName(E_MediaSetType::MST_Singer);
                if (!strSingerName.empty())
                {
                    context.pmImg = &_getSingerPixmap(uSingerID, strSingerName);
                }
            }
        }
        //else context.pixmap = &m_pmAlbumItem;
    }
    else if (context.pDir)
    {
        if (context.pDir->rootDir() == &__medialib)
        {
            context.pmImg = &m_pmSSDir;

            if (context.pDir->parent() == &__medialib)
            {
                auto strDirName = context.pDir->fileName();
                strutil::lowerCase(strDirName);
                if (strDirName.find(L"hires") != __wnpos)
                {
                    context.pmImg = &m_pmHires;
                }
                else if (strDirName.find(L"dsd") != __wnpos)
                {
                    context.pmImg = &m_pmDSD;
                }
                /*else if (strDirName.find(L"CD整轨") != __wnpos)
                {
                    context.pixmap = &m_pmCdDiskdir;
                }
                else if (strDirName.find(L"无损整轨") != __wnpos)
                {
                    context.pixmap = &m_pmllDiskdir;
                }*/
            }
        }
        else
        {
            context.pmImg = &m_pmDir;

            if (context.pDir->parent() == NULL)
            {
                CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(context.pDir);
                if (pAttachDir)
                {
                    context.pmImg = &m_pmDirLink;

                    /*if (E_AttachDirType::ADT_TF == pAttachDir->m_eType)
                    {
                        context.strRemark = L"扩展SD";
                    }
                    else if (E_AttachDirType::ADT_USB == pAttachDir->m_eType)
                    {
                        context.strRemark = L"USB";
                    }*/
                }
            }
        }
    }
    else if (context.pFile)
    {
        auto pMediaRes = ((CMediaRes*)context.pFile);
        if (pMediaRes->parent()->dirType() == E_MediaDirType::MDT_Snapshot)
        {
            if (pMediaRes->duration() > __wholeTrackDuration)
            {
                context.uIconRound = 0;
                if (pMediaRes->quality() >= E_MediaQuality::MQ_CD)
                {
                    context.pmImg = &m_app.m_pmHDDisk;
                }
                else
                {
                    context.pmImg = &m_app.m_pmLLDisk;
                }
            }
            else
            {
                context.pmImg = &m_pmSSFile;
            }

            context.strText = pMediaRes->GetTitle();
        }
        else
        {
            context.pmImg = &m_pmFile;
            context.strText = pMediaRes->fileName();
        }
    }
}

#define __rAlign Qt::AlignRight|Qt::AlignVCenter

inline static cqrc _paintRemark(CPainter& painter, cqrc rc, cqstr qsRemark)
{
    CPainterFontGuard fontGuard(painter, 0.8, QFont::Weight::ExtraLight);
    UINT uAlpha = CPainter::oppTextAlpha(__RemarkAlpha);
    return painter.drawTextEx(rc, __rAlign, qsRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);
}

cqrc CMedialibView::_paintText(tagLVItemContext& context, CPainter& painter, QRect& rc
                               , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    cauto mlContext = (tagMLItemContext&)context;

    if (mlContext.playable())
    {
        UINT cy = context->rc.height();
        int yMargin = cy * context.fIconMargin*1.3;
        cy -= yMargin*2;

        int x_icon = context->rc.right() + __playIconOffset - cy;
        int y_icon = context->rc.top()+yMargin;
        QRect rcPlayIcon(x_icon, y_icon, cy, cy);

        bool bFlashing = (int)mlContext->uRow == m_nFlashingRow;
        cauto pm = mlContext.pMediaSet
                ?(bFlashing?m_pmPlayOpacity:m_pmPlay)
               :(bFlashing?m_pmAddPlayOpacity:m_pmAddPlay);
        painter.drawPixmap(rcPlayIcon, pm);

        rc.setRight(x_icon-__lvRowMargin+__playIconOffset);
    }

    QString qsMediaQuality;
    WString strRemark;
    if (mlContext.pMedia)
    {
        if (mlContext.pMedia->GetMediaSetType() == E_MediaSetType::MST_Album)
        {
            rc.setLeft(rc.left() + __size(10));
        }
    }
    else if (mlContext.pMediaSet)
    {
        if (E_MediaSetType::MST_SingerGroup == mlContext.pMediaSet->m_eType)
        {
            auto pSingerGroup = (CSingerGroup*)mlContext.pMediaSet;
            size_t uAlbumCount = 0;
            cauto lstSingers = pSingerGroup->singers();
            for (cauto singer : lstSingers)
            {
                uAlbumCount += singer.albums().size();
            }
            strRemark << lstSingers.size() << L" 歌手\n" << uAlbumCount << L" 专辑";
        }
        else if (E_MediaSetType::MST_Singer == mlContext.pMediaSet->m_eType)
        {
            cauto lstAlbum = ((CSinger*)mlContext.pMediaSet)->albums();
            size_t uAlbumItemCount = 0;
            for (cauto album : lstAlbum)
            {
                uAlbumItemCount += album.albumItems().size();
            }
            strRemark << lstAlbum.size() << L" 专辑\n" << uAlbumItemCount << L" 曲目";
        }
        else if (E_MediaSetType::MST_Album == mlContext.pMediaSet->m_eType)
        {
            auto pAlbum = (CAlbum*)mlContext.pMediaSet;
            strRemark << pAlbum->albumItems().size() << L" 曲目";
        }
        else if (E_MediaSetType::MST_Playlist == mlContext.pMediaSet->m_eType)
        {
            auto pPlaylist = (CPlaylist*)mlContext.pMediaSet;
            strRemark << pPlaylist->size() << L" 曲目";
        }
    }
    if (!strRemark->empty())
    {
        _paintRemark(painter, rc, strRemark);
    }
    else
    {
        IMedia *pMedia = mlContext.pMedia;
        if (NULL == pMedia)
        {
            auto pMediaRes = (CMediaRes*)mlContext.pFile;
            if (pMediaRes)
            {
                pMedia = pMediaRes;

                if (pMediaRes->parent()->dirType() == E_MediaDirType::MDT_Snapshot)
                {
                    auto duration = pMediaRes->duration();
                    cauto cue = ((CSnapshotMediaRes*)pMediaRes)->cueFile();
                    if (cue)
                    {
                        WString strRemark;
                        strRemark << IMedia::genDurationString(duration) << '\n' << cue.m_alTrackInfo.size() << L"曲目";
                        _paintRemark(painter, rc, strRemark);

                        //QColor cr = g_crFore;
                        //cr.setAlpha(CPainter::oppTextAlpha(__RemarkAlpha)/2);
                        //painter.drawRectEx(QRect(rcRet.left(), rcRet.bottom()+1, rcRet.width(), 1), cr);

                        rc.setRight(rc.right() - __size(120));
                    }
                    else if (duration > __wholeTrackDuration)
                    {
                        _paintRemark(painter, rc, __WS2Q(L'\n' + IMedia::genDurationString(duration)));
                        rc.setRight(rc.right() - __size(100));
                    }
                }
            }
        }

        if (pMedia)
        {
            qsMediaQuality = mediaQualityString(pMedia->quality());
            rc.setRight(rc.right() - __size(20) - __size(10)*qsMediaQuality.length());
        }
    }

    uTextAlpha = 255;
    uShadowAlpha = __ShadowAlpha;
    if ((int)mlContext->uRow == m_nFlashingRow)
    {
        uTextAlpha = __FlashingAlpha;
        uShadowAlpha = uShadowAlpha*__FlashingAlpha/300;
    }

    cauto rcRet = CListView::_paintText(context, painter, rc, flags, uShadowAlpha, uTextAlpha);

    if (!qsMediaQuality.isEmpty())
    {
        CPainterFontGuard fontGuard(painter, 0.7, QFont::Weight::Thin);

        auto rcPos = rcRet;
        rcPos.setLeft(rcPos.right() + __size(20));
        rcPos.setTop(rcPos.top() - __size(9));
        rcPos.setRight(10000);
        painter.drawTextEx(rcPos, Qt::AlignLeft|Qt::AlignTop, qsMediaQuality, 1, uShadowAlpha, uTextAlpha);
    }

    return rcRet;
}

inline static bool _hittestPlayIcon(const tagMLItemContext& context, int x)
{
    if (!context.playable())
    {
        return false;
    }

    int xPlayIcon = context->rc.right() - __lvRowMargin + __playIconOffset - context->rc.height();
    return x >= xPlayIcon;
}

void CMedialibView::_onRowClick(tagLVItem& lvItem, const QMouseEvent& me, CMediaSet& mediaSet)
{
    if (_hittestPlayIcon(tagMLItemContext(lvItem, mediaSet), me.x()))
    {
        _flashRow(lvItem.uRow);

        TD_MediaList lstMedias;
        mediaSet.GetAllMedias(lstMedias);
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(lstMedias)));

        return;
    }

    CListViewEx::_onRowClick(lvItem, me, mediaSet);
}

void CMedialibView::_onRowClick(tagLVItem& lvItem, const QMouseEvent& me, CPath& path)
{
    auto& mediaRes = (CMediaRes&)path;
    if (!_hittestPlayIcon(tagMLItemContext(lvItem, (IMedia&)mediaRes), me.x()))
    {
        if (m_medialibDlg.tryShowWholeTrack(mediaRes))
        {
            selectRow(lvItem.uRow);
            return;
        }
    }

    _onMediaClick(lvItem, me, mediaRes);
}

void CMedialibView::_onMediaClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media)
{
    if (_hittestPlayIcon(tagMLItemContext(lvItem, media), me.x()))
    {
        _flashRow(lvItem.uRow);

        bool bPlay = false;
        if (m_app.getPlayMgr().playStatus() != E_PlayStatus::PS_Play)
        {
            bPlay = true;
        }
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(media, bPlay));

        return;
    }

    selectRow(lvItem.uRow);
    _flashRow(lvItem.uRow);

    if (me.x() <= (int)rowHeight())
    {
        if (media.GetMediaSetType() == E_MediaSetType::MST_Playlist)
        {
            /*UINT uSingerID = media.GetRelatedMediaSetID(E_MediaSetType::MST_Singer);
            if (uSingerID > 0)
            {
                auto pSinger = m_SingerLib.FindSubSet(E_MediaSetType::MST_Singer, uSingerID);
                if (pSinger)
                {
                    showMediaSet(*pSinger);
                    return;
                }
            }*/

            auto pAlbumItem = media.findRelatedMedia(E_MediaSetType::MST_Album);
            if (pAlbumItem)
            {
                hittestMedia(*pAlbumItem);
                return;
            }
        }
    }

    m_app.getCtrl().callPlayCtrl(tagPlayCtrl(media, true));
}

CMediaSet* CMedialibView::_onUpward(CMediaSet& currentMediaSet)
{
    m_thrAsyncTask.cancel();

    if (&currentMediaSet == &m_SingerLib || &currentMediaSet == &m_PlaylistLib)
    {
        return NULL;
    }

    return CListViewEx::_onUpward(currentMediaSet);
}

CPath* CMedialibView::_onUpward(CPath& currentDir)
{
    if (NULL == currentDir.parent() && ((CMediaDir&)currentDir).dirType() == E_MediaDirType::MDT_Attach)
    {
        return &__medialib;
    }

    return CListViewEx::_onUpward(currentDir);
}

bool CMedialibView::event(QEvent *ev)
{
    bool bRet = CListView::event(ev);

    if (ev->type() == QEvent::Resize)
    {
        update();
    }

    return bRet;
}

void CMedialibView::_flashRow(UINT uRow, UINT uMSDelay)
{
    m_nFlashingRow = uRow;
    update();

    CApp::async(uMSDelay, [&](){
        m_nFlashingRow = -1;
        update();
    });
}

void CMedialibView::updateSingerImg()
{
    CMediaSet* pMediaSet = currentMediaSet();
    if (pMediaSet)
    {
        if (E_MediaSetType::MST_SingerGroup == pMediaSet->m_eType
            || E_MediaSetType::MST_Playlist == pMediaSet->m_eType)
        {
            update();
        }
    }
}

void CMedialibView::play()
{
    dselectRow();

    TD_IMediaList paMedias;
    CMediaSet *pMediaSet = currentMediaSet();
    if (pMediaSet)
    {
        TD_MediaList lstMedias;
        pMediaSet->GetAllMedias(lstMedias);
        paMedias.add(lstMedias);
    }
    else
    {
        CPath *pDir = currentDir();
        if (pDir)
        {
            pDir->files()([&](XFile& file){
                paMedias.add((CMediaRes&)file);
            });
        }
    }

    if (paMedias)
    {
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(paMedias));
    }
}

void CMedialibView::cleanup()
{
    m_thrAsyncTask.cancel();

    m_mapSingerPixmap.clear();
    m_lstSingerPixmap.clear();

    CListViewEx::_cleanup();
}
