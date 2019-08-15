
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
    m_view.setFont(ui.labelTitle, 2, E_FontWeight::FW_SemiBold);

    ui.btnUpward->setVisible(false);

    m_MedialibView.setTextColor(crText);

    connect(ui.btnReturn, SIGNAL(signal_clicked(CButton*)), this, SLOT(slot_buttonClicked(CButton*)));

    connect(ui.btnUpward, SIGNAL(signal_clicked(CButton*)), this, SLOT(slot_buttonClicked(CButton*)));
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

    _resizeTitle();
}

void CMedialibDlg::setTitle(const wstring& strTitle) const
{
    ui.labelTitle->setText(wsutil::toQStr(strTitle));
}

CMedialibView::CMedialibView(class CPlayerView& view, CMedialibDlg& medialibDlg) :
    CListView(&medialibDlg)
    , m_view(view)
    , m_medialibDlg(medialibDlg)
    , m_RootMediaRes(view.getModel().getRootMediaRes())
    , m_SingerLib(view.getModel().getSingerMgr())
    , m_PlaylistLib(view.getModel().getPlaylistMgr())
{
    (void)m_pixmapDir.load(":/img/dir.png");
    (void)m_pixmapDirLink.load(":/img/dirLink.png");
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
    m_pMediaRes = NULL;

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    update(0, true);

    m_medialibDlg.showUpwardButton(false);

    m_medialibDlg.setTitle(L"");
}

void CMedialibView::showMediaRes(CMediaRes& MediaRes, CMediaRes *pHittestItem)
{
    if (MediaRes.IsDir())
    {
        m_pMediaRes = &MediaRes;

        m_pMediaset = NULL;
        m_lstSubSets.clear();
        m_lstSubMedias.clear();

        update(_scrollRecord(), true);

        m_medialibDlg.showUpwardButton(true);

        if (pHittestItem)
        {
            int nIdx = MediaRes.GetSubPath().indexOf(pHittestItem);
            if (nIdx >= 0)
            {
                selectItem((UINT)nIdx);
            }
        }

        WString strTitle;
        _getTitle(MediaRes, strTitle);
        m_medialibDlg.setTitle(strTitle);
    }
}

void CMedialibView::showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem)
{
    m_pMediaRes = NULL;

    m_pMediaset = &MediaSet;

    m_lstSubSets.clear();
    m_pMediaset->GetSubSets(m_lstSubSets);

    m_lstSubMedias.clear();
    m_pMediaset->GetMedias(m_lstSubMedias);

    update(_scrollRecord(), true);

    m_medialibDlg.showUpwardButton(true);

    if (pHittestItem)
    {
        int nIdx = pHittestItem->index();
        if (nIdx >= 0)
        {
            selectItem((UINT)nIdx);
        }
    }

    WString strTitle;
    _getTitle(MediaSet, strTitle);
    m_medialibDlg.setTitle(strTitle);
}

void CMedialibView::_getTitle(CMediaRes& MediaRes, WString& strTitle)
{
    if (&MediaRes == &m_RootMediaRes)
    {
        strTitle << L"媒体库";
        return;
    }

    auto pParent = MediaRes.parent();
    if (pParent)
    {
        _getTitle(*pParent, strTitle);
    }

    strTitle << __CNDot << MediaRes.GetName();
}

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

    strTitle << __CNDot << MediaSet.m_strName;
}

UINT CMedialibView::getRowCount()
{
    if (NULL == m_pMediaset && NULL == m_pMediaRes)
    {
        return 7;
    }

    UINT uRet = 10;
    int cy = m_medialibDlg.height();
    if (cy > 1920)
    {
        uRet++;

        if (cy > 2160)
        {
           uRet++;

            if (cy > 2340)
            {
               uRet++;
            }
        }
    }
    else if (cy < 1800)
    {
        uRet = round((float)uRet*m_medialibDlg.height()/1800);
    }

    return uRet;
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

void CMedialibView::_onPaintItem(CPainter& painter, QRect& rc, const tagListViewItem& lvitem)
{
    if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(lvitem.uItem, [&](CPath& subPath) {
            _paintMediaResItem(painter, rc, lvitem, (CMediaRes&)subPath);
        });
    }
    else if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvitem.uItem, [&](CMediaSet& mediaSet){
                _paintMediaSetItem(painter, rc, lvitem, mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvitem.uItem, [&](CMedia& Media) {
                tagItemContext context;
                context.eStyle = E_ItemStyle::IS_Underline;
                if (Media.GetMediaSetType() == E_MediaSetType::MST_Album)
                {
                    context.pixmap = &m_pixmapAlbumItem;
                }
                else
                {
                    context.pixmap = &m_pixmapPlayItem;
                }
                context.strText = Media.GetTitle();

                _paintItem(painter, rc, lvitem, context);
            });
        }
    }
    else
    {
        tagItemContext context;
        context.eStyle = E_ItemStyle::IS_Normal;
        switch (lvitem.uItem)
        {
        case 1:
            context.pixmap = &m_pixmapDir;
            context.strText = L"媒体库";
            break;
        case 3:
            context.pixmap = &m_pixmapSingerGroup;
            context.strText = L"歌手库";

            break;
        case 5:
            context.pixmap = &m_pixmapPlaylist;
            context.strText = L"列表库";

            break;
        default:
            return;
            break;
        }        
        _paintItem(painter, rc, lvitem, context);
    }
}

