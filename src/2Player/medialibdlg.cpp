
#include "view.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(class CPlayerView& view, QWidget *parent) :
    CDialog(parent)
    , m_view(view)
    , m_MedialibView(view, this)
{
    ui.setupUi(this);

    QColor crText(64, 128, 255);

    m_view.setTextColor(ui.labelTitle, crText);
    m_view.setFont(ui.labelTitle, 5, true);

    m_MedialibView.setTextColor(crText);
    m_view.setFont(&m_MedialibView, 2, false);

    connect(ui.btnReturn, SIGNAL(signal_clicked(CButton*)), this, SLOT(slot_buttonClicked(CButton*)));
}

void CMedialibDlg::_relayout(int cx, int cy)
{
#define __margin 40
    int y_MedialibView = ui.btnReturn->geometry().bottom() + __margin;
    m_MedialibView.setGeometry(__margin,y_MedialibView,cx-__margin*2,cy-__margin-y_MedialibView);
}

void CMedialibDlg::slot_buttonClicked(CButton* button)
{
    if (button == ui.btnReturn)
    {
        this->close();
    }
}

CMedialibView::CMedialibView(class CPlayerView& view, QWidget *parent) :
    CListView(parent)
    , m_view(view)
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
}

void CMedialibView::showRoot()
{
    m_pMediaRes = NULL;

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    this->update();
}

void CMedialibView::showMediaSet(CMediaSet& MediaSet)
{
    m_pMediaRes = NULL;

    m_pMediaset = &MediaSet;

    m_lstSubSets.clear();
    m_pMediaset->GetSubSets(m_lstSubSets);

    m_lstSubMedias.clear();
    m_pMediaset->GetMedias(m_lstSubMedias);

    this->update();
}

void CMedialibView::showMediaRes(CMediaRes& MediaRes)
{
    m_pMediaRes = &MediaRes;

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    this->update();
}

UINT CMedialibView::getRowCount()
{
    if (NULL == m_pMediaset && NULL == m_pMediaRes)
    {
        return 3;
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
        return 3;
    }
}

void CMedialibView::_onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem)
{
    if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(uItem, [&](CPath& subPath) {
            _paintItem(painter, rcItem, (CMediaRes&)subPath);
        });
    }
    else if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uItem, [&](CMediaSet& mediaSet){
                _paintItem(painter, rcItem, mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uItem, [&](CMedia& media){
                _paintItem(painter, rcItem, media);
            });
        }
    }
    else
    {
        switch (uItem)
        {
        case 0:
            _paintItem(painter, rcItem, m_RootMediaRes);
            break;
        case 1:
            _paintItem(painter, rcItem, m_SingerLib);
            break;
        case 2:
            _paintItem(painter, rcItem, m_PlaylistLib);
            break;
        default:
            break;
        }
    }
}

void CMedialibView::_paintItem(QPainter& painter, QRect& rcItem, CMediaRes& MediaRes)
{
    QPixmap *pPixmap = &m_pixmapFolder;
    int xOffset = 0;
    if (&MediaRes == &m_RootMediaRes)
    {
        xOffset = rcItem.width()/2-100;
    }
    else
    {
        if (MediaRes.IsDir())
        {
            if (MediaRes.parent() == NULL)
            {
                pPixmap = &m_pixmapFolderLink;
            }
        }
        else
        {
           pPixmap = &m_pixmapFile;
        }
    }

    cauto& qsName = wsutil::toQStr(MediaRes.GetName());
    _paintItem(painter, rcItem, qsName, *pPixmap, xOffset);
}

void CMedialibView::_paintItem(QPainter& painter, QRect& rcItem, CMediaSet& MediaSet)
{
    int xOffset = 0;
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

        break;
    case E_MediaSetType::MST_SingerGroup:
        pPixmap = &m_pixmapSingerGroup;
        break;
    default:
        if (&MediaSet == &m_SingerLib)
        {
            pPixmap = &m_pixmapSingerGroup;
        }
        else
        {
            pPixmap = &m_pixmapPlaylist;
        }

        xOffset = rcItem.width()/2-100;

        break;
    };

    cauto& qsName = wsutil::toQStr(MediaSet.m_strName);
    _paintItem(painter, rcItem, qsName, *pPixmap, xOffset);
}

void CMedialibView::_paintItem(QPainter& painter, QRect& rcItem, CMedia& Media)
{
    cauto& qsName = wsutil::toQStr(Media.m_strName);
    if (Media.GetMediaSetType() == E_MediaSetType::MST_Album)
    {
        _paintItem(painter, rcItem, qsName, m_pixmapAlbumItem);
    }
    else if (Media.GetMediaSetType() == E_MediaSetType::MST_Playlist)
    {
        _paintItem(painter, rcItem, qsName, m_pixmapPlayItem);
    }
}

void CMedialibView::_paintItem(QPainter& painter, QRect& rcItem, const QString& qsTitle, QPixmap& pixmap, int xOffset)
{
    int sz_icon = 120;
    sz_icon = MIN(sz_icon, rcItem.height()-10);

    int x_icon = rcItem.left()+xOffset;
    int y_icon = rcItem.center().y()-sz_icon/2;
    painter.drawPixmap(x_icon, y_icon, sz_icon, sz_icon, pixmap);

    rcItem.setLeft(x_icon+sz_icon + 20);
    painter.drawText(rcItem, Qt::AlignLeft|Qt::AlignVCenter, qsTitle);

    painter.fillRect(rcItem.left(), y_icon+sz_icon, rcItem.width(), 1, QColor(255,255,255));
}

void CMedialibView::_handleRowClick(UINT uRowIdx)
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
            });
        }
    }
    else if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(uRowIdx, [&](CPath& subPath) {
            _handleItemClick((CMediaRes&)subPath);
        });
    }
    else
    {
        switch (uRowIdx)
        {
        case 0:
            _handleItemClick(m_RootMediaRes);
            break;
        case 1:
            _handleItemClick(m_SingerLib);
            break;
        case 2:
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
