
#include "xmusic.h"

#include "singerimgdlg.h"
#include "ui_singerimgdlg.h"

static Ui::SingerImgDlg ui;

CSingerImgDlg::CSingerImgDlg(CMedialibDlg& medialibDlg)
    : CDialog(medialibDlg)
    , m_singerImgMgr(__app.getSingerImgMgr())
{
}

void CSingerImgDlg::init()
{
    ui.setupUi(this);
    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnBackward, &CButton::signal_clicked, [&]{
        _showImg(-1);
    });
    connect(ui.btnForward, &CButton::signal_clicked, [&]{
        _showImg(1);
    });
}

void CSingerImgDlg::_relayout(int cx, int cy)
{
    if (cx > cy)
    {
        auto y = cy/2-ui.btnBackward->height()/2;
        ui.btnBackward->move(ui.btnReturn->x(), y);
        ui.btnForward->move(cx-ui.btnReturn->x()-ui.btnForward->width(), y);
    }
    else
    {
        auto x = cx/2-ui.btnBackward->width()/2;
        ui.btnBackward->move(x, ui.btnReturn->y());
        ui.btnForward->move(x, cy-ui.btnReturn->y()-ui.btnForward->height());
    }
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

    if (0 == m_cxImg)
    {
        return;
    }

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    UINT cxDst = rc.width();
    UINT cyDst = rc.height();

#define __margin __size(30)
    QRect rcPos;
    _genRect(m_cxImg, m_cyImg, cxDst, cyDst, rcPos);
    if (rcPos.x() < __margin && rcPos.y() < __margin)
    {
        _genRect(cxDst, cyDst, m_cxImg, m_cyImg, rcPos);
        painter.drawPixmap(rc, m_brush, rcPos);
        return;
    }

    if (m_cxImg + __margin*2 < cxDst && m_cyImg + __margin*2 < cyDst)
    {
        rcPos.setRect((cxDst - m_cxImg)/2, (cyDst - m_cyImg)/2, m_cxImg, m_cyImg);
    }
    else if (rcPos.x() < __margin)
    {
        cyDst = cxDst * m_cyImg / m_cxImg;
        rcPos.setRect(0, (rc.height()-cyDst)/2, cxDst, cyDst);
    }
    else if (rcPos.y() < __margin)
    {
        cxDst = cyDst * m_cxImg / m_cyImg;
        rcPos.setRect((rc.width()-cxDst)/2, 0, cxDst, cyDst);
    }

    painter.drawPixmap(rcPos, m_brush, QRect(0,0,m_cxImg,m_cyImg), __szRound);
}

void CSingerImgDlg::show(cwstr strSingerName)
{
    m_uImgCount = m_singerImgMgr.getSingerImgCount(strSingerName);
    if (0 == m_uImgCount)
    {
        return;
    }

    ui.btnBackward->setVisible(m_uImgCount>1);
    ui.btnForward->setVisible(m_uImgCount>1);

    m_strSingerName = strSingerName;

    m_uImgIdx = 0;

    m_cxImg = m_cyImg = 0;

    _showImg(0);

    CDialog::show([&]{
        m_brush = QBrush();
    });
}

void CSingerImgDlg::updateSingerImg()
{
    if (0 == m_cxImg)
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

    auto pSingerImg = m_singerImgMgr.getSingerImg(m_strSingerName, uImgIdx, false);
    if (NULL == pSingerImg || !pSingerImg->bExist)
    {
        return;
    }

    m_nSwitchingOffset = 0;

    QPixmap pm(__WS2Q(m_singerImgMgr.file(*pSingerImg)));
    m_cxImg = pm.width();
    m_cyImg = pm.height();
    m_brush = QBrush(pm);
    update();

    m_uImgIdx = uImgIdx;

    if (m_uImgCount > 1)
    {
        if (m_uImgIdx < m_uImgCount-1)
        {
            (void)m_singerImgMgr.getSingerImg(m_strSingerName, uImgIdx, false);
        }

        uImgIdx = (0 == m_uImgIdx) ? (m_uImgCount-1) : (m_uImgIdx-1);
        (void)m_singerImgMgr.getSingerImg(m_strSingerName, uImgIdx, false);
    }
}

void CSingerImgDlg::_onTouchEvent(E_TouchEventType eType, const CTouchEvent& te)
{
    if (0 == m_cxImg || m_uImgCount <= 1)
    {
        return;
    }

    if (eType != E_TouchEventType::TET_TouchEnd)
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

    if (te.x() < rect().center().x()-__size100)
    {
        _showImg(-1);
    }
    else
    {
        _showImg(1);
    }
}
