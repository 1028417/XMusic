
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

#define __XMusicDirName L"XMusic"
#define __OuterDirName L"根目录"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(class CXMusicApp& app) : CDialog(app.mainWnd()),
    m_app(app)
    , m_MedialibView(app, *this, m_OuterDir)
{
}

void CMedialibDlg::init()
{
    ui.setupUi(this);

    QColor crText(__BlueLabel);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

    cauto crBkg = bkgColor();
    cauto crFlashText = CPainter::mixColor(crText, crBkg, 85);
    m_MedialibView.setTextColor(crText, crFlashText);

#if __android || __ios
    m_MedialibView.setFont(1.05);
#endif

    QColor crSelectedBkg = crBkg;
    crSelectedBkg.setRed(crBkg.red()-10);
    crSelectedBkg.setGreen(crBkg.green()-5);
    m_MedialibView.setSelectedBkgColor(crSelectedBkg);
    m_MedialibView.init();


    cauto strMediaLibDir = m_app.getModel().getMediaLib().GetAbsPath();
    wstring strOuterDir;
#if __windows
    for (cauto wch : strMediaLibDir)
    {
        if (fsutil::checkPathTail(wch))
        {
            strOuterDir.append(L"/..");
        }
    }

#else
    strOuterDir = L"/..";
#endif
    m_OuterDir.setDir(strMediaLibDir, strOuterDir);


    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_MedialibView.upward();
    });

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_MedialibView.play();
    });
}

void CMedialibDlg::_show()
{
    __async(0, [&](){
        _resizeTitle();
    });

    CDialog::show(true);
}

void CMedialibDlg::_relayout(int cx, int cy)
{
    static const QRect rcReturnPrev = ui.btnReturn->geometry();
    QRect rcReturn = __rect(rcReturnPrev);
    if (cy >= __size(812*3)) // 针对全面屏刘海作偏移
    {
#define __yOffset __size(66)
        rcReturn.setTop(rcReturn.top() + __yOffset);
        rcReturn.setBottom(rcReturn.bottom() + __yOffset);
    }
    ui.btnReturn->setGeometry(rcReturn);

    int y_margin = rcReturn.top();

    int y_MedialibView = rcReturn.bottom() + y_margin;
    m_MedialibView.setGeometry(0, y_MedialibView, cx, cy-y_MedialibView);

    static const int x_btnUpward = __size(ui.btnUpward->x());
    ui.btnUpward->setGeometry(x_btnUpward, y_margin, rcReturn.width(), rcReturn.height());

    ui.btnPlay->setGeometry(cx-rcReturn.right(), y_margin, rcReturn.width(), rcReturn.height());

    ui.labelTitle->move(ui.labelTitle->x(), rcReturn.center().y() - ui.labelTitle->height()/2);
    _resizeTitle();
}

void CMedialibDlg::_resizeTitle() const
{
#define __offset __size(30)

    auto pButton = ui.btnUpward->isVisible() ? ui.btnUpward : ui.btnReturn;
    int x_title = pButton->geometry().right() + __offset;

    int cx_title = 0;
    if (ui.btnPlay->isVisible())
    {
        cx_title = ui.btnPlay->x() - __offset - x_title;
    }
    else
    {
        cx_title = width() - __offset - x_title;
    }

    ui.labelTitle->setGeometry(x_title, ui.labelTitle->y(), cx_title, ui.labelTitle->height());
}

void CMedialibDlg::updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton)
{
    ui.labelTitle->setText(strutil::toQstr(strTitle));

    ui.btnPlay->setVisible(bShowPlayButton);

/*#if __android// || __ios
    bShowUpwardButton = false;
#endif*/
    ui.btnUpward->setVisible(bShowUpwardButton);

    _resizeTitle();
}

void CMedialibDlg::_onClose()
{
    m_MedialibView.clear();

    m_OuterDir.clear();
}

