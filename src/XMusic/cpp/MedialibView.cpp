
#include "app.h"

#include "MedialibView.h"

#define __XMusicDirName L"XMusic"
#define __OuterDirName L" 本地"

#define _showButton(context, bMedia)

#if 0
void CMedialibView::_showButton(tagMediaContext& context, bool bMedia)
{
    return;

    context.eStyle |= E_RowStyle::IS_RightButton;

    static map<UINT, class CButton*> m_mapButton;
    static map<class CButton*, UINT> m_mapButtonIdx;

    CButton*& pButton = m_mapButton[context.lvRow.uIdx];
    if (NULL == pButton)
    {
        pButton = new CButton(this);
        connect(pButton, &CButton::signal_clicked, [&, pButton]() {
            _onButton(m_mapButtonIdx[pButton]);
        });
        pButton->setVisible(true);
    }
    m_mapButtonIdx[pButton] = context.lvRow.uRow;

    pButton->setStyleSheet(bMedia?"border-image: url(:/img/btnAddplay.png);"
                                : "border-image: url(:/img/btnPlay.png);");

    auto& rc = context.lvRow.rc;
    int szButton = ui.btnReturn->height()-__size(8);
    auto margin = (rc.height()-szButton)/2;
    int x = rc.right()-margin-szButton + __xPlayButtonOffset;
    QRect rcPos(x, rc.y()+margin, szButton, szButton);
    pButton->setGeometry(rcPos);
}

void CMedialibView::_onButton(UINT uRow)
{
    CPath *pDir = currentPath();
    if (pDir)
    {
        pDir->get(uRow, [&](XFile& file){
            m_app.getCtrl().callPlayCtrl(tagPlayCtrl((CMediaRes&)file, false));
        });
        return;
    }

    if (currentSubSets().get(uRow, [&](CMediaSet& mediaSet){
         TD_MediaList lstMedias;
         mediaSet.GetAllMedias(lstMedias);
         m_app.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(lstMedias)));
    }))
    {
        return;
    }

    currentSubMedias().get(uRow, [&](CMedia& media){
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(media, false));
    });
}
#endif

CMedialibView::CMedialibView(class CApp& app, CMedialibDlg& medialibDlg, CMediaDir &OuterDir) :
    CListViewEx(&medialibDlg)
    , m_app(app)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(app.getSingerMgr())
    , m_PlaylistLib(app.getPlaylistMgr())
    , m_MediaLib(app.getMediaLib())
    , m_OuterDir(OuterDir)
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
            return;
        default:
            break;
        };

        _showButton(context, false);
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

        _showButton(context, true);
    }
    else if (context.pDir || context.pFile)
    {
        CMediaRes& MediaRes = context.pDir? (CMediaRes&)*context.pDir : (CMediaRes&)*context.pFile;
        if (MediaRes.IsDir())
        {
            context.pixmap = &m_pmDir;

            if (MediaRes.parent() == NULL)
            {
                CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(&MediaRes);
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
        else
        {
           context.pixmap = &m_pmFile;

           context.strText = MediaRes.GetTitle();

           _showButton(context, true);
        }
    }
}

void CMedialibView::_onMediaClick(tagLVRow& lvRow, const QMouseEvent& me, IMedia& media)
{
    flashRow(lvRow.uRow);
    selectRow(lvRow.uRow);

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
