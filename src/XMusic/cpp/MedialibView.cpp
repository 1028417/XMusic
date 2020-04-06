
#include "app.h"

#include "MedialibView.h"

#define __XMusicDirName L"XMusic"
#define __OuterDirName  L"  本地"

#define __FlashingAlpha 170

#define __RemarkAlpha 170

#define __wholeTrackDuration 600

CMedialibView::CMedialibView(class CApp& app, CMedialibDlg& medialibDlg, CMediaDir &OuterDir) :
    CListViewEx(&medialibDlg)
    , m_app(app)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(app.getSingerMgr())
    , m_PlaylistLib(app.getPlaylistMgr())
    , m_OuterDir(OuterDir)
{
}

#define __mediaPng(f) ":/img/medialib/" #f ".png"

void CMedialibView::init()
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

    (void)m_pmHDDisk.load(__mediaPng(hddisk));
    (void)m_pmLLDisk.load(__mediaPng(lldisk));

    (void)m_pmDir.load(__mediaPng(dir));
    (void)m_pmDirLink.load(__mediaPng(dirLink));
    (void)m_pmFile.load(__mediaPng(file));

    (void)m_pmPlay.load(":/img/btnPlay.png");
    m_pmPlayOpacity = CPainter::alphaPixmap(m_pmPlay, 128);
    (void)m_pmAddPlay.load(":/img/btnAddplay.png");
    m_pmAddPlayOpacity = CPainter::alphaPixmap(m_pmAddPlay, 128);

    connect(this, &CMedialibView::signal_update, this, [&](){
        update();
    });
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
        strTitle << strutil::trim_r(wstring(__OuterDirName));
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

size_t CMedialibView::getPageRowCount()
{
    if (isInRoot())
    {
        return _getRootRowCount();
    }

    return m_medialibDlg.getPageRowCount();
}

size_t CMedialibView::getColumnCount()
{
    if (isInRoot() && isHLayout())
    {
        return 2;
    }

    return 1;
}

size_t CMedialibView::_getRootRowCount()
{
    if (isHLayout())
    {
        return 6;
    }
    else
    {
        return 10;
    }
}

bool CMedialibView::_genRootRowContext(tagMediaContext& context)
{
    cauto lvRow = context.lvRow;

    context.eStyle = E_RowStyle::IS_CenterAlign;

    bool bHScreen = isHLayout();
    if (bHScreen)
    {
        context.fIconMargin /= 1.3;
    }

    if ((bHScreen && 1 == lvRow.uRow && 0 == lvRow.uCol) || (!bHScreen && 1 == lvRow.uRow))
    {
        context.pixmap = &m_pmSingerGroup;
        context.strText = L"  歌手";
        context.pMediaSet = &m_SingerLib;
        return true;
    }
    else if ((bHScreen && 1 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 3 == lvRow.uRow))
    {
        context.pixmap = &m_pmPlaylistSet;
        context.strText = L"  歌单";
        context.pMediaSet = &m_PlaylistLib;
        return true;
    }
    else if ((bHScreen && 3 == lvRow.uRow && 0 == lvRow.uCol) || (!bHScreen && 5 == lvRow.uRow))
    {
        context.pixmap = &m_pmXmusicDir;
        context.strText = __XMusicDirName;
        context.pDir = &__medialib;
        return true;
    }
    else if ((bHScreen && 3 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 7 == lvRow.uRow))
    {
        context.pixmap = &m_pmDir;
        context.strText = __OuterDirName;
        context.pDir = &m_OuterDir;
        return true;
    }

    return false;
}

