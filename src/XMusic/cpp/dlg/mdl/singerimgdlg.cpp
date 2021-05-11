
#include "xmusic.h"

#include "singerimgdlg.h"
#include "ui_singerimgdlg.h"

static Ui::SingerImgDlg ui;

CSingerImgDlg::CSingerImgDlg() : //CDialog(medialibDlg),
    m_singerImgMgr(g_app.getSingerImgMgr())
    , m_loadingLabel(this)
{
}

void CSingerImgDlg::init()
{
    ui.setupUi(this);

    ui.btnReturn->connect_dlgClose(this);

    ui.btnBackward->onClicked([&]{
        _showImg(-1);
    });
    ui.btnBackward_v->onClicked([&]{
        _showImg(-1);
    });
    ui.btnForward->onClicked([&]{
        _showImg(1);
    });
    ui.btnForward_v->onClicked([&]{
        _showImg(1);
    });
}

void CSingerImgDlg::_relayout(int cx, int cy)
{
    if (m_uImgCount <= 1)
    {
        ui.btnBackward->setVisible(false);
        ui.btnForward->setVisible(false);
        ui.btnBackward_v->setVisible(false);
        ui.btnForward_v->setVisible(false);
        return;
    }

    auto szBtn = ui.btnBackward->height();
    if (cx > cy)
    {
        auto y = cy/2-szBtn/2;
        ui.btnBackward->move(ui.btnReturn->x(), y);
        ui.btnForward->move(cx-ui.btnReturn->x()-szBtn, y);

        ui.btnBackward->setVisible(true);
        ui.btnForward->setVisible(true);
        ui.btnBackward_v->setVisible(false);
        ui.btnForward_v->setVisible(false);
    }
    else
    {
        auto x = cx/2-szBtn/2;
        ui.btnBackward_v->move(x, ui.btnReturn->y());
        ui.btnForward_v->move(x, cy-ui.btnReturn->y()-szBtn);

        ui.btnBackward->setVisible(false);
        ui.btnForward->setVisible(false);
        ui.btnBackward_v->setVisible(true);
        ui.btnForward_v->setVisible(true);
    }

    m_loadingLabel.move((cx-m_loadingLabel.width())/2, (cy-m_loadingLabel.height())/2);
}

void CSingerImgDlg::relayoutTitle(cqrc rcBtnReturn)
{
    ui.btnReturn->setGeometry(rcBtnReturn);
}

static void _genRect(UINT cxSrc, UINT cySrc, UINT cxDst, UINT cyDst, QRect& rc)
{
    int xDst = 0, yDst = 0;
    auto fHWRate = (float)cySrc/cxSrc;
    if (fHWRate > (float)cyDst/cxDst)
    {
        xDst = (cxDst - cyDst/fHWRate)/2;
        cxDst -= xDst*2;
    }
    else
    {
        yDst = (cyDst - cxDst*fHWRate)/2;
        cyDst -= yDst*2;
    }

    rc.setRect(xDst, yDst, cxDst, cyDst);
}

void CSingerImgDlg::_onPaint(CPainter& painter, cqrc rc)
{
    CDialog::_onPaint(painter, rc);

    UINT cxImg = m_pmImg.width();
    if (0 == cxImg)
    {
        return;
    }
    UINT cyImg = m_pmImg.height();

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    UINT cxDst = rc.width();
    UINT cyDst = rc.height();

#define __margin __size(30)
    QRect rcPos;
    _genRect(cxImg, cyImg, cxDst, cyDst, rcPos);
    if (rcPos.x() < __margin && rcPos.y() < __margin)
    {
        _genRect(cxDst, cyDst, cxImg, cyImg, rcPos);
        painter.drawImg(rc, m_pmImg, rcPos);
        return;
    }

    if (cxImg + __margin*2 < cxDst && cyImg + __margin*2 < cyDst)
    {
        rcPos.setRect((cxDst - cxImg)/2, (cyDst - cyImg)/2, cxImg, cyImg);
    }
    else if (rcPos.x() < __margin)
    {
        cyDst = cxDst * cyImg / cxImg;
        rcPos.setRect(0, (rc.height()-cyDst)/2, cxDst, cyDst);
    }
    else if (rcPos.y() < __margin)
    {
        cxDst = cyDst * cxImg / cyImg;
        rcPos.setRect((rc.width()-cxDst)/2, 0, cxDst, cyDst);
    }

    painter.drawImg(rcPos, m_pmImg, __szRound);
}

void CSingerImgDlg::show(UINT uSingerID)
{
    m_uImgCount = m_singerImgMgr.getSingerImgCount(uSingerID);
    if (0 == m_uImgCount)
    {
        return;
    }

    m_uSingerID = uSingerID;
    m_uImgIdx = 0;

    _showImg(0);

    CDialog::show([&]{
        m_pmImg = QPixmap();
    });
}

void CSingerImgDlg::updateSingerImg()
{
    if (m_pmImg.isNull())
    {
        m_uImgIdx = 0;
        _showImg(0);
    }
    else if (m_nSwitchingOffset != 0)
    {
        _showImg(m_nSwitchingOffset);
    }
}

void CSingerImgDlg::_showImg(int nOffset)
{
    auto uImgIdx = m_uImgIdx;
    if (nOffset > 0)
    {
        uImgIdx++;
        if (uImgIdx >= m_uImgCount)
        {
            uImgIdx = 0;
        }
    }
    else if (nOffset < 0)
    {
        if (uImgIdx > 0)
        {
            uImgIdx--;
        }
        else
        {
            uImgIdx = m_uImgCount-1;
        }
    }

    m_nSwitchingOffset = nOffset;

    auto pSingerImg = m_singerImgMgr.getSingerImg(m_uSingerID, uImgIdx, false);
    if (NULL == pSingerImg || !pSingerImg->bExist)
    {
        m_loadingLabel.show();
        return;
    }
    m_loadingLabel.show(false);

    m_nSwitchingOffset = 0;

    m_pmImg.load(__WS2Q(m_singerImgMgr.file(*pSingerImg)));
    update();

    m_uImgIdx = uImgIdx;

    if (m_uImgCount > 1)
    {
        if (m_uImgIdx < m_uImgCount-1)
        {
            (void)m_singerImgMgr.getSingerImg(m_uSingerID, uImgIdx, false);
        }

        uImgIdx = (0 == m_uImgIdx) ? (m_uImgCount-1) : (m_uImgIdx-1);
        (void)m_singerImgMgr.getSingerImg(m_uSingerID, uImgIdx, false);
    }
}

void CSingerImgDlg::_onTouchEvent(const CTouchEvent& te)
{
    if (m_pmImg.isNull() || m_uImgCount <= 1)
    {
        return;
    }

    if (te.type() != E_TouchEventType::TET_TouchEnd)
    {
        return;
    }

    if (te.dt() < __fastTouchDt)
    {
        auto dx = te.dx();
        auto dy = te.dy();
        if (abs(dx) > abs(dy))
        {
            if (dx >= 3)
            {
                _showImg(-1);
                return;
            }
            else if (dx <= -3)
            {
                _showImg(1);
                return;
            }
        }
        else
        {
            if (dy >= 3)
            {
                _showImg(-1);
                return;
            }
            else if (dy <= -3)
            {
                _showImg(1);
                return;
            }
        }
    }

    if (m_bHLayout)
    {
        if (te.x() < rect().center().x()-__size100)
        {
            _showImg(-1);
        }
        else
        {
            _showImg(1);
        }
    }
    else
    {
        if (te.y() < rect().center().y()-__size100)
        {
            _showImg(-1);
        }
        else
        {
            _showImg(1);
        }
    }
}
