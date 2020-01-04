
#include "app.h"

#include "MedialibView.h"

#define __XMusicDirName L"XMusic"
#define __OuterDirName L" 本地"

#define __FlashingAlpha 170

#define __RemarkAlpha 170

CMedialibView::CMedialibView(class CApp& app, CMedialibDlg& medialibDlg, CMediaDir &OuterDir) :
    CListViewEx(&medialibDlg)
    , m_app(app)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(app.getSingerMgr())
    , m_PlaylistLib(app.getPlaylistMgr())
    , m_MediaLib(app.getMediaLib())
    , m_OuterDir(OuterDir)
    , m_pmPlay(":/img/btnPlay.png")
    , m_pmPlayAlpha(CPainter::alphaPixmap(m_pmPlay, 128))
    , m_pmAddPlay(":/img/btnAddplay.png")
    , m_pmAddPlayAlpha(CPainter::alphaPixmap(m_pmAddPlay, 128))
{
}

void CMedialibView::init()
{
    (void)m_pmSingerGroup.load(":/img/singergroup.png");
    (void)m_pmDefaultSinger.load(":/img/singerdefault.png");
    (void)m_pmAlbum.load(":/img/album.png");
    (void)m_pmAlbumItem.load(":/img/albumitem.png");

    (void)m_pmPlaylist.load(":/img/playlist.png");
    (void)m_pmPlayItem.load(":/img/playitem.png");

    (void)m_pmDir.load(":/img/dir.png");
    (void)m_pmDirLink.load(":/img/dirLink.png");
    (void)m_pmFile.load(":/img/file.png");

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
        CPath *pDir = currentPath();
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
    m_medialibDlg.updateHead(L"媒体库", false, false);
}

