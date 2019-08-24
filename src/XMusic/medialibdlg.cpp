
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

#define __XMusic L"XMusic"
#define __InnerStorage L"内部存储"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(class CPlayerView& view) :
    m_view(view)
    , m_MedialibView(view, *this)
{
}

void CMedialibDlg::init()
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(2, E_FontWeight::FW_SemiBold);

    cauto& crBkg = bkgColor();
    cauto& crFlashText = CPainter::mixColor(crText, crBkg, 85);
    m_MedialibView.setTextColor(crText, crFlashText);

    QColor crSelectedBkg = crBkg;
    crSelectedBkg.setRed(crBkg.red()-10);
    crSelectedBkg.setGreen(crBkg.green()-5);
    m_MedialibView.setSelectedBkgColor(crSelectedBkg);

#if __android
    m_MedialibView.setFont(0.5);
#endif

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
    cauto& rcReturn = ui.btnReturn->geometry();
    int y_margin = rcReturn.top();

    ui.btnPlay->setGeometry(cx-rcReturn.right(), y_margin, rcReturn.width(), rcReturn.height());

    _resizeTitle();

    int y_MedialibView = rcReturn.bottom() + y_margin;
    m_MedialibView.setGeometry(0, y_MedialibView, cx, cy-y_MedialibView);
}

void CMedialibDlg::_resizeTitle() const
{
#define __offset 30

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

void CMedialibDlg::update(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton)
{
    ui.labelTitle->setText(wsutil::toQStr(strTitle));

    ui.btnPlay->setVisible(bShowPlayButton);

#if __android
    bShowUpwardButton = false;
#endif
    ui.btnUpward->setVisible(bShowUpwardButton);

    _resizeTitle();
}

CMedialibView::CMedialibView(class CPlayerView& view, CMedialibDlg& medialibDlg) :
    CListViewEx(&medialibDlg)
    , m_view(view)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(view.getModel().getSingerMgr())
    , m_PlaylistLib(view.getModel().getPlaylistMgr())
    , m_MediaLib(view.getModel().getMediaLib())
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
    if (m_pMediaset)
    {
        TD_MediaList lstMedias;
        m_pMediaset->GetAllMedias(lstMedias);
        if (lstMedias)
        {
            m_view.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(lstMedias)));
            dselectRow();
        }
    }
    else if (m_pPath)
    {
        cauto& paSubFile = m_pPath->files();
        if (paSubFile)
        {
            m_view.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(TD_MediaResList(paSubFile))));
            dselectRow();
        }
    }
}

void CMedialibView::_onShowRoot()
{
    m_medialibDlg.update(L"媒体库", false, false);
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    WString strTitle;
    _getTitle(MediaSet, strTitle);

    bool bPlayable = E_MediaSetType::MST_Singer==MediaSet.m_eType
            || E_MediaSetType::MST_Album==MediaSet.m_eType
            || E_MediaSetType::MST_Playlist==MediaSet.m_eType;

    m_medialibDlg.update(strTitle, bPlayable, true);
}

void CMedialibView::_onShowPath(CPath& path)
{
    if (path.fileInfo().bDir)
    {
        WString strTitle;
        _getTitle((CMediaRes&)path, strTitle);

        bool bPlayable = path.files();
        m_medialibDlg.update(strTitle, bPlayable, true);
    }
}

void CMedialibView::showFile(const wstring& strPath)
{
    CMediaRes *pMediaRes = m_MediaLib.FindSubPath(strPath, false);
    if (pMediaRes)
    {
        showPath(*pMediaRes);
        return;
    }

#if __android
    if (fsutil::CheckSubPath(m_rootDir.GetAbsPath(), strPath))
    {
        pMediaRes = m_rootDir.FindSubPath(strPath.substr(m_rootDir.GetAbsPath().size()), false);
        if (pMediaRes)
        {
            showPath(*pMediaRes);
        }
    }
#endif
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

void CMedialibView::_getTitle(CMediaRes& MediaRes, WString& strTitle)
{
    if (&MediaRes == &m_MediaLib)
    {
        strTitle << __XMusic;
        return;
    }
    else if (&MediaRes == &m_rootDir)
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

UINT CMedialibView::getPageRowCount()
{
    if (isInRoot())
    {
        return getRootCount();
    }

    UINT uRet = 10;
    int cy = m_medialibDlg.height();
    if (cy > 2160)
    {
       uRet++;

        if (cy > 2340)
        {
           uRet++;
        }
    }
    else if (cy < 1800)
    {
        uRet--;
        uRet = ceil((float)uRet*m_medialibDlg.height()/1800);
    }

    return uRet;
}

UINT CMedialibView::getColumnCount()
{
    if (isInRoot() && isHLayout())
    {
        return 2;
    }

    return 1;
}

UINT CMedialibView::getRootCount()
{
#if __android
    return isHLayout()?5:9;
#else
    return isHLayout()?5:7;
#endif
}

bool CMedialibView::_genRootRowContext(const tagLVRow& lvRow, tagMediaContext& context)
{
    context.eStyle = E_RowStyle::IS_None;

    bool bHScreen = isHLayout();
    if ((bHScreen && 1 == lvRow.uRow && 0 == lvRow.uCol) || (!bHScreen && 1 == lvRow.uRow))
    {
        context.pixmap = &m_pmSingerGroup;
        context.strText = m_SingerLib.m_strName;
        context.pMediaSet = &m_SingerLib;
    }
    else if ((bHScreen && 1 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 3 == lvRow.uRow))
    {
        context.pixmap = &m_pmPlaylist;
        context.strText = m_PlaylistLib.m_strName;
        context.pMediaSet = &m_PlaylistLib;
    }
    else if ((bHScreen && 3 == lvRow.uRow && 0 == lvRow.uCol) || (!bHScreen && 5 == lvRow.uRow))
    {
        context.pixmap = &m_pmDir;
        context.strText = __XMusic;
        context.pPath = &m_MediaLib;
    }
#if __android
    else if ((bHScreen && 3 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 7 == lvRow.uRow))
    {
        context.pixmap = &m_pmDir;
        context.strText = __InnerStorage;
        context.pPath = &m_rootDir;
    }
#endif
    else
    {
        return false;
    }

    return true;
}

const QPixmap& CMedialibView::_getSingerPixmap(CSinger& Singer)
{
    auto itr = m_mapSingerPixmap.find(&Singer);
    if (itr != m_mapSingerPixmap.end())
    {
        return *itr->second;
    }

    wstring strSingerImg;
    if (m_view.getModel().getSingerImgMgr().getSingerImg(Singer.m_strName, 0, strSingerImg))
    {
        m_lstSingerPixmap.push_back(QPixmap());
        auto& pm = m_lstSingerPixmap.back();
        pm.load(wsutil::toQStr(strSingerImg));

        m_mapSingerPixmap[&Singer] = &pm;

        return pm;
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

    m_view.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(media)));
}

bool CMedialibView::_onUpward()
{
    /*if (&m_SingerLib == m_pMediaset || &m_PlaylistLib == m_pMediaset
            || &m_MediaLib == m_pMediaset || &m_rootDir == m_pMediaset)
    {
        showRoot();
        return true;
    }*/

    if (m_pPath && NULL == m_pPath->fileInfo().pParent && dynamic_cast<CAttachDir*>(m_pPath) != NULL)
    {
        showPath(m_MediaLib);
        return true;
    }

    return CListViewEx::_onUpward();
}
