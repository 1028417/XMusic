
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

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    m_lv.setFont(1.04, QFont::Weight::Normal);

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

     auto x_title = rcBtnReturn.right()+rcBtnReturn.left();
     ui.labelTitle->setGeometry(x_title, rcBtnReturn.top(), rcBtnPlay.left()
                                -rcBtnReturn.left()-x_title, rcBtnReturn.height());

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

    ui.labelTitle->setText(__WS2Q(m_pMediaRes->GetName()));

    m_lv.showCue(cue, m_pMediaRes->duration());

    CDialog::show();

    return true;
}


CWholeTrackView::CWholeTrackView(CWholeTrackDlg& WholeTrackDlg)
    : CListView(&WholeTrackDlg)
    , m_WholeTrackDlg(WholeTrackDlg)
{
}

void CWholeTrackView::showCue(CCueFile cue, UINT uDuration)
{
    m_cue = cue;
    m_uDuration = uDuration;
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

cqrc CWholeTrackView::_paintText(tagRowContext& context, CPainter& painter, QRect& rc
                                 , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    UINT uDuration = 0;
    m_cue.m_alTrackInfo.get(context->uRow, [&](const tagTrackInfo& TrackInfo){
        context.strText << ' ';

        auto uIdx = TrackInfo.uIndex+1;
        if (uIdx<10)
        {
            context.strText << '0';
        }

        context.strText << uIdx << wstring(4, ' ') << TrackInfo.strTitle;


        if (!m_cue.m_alTrackInfo.get(context->uRow+1, [&](const tagTrackInfo& nextTrackInfo){
                                     uDuration = (nextTrackInfo.uMsBegin - TrackInfo.uMsBegin)/1000;
        }))
        {
            uDuration = m_uDuration - TrackInfo.uMsBegin/1000;
        }
    });

    if (uDuration > 0)
    {
        CPainterFontGuard fontGuard(painter, 0.8, QFont::Weight::ExtraLight);

        cauto qsDuration = __WS2Q(IMedia::genDurationString(uDuration));
        painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter, qsDuration);
    }

    rc.setRight(rc.right() - __size(100));

    return CListView::_paintText(context, painter, rc, flags, uShadowAlpha, uTextAlpha);
}
