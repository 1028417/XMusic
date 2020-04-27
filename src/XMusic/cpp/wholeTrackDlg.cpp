
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

    ui.labelDisk->setPixmapRound(0);
    ui.labelDisk->setShadow(0);

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

void CWholeTrackDlg::relayout(cqrc rcBtnReturn, cqrc rcLabelDisk, cqrc rcBtnPlay, cqrc rcLv)
{
     ui.btnReturn->setGeometry(rcBtnReturn);

     ui.labelDisk->setGeometry(rcLabelDisk);

     ui.btnPlay->setGeometry(rcBtnPlay);

     auto xOffset = rcLabelDisk.left() - rcBtnReturn.right();
     auto x_title = rcLabelDisk.right() + xOffset;
     ui.labelTitle->setGeometry(x_title, rcLabelDisk.top(), rcBtnPlay.left()-xOffset-x_title, rcLabelDisk.height());

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

    cauto pm = ((int)m_pMediaRes->quality() >= (int)E_MediaQuality::MQ_CD) ? m_app.m_pmHDDisk : m_app.m_pmLLDisk;
    ui.labelDisk->setPixmap(pm);

    ui.labelTitle->setText(__WS2Q(m_pMediaRes->GetTitle()));

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
    reset();
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

        context.strText << uIdx << L"   " << TrackInfo.strTitle;


        if (!m_cue.m_alTrackInfo.get(context->uRow+1, [&](const tagTrackInfo& nextTrackInfo){
                                     uDuration = (nextTrackInfo.uMsBegin - TrackInfo.uMsBegin)/1000;
        }))
        {
            uDuration = m_uDuration - TrackInfo.uMsBegin/1000;
        }
    });

    if (uDuration > 0)
    {
        CPainterFontGuard fontGuard(painter, 0.9f, QFont::Weight::ExtraLight);

        cauto qsDuration = __WS2Q(IMedia::genDurationString(uDuration) + L' ');
        painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter, qsDuration);
    }

    rc.setRight(rc.right() - __size(120));

    return CListView::_paintText(context, painter, rc, flags, uShadowAlpha, uTextAlpha);
}