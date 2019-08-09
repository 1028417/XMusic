
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

    m_MedialibView.setTextColor(QColor(64, 64, 255));

    connect(ui.btnReturn, SIGNAL(signal_clicked(CButton*)), this, SLOT(slot_buttonClicked(CButton*)));
}

void CMedialibDlg::_relayout(int cx, int cy)
{
#define __margin 30
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

void CMedialibView::showMediaSet(CMediaSet *pMediaSet)
{
    m_pMediaset = pMediaSet;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();
    if (m_pMediaset)
    {
        m_pMediaset->GetSubSets(m_lstSubSets);
        m_pMediaset->GetMedias(m_lstSubMedias);
    }

    m_pMediaRes = NULL;

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
    return MIN(getItemCount(), 10);
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
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uItem, [&](CMediaSet& mediaSet){
                _onPaintItem(painter, mediaSet, rcItem);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uItem, [&](CMedia& media){
                _onPaintItem(painter, media, rcItem);
            });
        }
    }
    else if (m_pMediaRes)
    {
        m_pMediaRes->GetSubPath().get(uItem, [&](CPath& subPath) {
            _onPaintItem(painter, (CMediaRes&)subPath, rcItem);
        });
    }
    else
    {
        switch (uItem)
        {
        case 0:
            _onPaintItem(painter, m_view.getModel().getRootMediaRes(), rcItem);
            break;
        case 1:
            _onPaintItem(painter, m_view.getModel().getSingerMgr(), rcItem);
            break;
        case 2:
            _onPaintItem(painter, m_view.getModel().getPlaylistMgr(), rcItem);
            break;
        default:
            break;
        }
    }
}

void CMedialibView::_onPaintItem(QPainter& painter, CMediaSet& MediaSet, QRect& rcItem)
{
    painter.drawText(rcItem, Qt::AlignLeft|Qt::AlignVCenter, wsutil::toQStr(MediaSet.m_strName));
}

void CMedialibView::_onPaintItem(QPainter& painter, CMedia& Media, QRect& rcItem)
{
    painter.drawText(rcItem, Qt::AlignLeft|Qt::AlignVCenter, wsutil::toQStr(Media.m_strName));
}

void CMedialibView::_onPaintItem(QPainter& painter, CMediaRes& MediaRes, QRect& rcItem)
{
    painter.drawText(rcItem, Qt::AlignLeft|Qt::AlignVCenter, wsutil::toQStr(MediaRes.GetName()));
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
            _handleItemClick(m_view.getModel().getRootMediaRes());
            break;
        case 1:
            _handleItemClick(m_view.getModel().getSingerMgr());
            break;
        case 2:
            _handleItemClick(m_view.getModel().getPlaylistMgr());
            break;
        default:
            break;
        }
    }
}

void CMedialibView::_handleItemClick(CMediaSet& MediaSet)
{
    showMediaSet(&MediaSet);
}

void CMedialibView::_handleItemClick(IMedia& Media)
{
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
