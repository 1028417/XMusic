
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

#define __XMusic L"XMusic"
#define __InnerStorage L"内部存储"

CMediaDir CAndroidDir::m_root;

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(class CXMusicApp& app)
    : m_app(app)
    , m_MedialibView(app, *this, m_pOuterDir)
{
}

void CMedialibDlg::init()
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

    cauto& crBkg = bkgColor();
    cauto& crFlashText = CPainter::mixColor(crText, crBkg, 85);
    m_MedialibView.setTextColor(crText, crFlashText);

#if __android || __ios
    m_MedialibView.setFont(1.05);
#endif

    QColor crSelectedBkg = crBkg;
    crSelectedBkg.setRed(crBkg.red()-10);
    crSelectedBkg.setGreen(crBkg.green()-5);
    m_MedialibView.setSelectedBkgColor(crSelectedBkg);

    m_MedialibView.init();

    connect(ui.btnReturn, &CButton::signal_clicked, [&](){
        this->close();
    });

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_MedialibView.upward();
    });

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_MedialibView.play();
    });
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
    ui.labelTitle->setText(strutil::wstrToQStr(strTitle));

    ui.btnPlay->setVisible(bShowPlayButton);

#if __android// || __ios
    bShowUpwardButton = false;
#endif
    ui.btnUpward->setVisible(bShowUpwardButton);

    _resizeTitle();
}

CMedialibView::CMedialibView(class CXMusicApp& app, CMedialibDlg& medialibDlg, CMediaDir* pOuterDir) :
    CListViewEx(&medialibDlg)
    , m_app(app)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(app.getModel().getSingerMgr())
    , m_PlaylistLib(app.getModel().getPlaylistMgr())
    , m_MediaLib(app.getModel().getMediaLib())
    , m_pOuterDir(pOuterDir)
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
}

void CMedialibView::play()
{
    SArray<wstring> arrOppPaths;
    if (m_pMediaset)
    {
        TD_MediaList lstMedias;
        m_pMediaset->GetAllMedias(lstMedias);
        arrOppPaths = lstMedias.map([](IMedia& Media) {
            return Media.GetPath();
        });
    }
    else if (m_pPath)
    {
        arrOppPaths = m_pPath->files().map([&](XFile& subFile) {
            return ((CMediaRes&)subFile).GetPath();
        });
    }

    if (arrOppPaths)
    {
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(arrOppPaths));
        dselectRow();
    }
}

void CMedialibView::_onShowRoot()
{
    m_medialibDlg.updateHead(L"媒体库", false, false);
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    WString strTitle;
    _getTitle(MediaSet, strTitle);

    bool bPlayable = E_MediaSetType::MST_Singer==MediaSet.m_eType
            || E_MediaSetType::MST_Album==MediaSet.m_eType
            || E_MediaSetType::MST_Playlist==MediaSet.m_eType;

    m_medialibDlg.updateHead(strTitle, bPlayable, true);
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

void CMedialibView::showFile(const wstring& strFile)
{
    CMediaRes *pMediaRes = (CMediaRes*)m_MediaLib.FindSubFile(strFile);
    if (pMediaRes)
    {
        showPath(*pMediaRes);
        return;
    }

    if (m_pOuterDir)
    {
        cauto& strOuterRoot = m_pOuterDir->GetAbsPath();
        if (fsutil::CheckSubPath(strOuterRoot, strFile))
        {
            pMediaRes = (CMediaRes*)m_pOuterDir->FindSubPath(strFile.substr(strOuterRoot.size()), false);
            if (pMediaRes)
            {
                showPath(*pMediaRes);
            }
        }
    }
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
        strTitle << __XMusic;
        return;
    }
    else if (&MediaRes == m_pOuterDir)
    {
        strTitle << __InnerStorage;
        return;
    }

    auto pParent = MediaRes.parent();
    if (NULL == pParent)
    {
        pParent = &m_MediaLib;
    }

    _getTitle(*pParent, strTitle);

    strTitle << __wcFSSlant << MediaRes.GetName();
}

size_t CMedialibView::getPageRowCount()
{
    if (isInRoot())
    {
        return getRootCount();
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

size_t CMedialibView::getRootCount()
{
    if (isHLayout())
    {
        return 6;
    }
    else
    {
        if (m_pOuterDir)
        {
            return 10;
        }
        else
        {
            return 8;

        }
    }
}

bool CMedialibView::_genRootRowContext(const tagLVRow& lvRow, tagMediaContext& context)
{
    context.eStyle = E_RowStyle::IS_None;

    bool bHScreen = isHLayout();
    if (bHScreen)
    {
        context.fIconMargin = 0.05f;
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
        context.strText = __XMusic;
        context.pPath = &m_MediaLib;
        return true;
    }
    else if ((bHScreen && 3 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 7 == lvRow.uRow))
    {
        if (m_pOuterDir)
        {
            context.pixmap = &m_pmDir;
            context.strText = __InnerStorage;
            context.pPath = m_pOuterDir;
            return true;
        }
    }

    return false;
}

const QPixmap& CMedialibView::_getSingerPixmap(CSinger& Singer)
{
    auto itr = m_mapSingerPixmap.find(&Singer);
    if (itr != m_mapSingerPixmap.end())
    {
        return *itr->second;
    }

    wstring strSingerImg;
    if (m_app.getModel().getSingerImgMgr().getSingerImg(Singer.m_strName, 0, strSingerImg))
    {
        QPixmap pm;
        if (pm.load(strutil::wstrToQStr(strSingerImg)))
        {
#define __zoomoutSize 150
            CPainter::zoomoutPixmap(pm, __zoomoutSize);
        }

        m_lstSingerPixmap.push_back(QPixmap());
        auto& back = m_lstSingerPixmap.back();
        back.swap(pm);

        m_mapSingerPixmap[&Singer] = &back;
        return back;
    }
    else
    {
        m_mapSingerPixmap[&Singer] = &m_pmDefaultSinger;
        return m_pmDefaultSinger;
    }
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
            context.pixmap = &_getSingerPixmap((CSinger&)*context.pMediaSet);
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
        }
    }
    else if (context.pPath)
    {
        CMediaRes& MediaRes = (CMediaRes&)*context.pPath;
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

void CMedialibView::_onMediaClick(const tagLVRow& lvRow, IMedia& media)
{
    flashRow(lvRow.uRow);
    selectRow(lvRow.uRow);

    m_app.getCtrl().callPlayCtrl(tagPlayCtrl(SArray<wstring>(media.GetPath())));
}

bool CMedialibView::_onUpward()
{
    if (m_pPath && NULL == m_pPath->fileInfo().pParent && dynamic_cast<CAttachDir*>(m_pPath) != NULL)
    {
        showPath(m_MediaLib);
        return true;
    }

    return CListViewEx::_onUpward();
}
