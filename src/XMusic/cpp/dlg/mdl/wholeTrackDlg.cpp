
#include "xmusic.h"

#include "medialibdlg.h"

#include "wholeTrackDlg.h"
#include "ui_wholeTrackDlg.h"

static Ui::WholeTrackDlg ui;

CWholeTrackDlg::CWholeTrackDlg(CMedialibDlg& medialibDlg)
    : CDialog(medialibDlg)
    , m_lv(*this)
{
}

void CWholeTrackDlg::init()
{
    ui.setupUi(this);

    ui.labelDisk->setPixmapRound(0);
    ui.labelDisk->setShadow(0);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    m_lv.setFont(1.0f, QFont::Weight::Normal);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QWidget::close);

    connect(ui.btnPlay, &CButton::signal_clicked, [&]{
        if (m_pMedia)
        {
            __app.getCtrl().callPlayCmd(tagPlayMediaCmd(*m_pMedia));
        }
    });
}

bool CWholeTrackDlg::tryShow(IMedia& media)
{
    cauto cue = media.cueFile();
    if (!cue)
    {
        return false;
    }

    m_pMedia = &media;

    cauto pm = ((int)media.quality() >= (int)E_MediaQuality::MQ_CD) ? __app.m_pmHDDisk : __app.m_pmLLDisk;
    ui.labelDisk->setPixmap(pm);

    ui.labelTitle->setText(__WS2Q(media.GetTitle()));

    m_lv.setCue(cue, media.duration());

    CDialog::show([&]{
        m_lv.reset();
    });

    return true;
}

void CWholeTrackDlg::relayoutTitle(cqrc rcBtnReturn, cqrc rcLabelDisk, cqrc rcBtnPlay, cqrc rcLv)
{
     ui.btnReturn->setGeometry(rcBtnReturn);

     ui.labelDisk->setGeometry(rcLabelDisk);

     ui.btnPlay->setGeometry(rcBtnPlay);

     auto x_title = rcLabelDisk.right() + rcBtnReturn.x();
     QRect rcTitle(x_title, 0, rcBtnPlay.x()-rcBtnReturn.x()-x_title, rcLv.y());
     ui.labelTitle->setGeometry(rcTitle);

     m_lv.setGeometry(rcLv);
}

CWholeTrackView::CWholeTrackView(CWholeTrackDlg& WholeTrackDlg)
    : CListView(&WholeTrackDlg, E_LVScrollBar::LVSB_Right)
    , m_WholeTrackDlg(WholeTrackDlg)
{
}

void CWholeTrackView::setCue(CCueFile cue, UINT uDuration)
{
    m_cue = cue;
    m_uDuration = uDuration;
}

size_t CWholeTrackView::getColCount() const
{
    if (m_WholeTrackDlg.isHLayout() && m_cue.m_alTrackInfo.size() > getRowCount())
    {
        return 2;
    }

    return 1;
}

size_t CWholeTrackView::getRowCount() const
{
    return CMedialibDlg::caleRowCount(m_WholeTrackDlg.height());
}

size_t CWholeTrackView::getItemCount() const
{
    return m_cue.m_alTrackInfo.size();
}

cqrc CWholeTrackView::_paintText(tagLVItemContext& context, CPainter& painter, QRect& rc
                                 , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    _paintBottonLine(painter, rc);

    UINT uDuration = 0;
    m_cue.m_alTrackInfo.get(context->uItem, [&](const tagTrackInfo& TrackInfo) {
        auto uIdx = TrackInfo.uIndex+1;
        if (uIdx<10)
        {
            context.strText << '0';
        }

        context.strText << uIdx << L"  " << TrackInfo.strTitle;


        if (!m_cue.m_alTrackInfo.get(context->uItem+1, [&](const tagTrackInfo& nextTrackInfo){
                                     uDuration = (nextTrackInfo.uMsBegin - TrackInfo.uMsBegin)/1000;
        }))
        {
            uDuration = m_uDuration - TrackInfo.uMsBegin/1000;
        }
    });

    rc.setLeft(rc.left() + __size(15));
    rc.setRight(rc.right() - __size(20));

    if (uDuration > 0)
    {
        cauto qsDuration = __WS2Q(IMedia::genDurationString(uDuration));

        CPainterFontGuard fontGuard(painter, 0.9f, QFont::Weight::ExtraLight);
        painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter, qsDuration);
    }

    rc.setRight(rc.right() - __size100);

    return CListView::_paintText(context, painter, rc, flags, uShadowAlpha, uTextAlpha);
}
