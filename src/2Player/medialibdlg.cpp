
#include "view.h"

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

    ui.btnUpward->setVisible(false);

    m_MedialibView.setTextColor(crText);
    m_MedialibView.setFont(0.5);
    m_MedialibView.init();

    connect(ui.btnReturn, &CButton::signal_clicked, [&](){
        this->close();
    });

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_MedialibView.upward();
    });
}

void CMedialibDlg::_relayout(int cx, int cy)
{
    cauto& rcReturn = ui.btnReturn->geometry();
    int y_margin = rcReturn.top();

    QRect rcUpward(cx-rcReturn.right(), y_margin, rcReturn.width(), rcReturn.height());
    ui.btnUpward->setGeometry(rcUpward);

    _resizeTitle();

    int y_MedialibView = rcReturn.bottom() + y_margin;
    m_MedialibView.setGeometry(0, y_MedialibView, cx, cy-y_MedialibView);
}

void CMedialibDlg::_resizeTitle() const
{
    int cx_title = this->width()-2*ui.labelTitle->x();
    if (!ui.btnUpward->isVisible())
    {
        cx_title += ui.btnUpward->width();
    }
    ui.labelTitle->resize(cx_title, ui.labelTitle->height());
}

void CMedialibDlg::showUpwardButton(bool bVisible) const
{
#if __android
    if (bVisible)
    {
        return;
    }
#endif

    ui.btnUpward->setVisible(bVisible);

    _resizeTitle();
}

void CMedialibDlg::setTitle(const wstring& strTitle) const
{
    ui.labelTitle->setText(wsutil::toQStr(strTitle));
}

bool CMedialibDlg::_handleReturn()
{
    if (m_MedialibView.isInRoot())
    {
        return false;
    }

    m_MedialibView.upward();

    return true;
}

CMedialibView::CMedialibView(class CPlayerView& view, CMedialibDlg& medialibDlg) :
    CListViewEx(&medialibDlg)
    , m_view(view)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(view.getModel().getSingerMgr())
    , m_PlaylistLib(view.getModel().getPlaylistMgr())
    , m_RootMediaRes(view.getModel().getRootMediaRes())
{
}

void CMedialibView::init()
{
    m_sdcard.SetDir(L"/sdcard");

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

void CMedialibView::_onShowRoot()
{
    m_medialibDlg.showUpwardButton(false);

    m_medialibDlg.setTitle(L"媒体库");
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    m_medialibDlg.showUpwardButton(true);

    WString strTitle;
    _getTitle(MediaSet, strTitle);
    m_medialibDlg.setTitle(strTitle);
}

void CMedialibView::_onShowPath(CPath& path)
{
    if (path.IsDir())
    {
        m_medialibDlg.showUpwardButton(true);

        WString strTitle;
        _getTitle((CMediaRes&)path, strTitle);
        m_medialibDlg.setTitle(strTitle);
    }
}

void CMedialibView::showFile(const wstring& strPath)
{
    CMediaRes *pMediaRes = m_RootMediaRes.FindSubPath(strPath, false);
    if (pMediaRes)
    {
        showPath(*pMediaRes);
        return;
    }

#if __android
    if (fsutil::CheckSubPath(m_sdcard.GetPath(), strPath))
    {
        pMediaRes = m_sdcard.FindSubPath(strPath.substr(m_sdcard.GetPath().size()), false);
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
    if (&MediaRes == &m_RootMediaRes)
    {
        strTitle << __XMusic;
        return;
    }
    else if (&MediaRes == &m_sdcard)
    {
        strTitle << __InnerStorage;
        return;
    }

    auto pParent = MediaRes.parent();
    if (NULL == pParent)
    {
        pParent = &m_RootMediaRes;
    }

    _getTitle(*pParent, strTitle);

    strTitle << __wcFSSlant << MediaRes.GetName();
}

UINT CMedialibView::getRootCount()
{
#if __android
    return isHLayout()?5:9;
#else
    return isHLayout()?5:7;
#endif
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
        uRet = round((float)uRet*m_medialibDlg.height()/1800);
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

bool CMedialibView::_genRootRowContext(const tagLVRow& lvRow, tagRowContext& context)
{
    context.eStyle = E_RowStyle::IS_Normal;

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
        context.pPath = &m_RootMediaRes;
    }
#if __android
    else if ((bHScreen && 3 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 7 == lvRow.uRow))
    {
        context.pixmap = &m_pmDir;
        context.strText = __InnerStorage;
        context.pPath = &m_sdcard;
    }
#endif
    else
    {
        return false;
    }

    return true;
}

static list<QPixmap> g_lstSingerPixmap;
static map<CSinger*, QPixmap*> g_mapSingerPixmap;

QPixmap& CMedialibView::_getSingerPixmap(CSinger& Singer)
{
    auto itr = g_mapSingerPixmap.find(&Singer);
    if (itr != g_mapSingerPixmap.end())
    {
        return *itr->second;
    }

    wstring strSingerImg;
    if (m_view.getModel().getSingerImgMgr().getSingerImg(Singer.m_strName, 0, strSingerImg))
    {
        g_lstSingerPixmap.push_back(QPixmap());
        QPixmap& pixmap = g_lstSingerPixmap.back();
        pixmap.load(wsutil::toQStr(strSingerImg));

        g_mapSingerPixmap[&Singer] = &pixmap;
        return pixmap;
    }
    else
    {
        g_mapSingerPixmap[&Singer] = &m_pmDefaultSinger;
        return m_pmDefaultSinger;
    }
}

void CMedialibView::_genRowContext(tagRowContext& context)
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
        if (context.pPath->IsDir())
        {
            context.pixmap = &m_pmDir;

            if (context.pPath->parent() == NULL)
            {
                CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(context.pPath);
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

           context.strText = ((CMediaRes*)context.pPath)->GetTitle();
        }
    }
}

void CMedialibView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, const tagRowContext&)
{
    if (lvRow.bSelect)
    {
        QColor crBkg = m_medialibDlg.bkgColor();
        crBkg.setRed(crBkg.red()-10);
        crBkg.setGreen(crBkg.green()-5);
        painter.fillRect(rc.left(), rc.top(), rc.width(), rc.height()-1, crBkg);
    }

    if (lvRow.bFlash)
    {
        painter.setPen(painter.mixColor(m_crText, m_medialibDlg.bkgColor(), 85));
    }
}

void CMedialibView::_onMediaClick(const tagLVRow& lvRow, IMedia& media)
{
    flashRow(lvRow.uRow);
    selectRow(lvRow.uRow);

    m_view.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(media)));
}

void CMedialibView::upward()
{
    if (&m_SingerLib == m_pMediaset || &m_PlaylistLib == m_pMediaset)
    {
        showRoot();
        return;
    }

    // if (m_pMediaRes && m_pMediaRes->parent() == NULL)
    // {    if (&m_RootMediaRes != m_pMediaRes || &m_sdcard = m_pMediaRes)

    if (dynamic_cast<CAttachDir*>(m_pPath) != NULL)
    {
        showPath(m_RootMediaRes);
        return;
    }

    CListViewEx::upward();

    m_medialibDlg.showUpwardButton(!isInRoot());
}