CMedialibView::CMedialibView(class CXMusicApp& app, CMedialibDlg& medialibDlg, CMediaDir &OuterDir) :
    CListViewEx(&medialibDlg)
    , m_app(app)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(app.getModel().getSingerMgr())
    , m_PlaylistLib(app.getModel().getPlaylistMgr())
    , m_MediaLib(app.getModel().getMediaLib())
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

    SArray<wstring> arrOppPaths = CListViewEx::currentSubFiles().map([&](const XFile& subFile) {
        return ((const CMediaRes&)subFile).GetPath();
    });
    if (!arrOppPaths)
    {
        CMediaSet *pMediaSet = CListViewEx::currentMediaSet();
        if (pMediaSet)
        {
            TD_MediaList lstMedias;
            pMediaSet->GetAllMedias(lstMedias);

            arrOppPaths = lstMedias.map([](const IMedia& Media) {
                return Media.GetPath();
            });
        }
    }
    if (arrOppPaths)
    {
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(arrOppPaths));
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

#define __Dot L"·"

void CMedialibView::_getTitle(CMediaSet& MediaSet, WString& strTitle)
{
    if (&MediaSet == &m_SingerLib || &MediaSet == &m_PlaylistLib)
    {
        strTitle << MediaSet.m_strName;
        return;
    }

    if (MediaSet.m_pParent)
    {
        _getTitle(*MediaSet.m_pParent, strTitle);
    }

    strTitle << __Dot << MediaSet.m_strName;
}

void CMedialibView::_getTitle(CMediaDir& MediaRes, WString& strTitle)
{
    if (&MediaRes == &m_MediaLib)
    {
        strTitle << __XMusicDirName;
        return;
    }
    else if (&MediaRes == &m_OuterDir)
    {
        strTitle << __OuterDirName;
        return;
    }

    auto pParent = MediaRes.parent();
    if (NULL == pParent)
    {
        pParent = &m_MediaLib;
    }

    _getTitle(*pParent, strTitle);

    strTitle << __wcDirSeparator << MediaRes.GetName();
}

size_t CMedialibView::getPageRowCount()
{
    if (isInRoot())
    {
        return _getRootRowCount();
    }

    UINT uRet = 10;
    int cy = m_medialibDlg.height();
    /*if (cy >= __size(2560))
    {
       uRet++;
    }
    else*/ if (cy < __size(1800))
    {
        uRet--;
        uRet = ceil((float)uRet*m_medialibDlg.height()/__size(1800));
    }

    return uRet;
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

bool CMedialibView::_genRootRowContext(const tagLVRow& lvRow, tagMediaContext& context)
{
    context.eStyle = E_RowStyle::IS_CenterAlign;

    bool bHScreen = isHLayout();
    if (bHScreen)
    {
        context.fIconMargin /= 1.3;
    }

    if ((bHScreen && 1 == lvRow.uRow && 0 == lvRow.uCol) || (!bHScreen && 1 == lvRow.uRow))
    {
        context.pixmap = &m_pmSingerGroup;
        context.strText = m_SingerLib.m_strName;
        context.pMediaSet = &m_SingerLib;
        return true;
    }
    else if ((bHScreen && 1 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 3 == lvRow.uRow))
    {
        context.pixmap = &m_pmPlaylist;
        context.strText = m_PlaylistLib.m_strName;
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

    cauto strSingerImg = m_app.getModel().getSingerImgMgr().getSingerImg(strSingerName, 0);
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
            break;
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

                    if (E_AttachDirType::ADT_TF == pAttachDir->m_eType)
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
                    }
                }
            }
        }
        else
        {
           context.pixmap = &m_pmFile;

           context.strText = MediaRes.GetTitle();
        }
    }
}

void CMedialibView::_onMediaClick(const tagLVRow& lvRow, const QMouseEvent& me, IMedia& media)
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

    m_app.getCtrl().callPlayCtrl(tagPlayCtrl(SArray<wstring>(media.GetPath())));
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
