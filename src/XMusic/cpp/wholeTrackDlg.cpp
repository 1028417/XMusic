
#include "app.h"

#include "medialibdlg.h"

#include "wholeTrackDlg.h"
#include "ui_wholeTrackDlg.h"

static Ui::WholeTrackDlg ui;

CWholeTrackDlg::CWholeTrackDlg(CMedialibDlg& medialibDlg, class CApp& app)
    : CDialog(medialibDlg)
    , m_medialibDlg(medialibDlg)
    , m_app(app)
    , m_lv(this)
{
    ui.setupUi(this);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QWidget::close);

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(m_pMediaRes)));
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

    cauto cueFile = ((CSnapshotMediaRes&)mediaRes).cueFile();
    if (!cueFile)
    {
        return false;
    }

    m_pMediaRes = &mediaRes;

    CDialog::show();

    return true;
}


CWholeTrackView::CWholeTrackView(class CWholeTrackDlg *WholeTrackDlg)
    : CListView(WholeTrackDlg)
{
}

void CWholeTrackView::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{

}

size_t CWholeTrackView::getPageRowCount() const
{
    return 0;
}

size_t CWholeTrackView::getRowCount() const
{
    return 0;
}
