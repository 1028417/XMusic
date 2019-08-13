
#include "view.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(class CPlayerView& view, QWidget *parent) :
    CDialog(parent)
    , m_view(view)
    , m_MedialibView(view, *this)
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);

    m_view.setTextColor(ui.labelTitle, crText);
    m_view.setFont(ui.labelTitle, 5, false);

    ui.btnUpward->setVisible(false);

    m_MedialibView.setTextColor(crText);
    m_view.setFont(&m_MedialibView, 2, false);

    connect(ui.btnReturn, SIGNAL(signal_clicked(CButton*)), this, SLOT(slot_buttonClicked(CButton*)));

    connect(ui.btnUpward, SIGNAL(signal_clicked(CButton*)), this, SLOT(slot_buttonClicked(CButton*)));
}

void CMedialibDlg::_relayout(int cx, int cy)
{
    cauto& rcReturn = ui.btnReturn->geometry();
    int y_margin = rcReturn.top();

    ui.btnUpward->setGeometry(cx-rcReturn.right(), y_margin, rcReturn.width(), rcReturn.height());

    int y_MedialibView = rcReturn.bottom() + y_margin;
    m_MedialibView.setGeometry(0, y_MedialibView, cx, cy-y_margin-y_MedialibView);
}

void CMedialibDlg::slot_buttonClicked(CButton* button)
{
    if (button == ui.btnReturn)
    {
        this->close();
    }
    else if (button == ui.btnUpward)
    {
        (void)m_MedialibView.handleReturn();
    }
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
}

CMedialibView::CMedialibView(class CPlayerView& view, CMedialibDlg& medialibDlg) :
    CListView(&medialibDlg)
    , m_view(view)
    , m_medialibDlg(medialibDlg)
    , m_RootMediaRes(view.getModel().getRootMediaRes())
    , m_SingerLib(view.getModel().getSingerMgr())
    , m_PlaylistLib(view.getModel().getPlaylistMgr())
{
    (void)m_pixmapFolder.load(":/img/folder.png");
    (void)m_pixmapFolderLink.load(":/img/folderLink.png");
    (void)m_pixmapFile.load(":/img/file.png");

    (void)m_pixmapSingerGroup.load(":/img/singergroup.png");
    (void)m_pixmapDefaultSinger.load(":/img/singerdefault.png");
    (void)m_pixmapAlbum.load(":/img/album.png");
    (void)m_pixmapAlbumItem.load(":/img/albumitem.png");

    (void)m_pixmapPlaylist.load(":/img/playlist.png");
    (void)m_pixmapPlayItem.load(":/img/playitem.png");

    (void)m_pixmapRightTip.load(":/img/righttip.png");
}

void CMedialibView::showRoot()
{
    dselectItem();

    m_pMediaRes = NULL;

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    this->update();

    m_medialibDlg.showUpwardButton(false);
}

void CMedialibView::showMediaRes(CMediaRes& MediaRes, CMediaRes *pHittestItem)
{
    if (MediaRes.IsDir())
    {
        dselectItem();

        m_pMediaRes = &MediaRes;

        m_pMediaset = NULL;
        m_lstSubSets.clear();
        m_lstSubMedias.clear();

        this->update();

        m_medialibDlg.showUpwardButton(true);

        if (pHittestItem)
        {
            int nIdx = MediaRes.GetSubPath().indexOf(pHittestItem);
            if (nIdx >= 0)
            {
                selectItem((UINT)nIdx);
            }
        }
    }
}

void CMedialibView::showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem)
{
    dselectItem();

    m_pMediaRes = NULL;

    m_pMediaset = &MediaSet;

    m_lstSubSets.clear();
    m_pMediaset->GetSubSets(m_lstSubSets);

    m_lstSubMedias.clear();
    m_pMediaset->GetMedias(m_lstSubMedias);

    this->update();

    m_medialibDlg.showUpwardButton(true);

    if (pHittestItem)
    {
        int nIdx = pHittestItem->index();
        if (nIdx >= 0)
        {
            selectItem((UINT)nIdx);
        }
    }
}

