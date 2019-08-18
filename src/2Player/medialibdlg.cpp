
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
        (void)m_MedialibView.handleReturn();
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

CListViewEx::CListViewEx(QWidget *parent, UINT uColumnCount, UINT uPageRowCount)
    : CListView(parent, uColumnCount, uPageRowCount)
{
    (void)m_pmRightTip.load(":/img/righttip.png");
}

UINT CListViewEx::getRowCount()
{
    if (m_pMediaset)
    {
          return m_lstSubSets.size() + m_lstSubMedias.size();
    }
    else if (m_pMediaRes)
    {
        return m_pMediaRes->GetSubPath().size();
    }
    else
    {
        return getRootCount();
    }
}

void CListViewEx::showRoot()
{
    m_pMediaRes = NULL;

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    update(0, true);
}

void CListViewEx::showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem)
{
    m_pMediaRes = NULL;

    m_pMediaset = &MediaSet;

    m_lstSubSets.clear();
    m_pMediaset->GetSubSets(m_lstSubSets);

    m_lstSubMedias.clear();
    m_pMediaset->GetMedias(m_lstSubMedias);

    if (pHittestItem)
    {
        int nIdx = pHittestItem->index();
        if (nIdx >= 0)
        {
            showRow((UINT)nIdx, true);
            selectRow((UINT)nIdx);
        }
    }
    else
    {
        update(_scrollRecord(), true);
    }
}

void CListViewEx::showMediaRes(CMediaRes& MediaRes)
{
    CMediaRes *pHittestItem = NULL;
    if (MediaRes.IsDir())
    {
        m_pMediaRes = &MediaRes;
    }
    else
    {
        auto pParent = MediaRes.parent();
        if (NULL == pParent)
        {
            return;
        }

        m_pMediaRes = pParent;
        pHittestItem = &MediaRes;
    }

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    if (pHittestItem)
    {
        int nIdx = m_pMediaRes->GetSubPath().indexOf(pHittestItem);
        if (nIdx >= 0)
        {
            showRow((UINT)nIdx, true);
            selectRow((UINT)nIdx);
        }
    }
    else
    {
        update(_scrollRecord(), true);
    }
}

void CListViewEx::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvRow.uRow, [&](CMediaSet& mediaSet){
                _onPaintRow(painter, rc, lvRow, mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvRow.uRow, [&](CMedia& media) {
                _onPaintRow(painter, rc, lvRow, media);
            });
        }
    }
    else if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(lvRow.uRow, [&](CPath& subPath) {
            _onPaintRow(painter, rc, lvRow, (CMediaRes&)subPath);
        });
    }
    else
    {
        _onPaintRootRow(painter, rc, lvRow);
    }
}

void CListViewEx::_paintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, const tagItemContext& context)
{
    UINT sz_icon = rc.height();
    if (context.uIconSize > 0 && context.uIconSize < sz_icon)
    {
        sz_icon = context.uIconSize;
    }
    else
    {
        sz_icon = sz_icon *65/100;
    }

    int nMargin = (rc.height()-sz_icon)/2;

    int x_icon = 0;
    if (E_ItemStyle::IS_Normal == context.eStyle)
    {
        x_icon = rc.center().x()-sz_icon;

        rc.setLeft(x_icon + sz_icon + nMargin/2);
    }
    else
    {
        x_icon = rc.left() + nMargin;

        rc.setRight(rc.right() - nMargin);
    }

    int y_icon = rc.center().y()-sz_icon/2;
    QRect rcDst(x_icon, y_icon, sz_icon, sz_icon);
    if (context.pixmap && !context.pixmap->isNull())
    {
        painter.drawPixmapEx(rcDst, *context.pixmap);
    }

    rc.setLeft(x_icon + sz_icon + nMargin);

    if (context.eStyle != E_ItemStyle::IS_Normal)
    {
        painter.fillRect(rc.left(), rc.bottom(), rc.width(), 1, QColor(255,255,255,128));

        if (E_ItemStyle::IS_RightTip == context.eStyle)
        {
            int sz_righttip = sz_icon*30/100;
            int x_righttip = rc.right()-sz_righttip;
            int y_righttip = rc.center().y()-sz_righttip/2;

            painter.drawPixmap(x_righttip, y_righttip, sz_righttip, sz_righttip, m_pmRightTip);

            rc.setRight(x_righttip - nMargin);
        }
    }

    QString qsText = painter.fontMetrics(). elidedText(wsutil::toQStr(context.strText)
                            , Qt::ElideRight, rc.width(), Qt::TextShowMnemonic);
    painter.drawText(rc, Qt::AlignLeft|Qt::AlignVCenter, qsText);
}

