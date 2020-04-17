
#include "app.h"

#include "medialibdlg.h"

#include "wholeTrackDlg.h"
#include "ui_wholeTrackDlg.h"

static Ui::WholeTrackDlg ui;

CWholeTrackDlg::CWholeTrackDlg(CMedialibDlg& medialibDlg, class CApp& app)
    : CDialog(medialibDlg)
    , m_medialibDlg(medialibDlg)
    , m_app(app)
{
    ui.setupUi(this);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QWidget::close);

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(TD_IMediaList(m_pMediaRes)));
    });
}

void CWholeTrackDlg::relayout(cqrc rcBtnReturn, cqrc rcBtnPlay)
{
     ui.btnReturn->setGeometry(rcBtnReturn);

     ui.btnPlay->setGeometry(rcBtnPlay);
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