UINT CMedialibView::getRowCount()
{
    if (NULL == m_pMediaset && NULL == m_pMediaRes)
    {
        return 7;
    }

    return 10;
}

UINT CMedialibView::getItemCount()
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
        return 7;
    }
}

void CMedialibView::_onPaintItem(CPainter& painter, QRect& rc, const tagListViewItem& item)
{
    if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(item.uItem, [&](CPath& subPath) {
            _paintMediaResItem(painter, rc, item, (CMediaRes&)subPath);
        });
    }
    else if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(item.uItem, [&](CMediaSet& mediaSet){
                _paintMediaSetItem(painter, rc, item, mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(item.uItem, [&](CMedia& Media){
                auto& pixmap = Media.GetMediaSetType() == E_MediaSetType::MST_Album? m_pixmapAlbumItem : m_pixmapPlayItem;
                _paintItem(painter, rc, item, pixmap, Media.m_strName, E_ItemStyle::IS_Underline);
            });
        }
    }
    else
    {
        QPixmap *pPixmap = NULL;
        wstring strText;
        switch (item.uItem)
        {
        case 1:
            pPixmap = &m_pixmapFolder;
            strText = L"媒体库";
            break;
        case 3:
            pPixmap = &m_pixmapSingerGroup;
            strText = L"歌手库";

            break;
        case 5:
            pPixmap = &m_pixmapPlaylist;
            strText = L"列表库";

            break;
        default:
            return;
            break;
        }

        _paintItem(painter, rc, item, *pPixmap, strText, E_ItemStyle::IS_Normal);
    }
}

void CMedialibView::_paintMediaResItem(CPainter& painter, QRect& rc, const tagListViewItem& item, CMediaRes& MediaRes)
{
    QPixmap *pPixmap = &m_pixmapFolder;

    E_ItemStyle eStyle = E_ItemStyle::IS_Underline;

    if (MediaRes.IsDir())
    {
        eStyle = E_ItemStyle::IS_RightTip;

        if (MediaRes.parent() == NULL)
        {
            pPixmap = &m_pixmapFolderLink;
        }
    }
    else
    {
       pPixmap = &m_pixmapFile;

    }

    _paintItem(painter, rc, item, *pPixmap, MediaRes.GetName(), eStyle);
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
        g_mapSingerPixmap[&Singer] = &m_pixmapDefaultSinger;
        return m_pixmapDefaultSinger;
    }
}

void CMedialibView::_paintMediaSetItem(CPainter& painter, QRect& rc, const tagListViewItem& item, CMediaSet& MediaSet)
{
    QPixmap *pPixmap = NULL;

    switch (MediaSet.m_eType)
    {
    case E_MediaSetType::MST_Playlist:
        pPixmap = &m_pixmapPlaylist;
        break;
    case E_MediaSetType::MST_Album:
        pPixmap = &m_pixmapAlbum;
        break;
    case E_MediaSetType::MST_Singer:
        pPixmap = &_getSingerPixmap((CSinger&)MediaSet);
        break;
    case E_MediaSetType::MST_SingerGroup:
        pPixmap = &m_pixmapSingerGroup;
        break;
    default:
        return;
        break;
    };

    _paintItem(painter, rc, item, *pPixmap, MediaSet.m_strName, E_ItemStyle::IS_RightTip);
}