static XThread g_thrAsyncTask;

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    WString strTitle;
    _getTitle(MediaSet, strTitle);

    bool bPlayable = E_MediaSetType::MST_Singer==MediaSet.m_eType
            || E_MediaSetType::MST_Album==MediaSet.m_eType
            || E_MediaSetType::MST_Playlist==MediaSet.m_eType;

    m_medialibDlg.updateHead(strTitle, bPlayable, true);

    if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
    {
        g_thrAsyncTask.start([&](const bool& bRunSignal){
            ((CPlaylist&)MediaSet).playItems()([&](CPlayItem& playItem){
                playItem.findRelatedMedia(E_MediaSetType::MST_Album);

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

void CMedialibView::_onShowPath(CPath& path)
{
    if (path.fileInfo().bDir)
    {
        WString strTitle;
        _getTitle((CMediaDir&)path, strTitle);

        bool bPlayable = path.files();
        m_medialibDlg.updateHead(strTitle, bPlayable, true);
    }
}

bool CMedialibView::showFile(const wstring& strFile)
{    
    CMediaRes *pMediaRes = m_OuterDir.findSubFile(strFile);
    if (NULL == pMediaRes)
    {
        pMediaRes = m_MediaLib.findSubFile(strFile);
    }
    if (pMediaRes)
    {
        showPath(*pMediaRes);
        return true;
    }

    cauto strOuterRoot = m_OuterDir.GetAbsPath();
    if (fsutil::CheckSubPath(strOuterRoot, strFile))
    {
        pMediaRes = (CMediaRes*)m_OuterDir.findSubFile(strFile.substr(strOuterRoot.size()));
        if (pMediaRes)
        {
            showPath(*pMediaRes);
            return true;
        }
    }

    return false;
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
            _getTitle(*MediaSet.m_pParent, strTitle);
        }

        strTitle << __CNDot << MediaSet.m_strName;
    }
}

void CMedialibView::_getTitle(CMediaDir& MediaDir, WString& strTitle)
{
    if (&MediaDir == &m_MediaLib)
    {
        strTitle << __XMusicDirName;
    }
    else if (&MediaDir == &m_OuterDir)
    {
        strTitle << strutil::trim_r(__OuterDirName);
    }
    else
    {
        strTitle << MediaDir.GetName();

        /*auto pParent = MediaDir.parent();
        if (NULL == pParent)
        {
            pParent = &m_MediaLib;
        }

        _getTitle(*pParent, strTitle);

        strTitle << __wcDirSeparator << MediaDir.GetName();*/
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
        context.strText = L" 歌手";
        context.pMediaSet = &m_SingerLib;
        return true;
    }
    else if ((bHScreen && 1 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 3 == lvRow.uRow))
    {
        context.pixmap = &m_pmPlaylist;
        context.strText = L" 歌单";
        context.pMediaSet = &m_PlaylistLib;
        return true;
    }
    else if ((bHScreen && 3 == lvRow.uRow && 0 == lvRow.uCol) || (!bHScreen && 5 == lvRow.uRow))
    {
        context.pixmap = &m_pmDir;
        context.strText = __XMusicDirName;
        context.pDir = &m_MediaLib;
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
#define __zoomoutSize 150
            CPainter::zoomoutPixmap(pm, __zoomoutSize);

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
        default:
            break;
        };
    }
    else if (context.pMedia)
    {
        if (context.pMedia->GetMediaSetType() == E_MediaSetType::MST_Album)
        {
            context.pixmap = &m_pmAlbumItem;
        }
        else
        {
            context.pixmap = &m_pmPlayItem;

            if (context.pMedia->GetMediaSetType() == E_MediaSetType::MST_Playlist)
            {
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
        }
    }
    else if (context.pDir)
    {
        CMediaDir *pMediaDir = (CMediaDir*)context.pDir;
        context.pixmap = &m_pmDir;

        if (pMediaDir->parent() == NULL)
        {
            CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(pMediaDir);
            if (pAttachDir)
            {
                context.pixmap = &m_pmDirLink;

                /*if (E_AttachDirType::ADT_TF == pAttachDir->m_eType)
                {
                    context.strRemark = L"扩展";
                }
                else if (E_AttachDirType::ADT_USB == pAttachDir->m_eType)
                {
                    context.strRemark = L"USB";
                }
                else
                {
                    context.strRemark = L"內部";
                }*/
            }
        }
    }
    else if (context.pFile)
    {
       context.pixmap = &m_pmFile;

       context.strText = ((CMediaRes*)context.pFile)->GetTitle();
    }
}

inline bool CMedialibView::_playIconRect(const tagMediaContext& mediaContext, QRect& rcPlayIcon)
{
    if ((mediaContext.pMediaSet && (E_MediaSetType::MST_Playlist == mediaContext.pMediaSet->m_eType
         || E_MediaSetType::MST_Album == mediaContext.pMediaSet->m_eType
         || E_MediaSetType::MST_Singer == mediaContext.pMediaSet->m_eType))
        || mediaContext.pMedia || mediaContext.pFile)
    {
        UINT cy = mediaContext.lvRow.rc.height();
        int margin = cy * mediaContext.fIconMargin*1.5;
        cy -= margin*2;

        int x_icon = mediaContext.lvRow.rc.right()-margin-cy;
        int y_icon = mediaContext.lvRow.rc.top()+margin;
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
        bool bFlashing = (int)context.lvRow.uRow == m_nFlashingRow;
        cauto pixmap = mediaContext.pMediaSet
                ?(bFlashing?m_pmPlayAlpha:m_pmPlay)
               :(bFlashing?m_pmAddPlayAlpha:m_pmAddPlay);
        painter.drawPixmap(rcPlayIcon, pixmap);

        rc.setRight(rc.right()-rc.height());
    }

    if (mediaContext.pMediaSet)
    {
        QString qsRemark;
        if (E_MediaSetType::MST_SingerGroup == mediaContext.pMediaSet->m_eType)
        {
            auto pSingerGroup = (CSingerGroup*)mediaContext.pMediaSet;
            qsRemark.sprintf("%d歌手", pSingerGroup->singers().size());
        }
        else if (E_MediaSetType::MST_Singer == mediaContext.pMediaSet->m_eType)
        {
            auto pSinger = (CSinger*)mediaContext.pMediaSet;
            qsRemark.sprintf("%d专辑", pSinger->albums().size());
        }
        else if (E_MediaSetType::MST_Album == mediaContext.pMediaSet->m_eType)
        {
            auto pAlbum = (CAlbum*)mediaContext.pMediaSet;
            qsRemark.sprintf("%d曲目", pAlbum->albumItems().size());
        }
        else if (E_MediaSetType::MST_Playlist == mediaContext.pMediaSet->m_eType)
        {
            auto pPlaylist = (CPlaylist*)mediaContext.pMediaSet;
            qsRemark.sprintf("%d曲目", pPlaylist->playItems().size());
        }

        if (!qsRemark.isEmpty())
        {
            painter.save();

            painter.adjustFont(0.9f);

            UINT uAlpha = oppTextAlpha(__RemarkAlpha);
            painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter
                               , qsRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);

            painter.restore();

            rc.setRight(rc.right() - 100);
        }
    }

    UINT uTextAlpha = 255;
    UINT uShadowAlpha = __ShadowAlpha;
    if ((int)context.lvRow.uRow == m_nFlashingRow)
    {
        uTextAlpha = __FlashingAlpha;
        uShadowAlpha = uShadowAlpha*__FlashingAlpha/300;
    }

    painter.drawTextEx(rc, flags, qsText, 1, uShadowAlpha, uTextAlpha);
}

void CMedialibView::_onRowClick(tagLVRow& lvRow, const QMouseEvent& me, CMediaSet& mediaSet)
{
    QRect rcPlayIcon;
    if (_playIconRect(tagMediaContext(lvRow, mediaSet), rcPlayIcon))
    {
        if (rcPlayIcon.contains(me.x(), me.y()))
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
        if (rcPlayIcon.contains(me.x(), me.y()))
        {
            _flashRow(lvRow.uRow);

            m_app.getCtrl().callPlayCtrl(tagPlayCtrl(media, false));

             return;
        }
    }

    selectRow(lvRow.uRow);
    _flashRow(lvRow.uRow);

    if (me.x() <= (int)rowHeight())
    {
        if (media.GetMediaSetType() == E_MediaSetType::MST_Playlist)
        {
            UINT uSingerID = media.GetRelatedMediaSetID(E_MediaSetType::MST_Singer);
            if (uSingerID > 0)
            {
                auto pSinger = m_SingerLib.FindSubSet(E_MediaSetType::MST_Singer, uSingerID);
                if (pSinger)
                {
                    showMediaSet(*pSinger);
                    return;
                }
            }
        }
    }

    m_app.getCtrl().callPlayCtrl(tagPlayCtrl(media, true));
}

CMediaSet* CMedialibView::_onUpward(CMediaSet& currentMediaSet)
{
    g_thrAsyncTask.cancel();

    if (&currentMediaSet == &m_SingerLib || &currentMediaSet == &m_PlaylistLib)
    {
        return NULL;
    }

    return CListViewEx::_onUpward(currentMediaSet);
}

CPath* CMedialibView::_onUpward(CPath& currentDir)
{
    if (NULL == currentDir.fileInfo().pParent && dynamic_cast<CAttachDir*>(&currentDir))
    {
        return &m_MediaLib;
    }

    return CListViewEx::_onUpward(currentDir);
}

void CMedialibView::clear()
{
    g_thrAsyncTask.cancel();

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

    CApp::async(uMSDelay, [&](){
        m_nFlashingRow = -1;
        update();
    });
}