const QPixmap& CMedialibView::_getSingerPixmap(UINT uSingerID, const wstring& strSingerName)
{
    auto& pSingerPixmap = m_mapSingerPixmap[uSingerID];
    if (pSingerPixmap)
    {
        return *pSingerPixmap;
    }

    cauto strSingerImg = m_app.getSingerImgMgr().getSingerImg(strSingerName, 0);
    if (!strSingerImg.empty())
    {
        m_lstSingerPixmap.emplace_back(QPixmap());
        QPixmap& pm = m_lstSingerPixmap.back();
        if (pm.load(strutil::toQstr(strSingerImg)))
        {
#define __singerimgZoomout 160
            CPainter::zoomoutPixmap(pm, __singerimgZoomout);

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

void CMedialibView::_genMediaContext(tagMediaContext& context)
{
    context.eStyle |= E_RowStyle::IS_MultiLine;
    if (context.pMediaSet)
    {
        switch (context.pMediaSet->m_eType)
        {
        case E_MediaSetType::MST_Playlist:
            context.pixmap = &m_pmPlaylist;
            break;
        case E_MediaSetType::MST_Album:
            context.pixmap = &m_pmAlbum;
            break;
        case E_MediaSetType::MST_Singer:
            context.pixmap = &_getSingerPixmap(context.pMediaSet->m_uID,
                                               context.pMediaSet->m_strName);
            break;
        case E_MediaSetType::MST_SingerGroup:
            context.pixmap = &m_pmSingerGroup;

            context.eStyle |= E_RowStyle::IS_RightTip;
        default:
            break;
        };
    }
    else if (context.pMedia)
    {
        if (context.pMedia->GetMediaSetType() == E_MediaSetType::MST_Playlist)
        {
            context.pixmap = &m_pmPlayItem;

            UINT uSingerID = context.pMedia->GetRelatedMediaSetID(E_MediaSetType::MST_Singer);
            if (uSingerID > 0)
            {
                cauto strSingerName = context.pMedia->GetRelatedMediaSetName(E_MediaSetType::MST_Singer);
                if (!strSingerName.empty())
                {
                    context.pixmap = &_getSingerPixmap(uSingerID, strSingerName);
                }
            }
        }
        //else context.pixmap = &m_pmAlbumItem;
    }
    else if (context.pDir)
    {
        if (context.pDir->rootDir() == &__medialib)
        {
            context.pixmap = &m_pmSSDir;

            if (context.pDir->parent() == &__medialib)
            {
                auto strDirName = context.pDir->fileName();
                strutil::lowerCase(strDirName);
                if (strDirName.find(L"hires") != __wnpos)
                {
                    context.pixmap = &m_pmHires;
                }
                else if (strDirName.find(L"dsd") != __wnpos)
                {
                    context.pixmap = &m_pmDSD;
                }
                /*else if (strDirName.find(L"整轨") != __wnpos)
                {
                    context.pixmap = &m_pmDiskdir;
                }*/
            }
        }
        else
        {
            context.pixmap = &m_pmDir;

            if (context.pDir->parent() == NULL)
            {
                CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(context.pDir);
                if (pAttachDir)
                {
                    context.pixmap = &m_pmDirLink;

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
            context.pixmap = &m_pmSSFile;
            context.strText = pMediaRes->GetTitle();
        }
        else
        {
            context.pixmap = &m_pmFile;
            context.strText = pMediaRes->fileName();
        }
    }

    IMedia* pMedia = ((tagMediaContext&)context).media();
    if (pMedia)
    {
        if (pMedia->duration() > __wholeTrackDuration)
        {
            if (pMedia->quality() >= E_MediaQuality::MQ_CD)
            {
                context.pixmap = &m_pmHDDisk;
            }
            else
            {
                context.pixmap = &m_pmLLDisk;
            }
        }
    }
}

inline bool CMedialibView::_playIconRect(const tagMediaContext& context, QRect& rcPlayIcon)
{
    if ((context.pMediaSet && (E_MediaSetType::MST_Playlist == context.pMediaSet->m_eType
         || E_MediaSetType::MST_Album == context.pMediaSet->m_eType
         || E_MediaSetType::MST_Singer == context.pMediaSet->m_eType))
        || context.pMedia || context.pFile)
    {
        UINT cy = context.lvRow.rc.height();
        int yMargin = cy * context.fIconMargin*1.3;
        cy -= yMargin*2;

        int x_icon = context.lvRow.rc.right()- __playIconMagin - cy;
        int y_icon = context.lvRow.rc.top()+yMargin;
        rcPlayIcon.setRect(x_icon, y_icon, cy, cy);

        return true;
    }

    return false;
}

void CMedialibView::_paintText(CPainter& painter, QRect& rc, const tagRowContext& context, const QString& qsText, int flags)
{
    cauto mediaContext = (tagMediaContext&)context;

    QRect rcPlayIcon;
    if (_playIconRect(mediaContext, rcPlayIcon))
    {
        bool bFlashing = (int)mediaContext.lvRow.uRow == m_nFlashingRow;
        cauto pixmap = mediaContext.pMediaSet
                ?(bFlashing?m_pmPlayOpacity:m_pmPlay)
               :(bFlashing?m_pmAddPlayOpacity:m_pmAddPlay);
        painter.drawPixmap(rcPlayIcon, pixmap);

        rc.setRight(rcPlayIcon.left() - __playIconMagin);
    }

    WString strMediaQuality;
    WString strRemark;
    IMedia* pMedia = mediaContext.media();
    if (pMedia)
    {
        strMediaQuality = pMedia->qualityString();

        if (pMedia->duration() > __wholeTrackDuration)
        {
            strRemark << '\n' << pMedia->durationString();
        }

        if (pMedia->GetMediaSetType() == E_MediaSetType::MST_Album)
        {
            rc.setLeft(rc.left() + __size(10));
        }
    }
    else if (mediaContext.pMediaSet)
    {
        if (E_MediaSetType::MST_SingerGroup == mediaContext.pMediaSet->m_eType)
        {
            auto pSingerGroup = (CSingerGroup*)mediaContext.pMediaSet;
            size_t uAlbumCount = 0;
            cauto lstSingers = pSingerGroup->singers();
            for (cauto singer : lstSingers)
            {
                uAlbumCount += singer.albums().size();
            }
            strRemark << lstSingers.size() << L" 歌手\n" << uAlbumCount << L" 专辑";
        }
        else if (E_MediaSetType::MST_Singer == mediaContext.pMediaSet->m_eType)
        {
            cauto lstAlbum = ((CSinger*)mediaContext.pMediaSet)->albums();
            size_t uAlbumItemCount = 0;
            for (cauto album : lstAlbum)
            {
                uAlbumItemCount += album.albumItems().size();
            }
            strRemark << lstAlbum.size() << L" 专辑\n" << uAlbumItemCount << L" 曲目";
        }
        else if (E_MediaSetType::MST_Album == mediaContext.pMediaSet->m_eType)
        {
            auto pAlbum = (CAlbum*)mediaContext.pMediaSet;
            strRemark << pAlbum->albumItems().size() << L" 曲目";
        }
        else if (E_MediaSetType::MST_Playlist == mediaContext.pMediaSet->m_eType)
        {
            auto pPlaylist = (CPlaylist*)mediaContext.pMediaSet;
            strRemark << pPlaylist->size() << L" 曲目";
        }
    }
    if (!strRemark->empty())
    {
        CPainterFontGuard fontGuard(painter, 0.8, QFont::Weight::ExtraLight);

        UINT uAlpha = CPainter::oppTextAlpha(__RemarkAlpha);
        painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter
                           , strRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);

        rc.setRight(rc.right() - __size(100));
    }
    else
    {
        if (!strMediaQuality->empty())
        {
            rc.setRight(rc.right() - __size(20) - __size(10)*strMediaQuality->size());
        }
    }

    UINT uTextAlpha = 255;
    UINT uShadowAlpha = __ShadowAlpha;
    if ((int)mediaContext.lvRow.uRow == m_nFlashingRow)
    {
        uTextAlpha = __FlashingAlpha;
        uShadowAlpha = uShadowAlpha*__FlashingAlpha/300;
    }

    auto rcPos = painter.drawTextEx(rc, flags, qsText, 1, uShadowAlpha, uTextAlpha);

    if (!strMediaQuality->empty())
    {
        CPainterFontGuard fontGuard(painter, 0.7, QFont::Weight::Thin);

        rcPos.setLeft(rcPos.right() + __size(20));
        rcPos.setTop(rcPos.top() - __size(9));
        rcPos.setRight(10000);
        painter.drawTextEx(rcPos, Qt::AlignLeft|Qt::AlignTop, strMediaQuality, 1, __ShadowAlpha, uTextAlpha);
    }
}

void CMedialibView::_onRowClick(tagLVRow& lvRow, const QMouseEvent& me, CMediaSet& mediaSet)
{
    QRect rcPlayIcon;
    if (_playIconRect(tagMediaContext(lvRow, mediaSet), rcPlayIcon))
    {
        if (me.x() >= rcPlayIcon.left()- __playIconMagin)
        {
            _flashRow(lvRow.uRow);

            TD_MediaList lstMedias;
            mediaSet.GetAllMedias(lstMedias);
            m_app.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(lstMedias)));

            return;
        }
    }

    CListViewEx::_onRowClick(lvRow, me, mediaSet);
}

void CMedialibView::_onMediaClick(tagLVRow& lvRow, const QMouseEvent& me, IMedia& media)
{
    QRect rcPlayIcon;
    if (_playIconRect(tagMediaContext(lvRow, media), rcPlayIcon))
    {
        if (me.x() >= rcPlayIcon.left()- __playIconMagin)
        {
            _flashRow(lvRow.uRow);

            bool bPlay = false;
            if (m_app.getPlayMgr().playStatus() != E_PlayStatus::PS_Play)
            {
                bPlay = true;
            }
            m_app.getCtrl().callPlayCtrl(tagPlayCtrl(media, bPlay));

            return;
        }
    }

    selectRow(lvRow.uRow);
    _flashRow(lvRow.uRow);

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

void CMedialibView::clear()
{
    m_thrAsyncTask.cancel();

    m_mapSingerPixmap.clear();
    m_lstSingerPixmap.clear();

    CListViewEx::_clear();
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

    __appAsync(uMSDelay, [&](){
        m_nFlashingRow = -1;
        update();
    });
}
