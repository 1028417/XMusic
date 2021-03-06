
#include "xmusic.h"

#include "medialibdlg.h"

#include "wholeTrackDlg.h"
#include "ui_wholeTrackDlg.h"

static Ui::WholeTrackDlg ui;

CWholeTrackDlg::CWholeTrackDlg() :
    m_lv(*this)
{
}

void CWholeTrackDlg::init()
{
    ui.setupUi(this);

    ui.labelDisk->setPixmapRound(0);
    ui.labelDisk->setShadow(0);

    ui.labelTitle->setFont(__titleFontSize, TD_FontWeight::DemiBold);

    m_lv.adjustFontWeight(TD_FontWeight::Normal);

    ui.btnReturn->connect_dlgClose(this);

    ui.btnPlay->onClicked([&]{
        if (m_pMedia)
        {
            g_app.getCtrl().callPlayCmd(tagPlayMediaCmd(*m_pMedia));
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

    cauto pm = ((int)media.quality() >= (int)E_MediaQuality::MQ_CD) ? g_app.m_pmHDDisk : g_app.m_pmSQDisk;
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

     auto x_title = rcLabelDisk.right() + (rcLabelDisk.x() - rcBtnReturn.right());
     auto cx_title = rcBtnPlay.x()-rcBtnReturn.x()-x_title;
     int cyMargin = m_lv.y()-rcBtnReturn.bottom();
     ui.labelTitle->setGeometry(x_title, rcBtnReturn.top()-cyMargin
                                , cx_title, rcBtnReturn.height() + cyMargin*2);

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
                                 , int flags, UINT uTextAlpha, UINT uShadowAlpha)
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

        CPainterFontGuard fontGuard(painter, 0.9f, TD_FontWeight::ExtraLight);
        painter.drawTextEx(rc, Qt::AlignRight|Qt::AlignVCenter, qsDuration);
    }

    rc.setRight(rc.right() - __size100);

    return CListView::_paintText(context, painter, rc, flags, uTextAlpha, uShadowAlpha);
}