bool CListViewEx::handleReturn()
{
    _clearScrollRecord();

    if (m_pMediaset)
    {
        if (m_pMediaset->m_pParent)
        {
            showMediaSet(*m_pMediaset->m_pParent);
        }
        else
        {
            showRoot();
        }
    }
    else if (m_pMediaRes)
    {
        auto parent = m_pMediaRes->parent();
        if (NULL != parent)
        {
            showMediaRes(*parent);
        }
        else
        {
            showRoot();
        }
    }
    else
    {
        return false;
    }

    return true;
}

float& CListViewEx::_scrollRecord()
{
    void *p = m_pMediaset;
    if (NULL == p)
    {
        p = m_pMediaRes;
    }

    return m_mapScrollRecord[p];
}

void CListViewEx::_saveScrollRecord()
{
    _scrollRecord() = scrollPos();
}

void CListViewEx::_clearScrollRecord()
{
    if (m_pMediaset)
    {
        m_mapScrollRecord.erase(m_pMediaset);
    }
    else if (m_pMediaRes)
    {
        m_mapScrollRecord.erase(m_pMediaRes);
    }
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

void CMedialibView::showRoot()
{
    CListViewEx::showRoot();

    m_medialibDlg.showUpwardButton(false);

    m_medialibDlg.setTitle(L"媒体库");
}

void CMedialibView::showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem)
{
    CListViewEx::showMediaSet(MediaSet, pHittestItem);

    m_medialibDlg.showUpwardButton(true);

    WString strTitle;
    _getTitle(MediaSet, strTitle);
    m_medialibDlg.setTitle(strTitle);
}

void CMedialibView::showMediaRes(CMediaRes& MediaRes)
{
    CListViewEx::showMediaRes(MediaRes);

    m_medialibDlg.showUpwardButton(true);

    WString strTitle;
    _getTitle(MediaRes, strTitle);
    m_medialibDlg.setTitle(strTitle);
}

