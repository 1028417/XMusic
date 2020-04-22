
#include "app.h"

#include "medialibdlg.h"

#include "wholeTrackDlg.h"
#include "ui_wholeTrackDlg.h"

static Ui::WholeTrackDlg ui;

CWholeTrackDlg::CWholeTrackDlg(CMedialibDlg& medialibDlg, class CApp& app)
    : CDialog(medialibDlg)
    , m_medialibDlg(medialibDlg)
    , m_app(app)
    , m_lv(*this)
{
    ui.setupUi(this);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QWidget::close);

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        if (m_pMediaRes)
        {
            m_app.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(m_pMediaRes)));
        }
    });
}

void CWholeTrackDlg::relayout(cqrc rcBtnReturn, cqrc rcBtnPlay, cqrc rcLv)
{
     ui.btnReturn->setGeometry(rcBtnReturn);

     ui.btnPlay->setGeometry(rcBtnPlay);

     m_lv.setGeometry(rcLv);
}

bool CWholeTrackDlg::tryShow(CMediaRes& mediaRes)
{
    if (mediaRes.parent()->dirType() != E_MediaDirType::MDT_Snapshot)
    {
        return false;
    }

    cauto cue = ((CSnapshotMediaRes&)mediaRes).cueFile();
    if (!cue)
    {
        return false;
    }

    m_pMediaRes = &mediaRes;

    m_lv.showCue(cue);

    CDialog::show();

    return true;
}


CWholeTrackView::CWholeTrackView(CWholeTrackDlg& WholeTrackDlg)
    : CListView(&WholeTrackDlg)
    , m_WholeTrackDlg(WholeTrackDlg)
{
}

void CWholeTrackView::showCue(CCueFile cue)
{
    m_cue = cue;
    update();
}

size_t CWholeTrackView::getPageRowCount() const
{
    return CMedialibDlg::getPageRowCount(m_WholeTrackDlg.height());
}

size_t CWholeTrackView::getRowCount() const
{
    return m_cue.m_alTrackInfo.size();
}

void CWholeTrackView::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{
    tagRowContext rowContext(lvRow);
    m_cue.m_alTrackInfo.get(lvRow.uRow, [&](const tagTrackInfo& TrackInfo){
        rowContext.strText << ' ';
        auto uIdx = TrackInfo.uIndex+1;
        if (uIdx<10)
        {
            rowContext.strText << '0';
        }
        rowContext.strText << uIdx << wstring(5, ' ') << TrackInfo.strTitle;
    });
    _paintRow(painter, rowContext);
}

cqrc CWholeTrackView::_paintText(const tagRowContext& context, CPainter& painter, QRect& rc
                                 , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    return CListView::_paintText(context, painter, rc, flags, uShadowAlpha, uTextAlpha);
}