void CMedialibView::_paintItem(CPainter& painter, QRect& rc, const tagListViewItem& item, QPixmap& pixmap
                               , const wstring& strText, E_ItemStyle eStyle, UINT uIconSize)
{
    if (item.bSelect)
    {
        QColor crBkg = m_medialibDlg.bkgColor();
        crBkg.setRed(crBkg.red()-10);
        crBkg.setGreen(crBkg.green()-5);
        painter.fillRect(rc.left(), rc.top(), rc.width(), rc.height()-1, crBkg);
    }

    if (item.bFlash)
    {
        QColor crText = m_crText;
        crText.setAlpha(crText.alpha()*60/100);
        painter.setPen(crText);
    }

    int x_margin = 35;
    rc.setLeft(rc.left()+x_margin);
    rc.setRight(rc.right()-x_margin);

    UINT sz_icon = rc.height();
    if (uIconSize > 0 && uIconSize < sz_icon)
    {
        sz_icon = uIconSize;
    }
    else
    {
        sz_icon = sz_icon *2/3;
    }

    int x_icon = 0;
    if (E_ItemStyle::IS_Normal == eStyle)
    {
        x_icon = rc.center().x()-sz_icon;
    }
    else
    {
        x_icon = rc.left();
    }

    int y_icon = rc.center().y()-sz_icon/2;
    QRect rcDst(x_icon, y_icon, sz_icon, sz_icon);
    painter.drawPixmapEx(rcDst, pixmap);

    rc.setLeft(x_icon+sz_icon + 20);

    QString qsText = painter.fontMetrics(). elidedText(wsutil::toQStr(strText)
                            , Qt::ElideRight, rc.width(), Qt::TextShowMnemonic);
    painter.drawText(rc, Qt::AlignLeft|Qt::AlignVCenter, qsText);

    if (eStyle != E_ItemStyle::IS_Normal)
    {
        painter.fillRect(rc.left(), rc.bottom(), rc.width(), 1, QColor(255,255,255,128));

        if (E_ItemStyle::IS_RightTip == eStyle)
        {
            int sz_righttip = sz_icon*30/100;
            int x_righttip = rc.right()-sz_righttip;
            int y_righttip = rc.center().y()-sz_righttip/2;

            painter.drawPixmap(x_righttip, y_righttip, sz_righttip, sz_righttip, m_pixmapRightTip);
        }
    }
}

void CMedialibView::_handleRowClick(UINT uRowIdx, QMouseEvent&)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uRowIdx, [&](CMediaSet& mediaSet){
                _handleItemClick(mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uRowIdx, [&](CMedia& media){
                _handleItemClick(media);

                flashItem(uRowIdx);
                selectItem(uRowIdx);
            });
        }
    }
    else if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(uRowIdx, [&](CPath& subPath) {
            _handleItemClick((CMediaRes&)subPath);

            if (!subPath.IsDir())
            {
                flashItem(uRowIdx);
                selectItem(uRowIdx);
            }
        });
    }
    else
    {
        switch (uRowIdx)
        {
        case 1:
            _handleItemClick(m_RootMediaRes);
            break;
        case 3:
            _handleItemClick(m_SingerLib);
            break;
        case 5:
            _handleItemClick(m_PlaylistLib);
            break;
        default:
            break;
        }
    }
}

void CMedialibView::_handleItemClick(CMediaSet& MediaSet)
{
    showMediaSet(MediaSet);
}

void CMedialibView::_handleItemClick(IMedia& Media)
{
    m_view.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(Media)));
}

void CMedialibView::_handleItemClick(CMediaRes& MediaRes)
{
    if (MediaRes.IsDir())
    {
        showMediaRes(MediaRes);
    }
    else
    {
        _handleItemClick((IMedia&)MediaRes);
    }
}

bool CMedialibView::handleReturn()
{
    if (m_pMediaset)
    {
        if (&m_SingerLib == m_pMediaset || &m_PlaylistLib == m_pMediaset || NULL == m_pMediaset->m_pParent)
        {
            showRoot();
        }
        else
        {
            showMediaSet(*m_pMediaset->m_pParent);
        }
    }
    else if (m_pMediaRes)
    {
        if (&m_RootMediaRes == m_pMediaRes)
        {
            showRoot();
        }
        else
        {
            auto parent = m_pMediaRes->parent();
            if (NULL != parent)
            {
                showMediaRes(*parent);
            }
            else
            {
                showMediaRes(m_RootMediaRes);
            }
        }
    }
    else
    {
        return false;
    }

    return true;
}