void CMedialibView::showMediaRes(const wstring& strPath)
{
    CMediaRes *pMediaRes = m_RootMediaRes.FindSubPath(strPath, false);
    if (pMediaRes)
    {
        showMediaRes(*pMediaRes);
    }
    else
    {
        if (fsutil::CheckSubPath(m_sdcard.GetPath(), strPath))
        {
            pMediaRes = m_sdcard.FindSubPath(strPath.substr(m_sdcard.GetPath().size()), false);
            if (pMediaRes)
            {
                showMediaRes(*pMediaRes);
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

bool CMedialibView::_getRootItemContext(const tagLVRow& lvRow, tagRootItemContext& context)
{
    context.eStyle = E_ItemStyle::IS_Normal;

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
        context.pMediaRes = &m_RootMediaRes;
    }
#if __android
    else if ((bHScreen && 3 == lvRow.uRow && 1 == lvRow.uCol) || (!bHScreen && 7 == lvRow.uRow))
    {
        context.pixmap = &m_pmDir;
        context.strText = __InnerStorage;
        context.pMediaRes = &m_sdcard;
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

void CMedialibView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, CMediaSet& MediaSet)
{
    tagItemContext context;
    context.eStyle = E_ItemStyle::IS_RightTip;
    context.strText = MediaSet.m_strName;

    switch (MediaSet.m_eType)
    {
    case E_MediaSetType::MST_Playlist:
        context.pixmap = &m_pmPlaylist;
        break;
    case E_MediaSetType::MST_Album:
        context.pixmap = &m_pmAlbum;
        break;
    case E_MediaSetType::MST_Singer:
        context.pixmap = &_getSingerPixmap((CSinger&)MediaSet);
        break;
    case E_MediaSetType::MST_SingerGroup:
        context.pixmap = &m_pmSingerGroup;
        break;
    default:
        return;
        break;
    };

    _paintRow(painter, rc, lvRow, context);
}

void CMedialibView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, CMedia& media)
{
    tagItemContext context;
    context.eStyle = E_ItemStyle::IS_Underline;
    if (media.GetMediaSetType() == E_MediaSetType::MST_Album)
    {
        context.pixmap = &m_pmAlbumItem;
    }
    else
    {
        context.pixmap = &m_pmPlayItem;
    }
    context.strText = media.GetTitle();

    _paintRow(painter, rc, lvRow, context);
}

void CMedialibView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, CMediaRes& MediaRes)
{
    tagItemContext context;
    context.eStyle = E_ItemStyle::IS_Underline;
    context.pixmap = &m_pmDir;
    if (MediaRes.IsDir())
    {
        context.strText = MediaRes.GetName();
    }
    else
    {
        context.strText = MediaRes.GetTitle();
    }

    if (MediaRes.IsDir())
    {
        context.eStyle = E_ItemStyle::IS_RightTip;

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
    }

    _paintRow(painter, rc, lvRow, context);
}

void CMedialibView::_onPaintRootRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    tagRootItemContext context;
    if (_getRootItemContext(lvRow, context))
    {
        _paintRow(painter, rc, lvRow, context);
    }
}

void CMedialibView::_paintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, const tagItemContext& context)
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

    CListViewEx::_paintRow(painter, rc, lvRow, context);
}

void CListViewEx::_onRowClick(const tagLVRow& lvRow, const QMouseEvent&)
{
    UINT uRow = lvRow.uRow;

    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uRow, [&](CMediaSet& mediaSet){
                _saveScrollRecord();

                showMediaSet(mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uRow, [&](CMedia& media){
                _onRowClick(lvRow, media);
            });
        }
    }
    else if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(uRow, [&](CPath& subPath) {
            if (subPath.IsDir())
            {
                _saveScrollRecord();

                showMediaRes((CMediaRes&)subPath);
            }
            else
            {
                _onRowClick(lvRow, (CMediaRes&)subPath);
            }

        });
    }
    else
    {
        _saveScrollRecord();

        _onRootRowClick(lvRow);
    }
}

void CMedialibView::_onRootRowClick(const tagLVRow& lvRow)
{
    tagRootItemContext context;
    if (_getRootItemContext(lvRow, context))
    {
        if (context.pMediaSet)
        {
            showMediaSet(*context.pMediaSet);
        }
        else if (context.pMediaRes)
        {
            showMediaRes(*context.pMediaRes);
        }
    }
}

void CMedialibView::_onRowClick(const tagLVRow& lvRow, IMedia& Media)
{
    flashRow(lvRow.uRow);
    selectRow(lvRow.uRow);

    m_view.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(Media)));
}

bool CMedialibView::handleReturn()
{
    if (&m_SingerLib == m_pMediaset || &m_PlaylistLib == m_pMediaset)
    {
        showRoot();
        return true;
    }

    // if (m_pMediaRes && m_pMediaRes->parent() == NULL)
    // {    if (&m_RootMediaRes != m_pMediaRes || &m_sdcard = m_pMediaRes)

    if (dynamic_cast<CAttachDir*>(m_pMediaRes) != NULL)
    {
        showMediaRes(m_RootMediaRes);
        return true;
    }

    if (!CListViewEx::handleReturn())
    {
        return false;
    }

    m_medialibDlg.showUpwardButton(!isInRoot());

    return true;
}