void CMedialibView::_paintMediaResItem(CPainter& painter, QRect& rc, const tagListViewItem& lvitem, CMediaRes& MediaRes)
{
    tagItemContext context;
    context.eStyle = E_ItemStyle::IS_Underline;
    context.pixmap = &m_pixmapDir;
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

        context.pixmap = &m_pixmapDir;

        if (&m_RootMediaRes == m_pMediaRes)
        {
            if (MediaRes.parent() == NULL)
            {
                CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(&MediaRes);
                if (pAttachDir)
                {
                    context.pixmap = &m_pixmapDirLink;

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
    }
    else
    {
       context.pixmap = &m_pixmapFile;
    }

    _paintItem(painter, rc, lvitem, context);
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

void CMedialibView::_paintMediaSetItem(CPainter& painter, QRect& rc, const tagListViewItem& lvitem, CMediaSet& MediaSet)
{
    tagItemContext context;
    context.eStyle = E_ItemStyle::IS_RightTip;
    context.strText = MediaSet.m_strName;

    switch (MediaSet.m_eType)
    {
    case E_MediaSetType::MST_Playlist:
        context.pixmap = &m_pixmapPlaylist;
        break;
    case E_MediaSetType::MST_Album:
        context.pixmap = &m_pixmapAlbum;
        break;
    case E_MediaSetType::MST_Singer:
        context.pixmap = &_getSingerPixmap((CSinger&)MediaSet);
        break;
    case E_MediaSetType::MST_SingerGroup:
        context.pixmap = &m_pixmapSingerGroup;
        break;
    default:
        return;
        break;
    };

    _paintItem(painter, rc, lvitem, context);
}

void CMedialibView::_paintItem(CPainter& painter, QRect& rc, const tagListViewItem& lvitem, const tagItemContext& context)
{
    if (lvitem.bSelect)
    {
        QColor crBkg = m_medialibDlg.bkgColor();
        crBkg.setRed(crBkg.red()-10);
        crBkg.setGreen(crBkg.green()-5);
        painter.fillRect(rc.left(), rc.top(), rc.width(), rc.height()-1, crBkg);
    }

    if (lvitem.bFlash)
    {
        int r = m_crText.red();
        int g = m_crText.green();
        int b = m_crText.blue();

        cauto& crBkg = m_medialibDlg.bkgColor();
        r += (-r + crBkg.red())*85 / 255;
        g += (-g + crBkg.green())*85 / 255;
        b += (-b + crBkg.blue())*85 / 255;

        QColor crText(r,g,b);// = m_crText;
        //crText.setAlpha(crText.alpha()*80/100);
        painter.setPen(crText);
    }

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

        rc.setLeft(x_icon + sz_icon + nMargin);
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

            painter.drawPixmap(x_righttip, y_righttip, sz_righttip, sz_righttip, m_pixmapRightTip);

            rc.setRight(x_righttip - nMargin);
        }
    }

    QString qsText = painter.fontMetrics(). elidedText(wsutil::toQStr(context.strText)
                            , Qt::ElideRight, rc.width(), Qt::TextShowMnemonic);
    painter.drawText(rc, Qt::AlignLeft|Qt::AlignVCenter, qsText);
}

void CMedialibView::_handleRowClick(UINT uRowIdx, const QMouseEvent&)
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
    _saveScrollRecord();

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
        _saveScrollRecord();

        showMediaRes(MediaRes);
    }
    else
    {
        _handleItemClick((IMedia&)MediaRes);
    }
}

float& CMedialibView::_scrollRecord()
{
    void *p = m_pMediaRes;
    if (m_pMediaset)
    {
        p = m_pMediaset;
    }

    return m_mapScrollRecord[p];
}

void CMedialibView::_saveScrollRecord()
{
    _scrollRecord() = scrollPos();
}

void CMedialibView::_clearScrollRecord()
{
    if (m_pMediaRes)
    {
        m_mapScrollRecord.erase(m_pMediaRes);
    }
    else if (m_pMediaset)
    {
        m_mapScrollRecord.erase(m_pMediaset);
    }
}

bool CMedialibView::handleReturn()
{
    _clearScrollRecord();

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
