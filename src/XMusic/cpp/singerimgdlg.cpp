#include "app.h"

#include "singerimgdlg.h"
#include "ui_singerimgdlg.h"

static Ui::SingerImgDlg ui;

CSingerImgDlg::CSingerImgDlg(CMedialibDlg& medialibDlg, CApp& app)
    : CDialog(medialibDlg)
    , m_singerImgMgr(app.getSingerImgMgr())
{
    ui.setupUi(this);
    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);
}

void CSingerImgDlg::relayout(cqrc rcBtnReturn)
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

    m_strSingerName = strSingerName;

    m_mapImg.clear();
    m_uImgIdx = 0;

    m_cxImg = m_cyImg = 0;

    _switchImg(0);

    CDialog::show([&](){
        m_brush = QBrush();
    });
}

void CSingerImgDlg::updateSingerImg()
{
    if (0 == m_cxImg)
    {
        m_uImgIdx = 0;
        _switchImg(0);
    }
    else if (m_nSwitchingOffset != 0)
    {
        _switchImg(m_nSwitchingOffset);
    }
}

void CSingerImgDlg::_switchImg(int nOffset)
{
    auto uImgIdx = m_uImgIdx;
    if (nOffset > 0)
    {
        uImgIdx++;
    }
    else if (nOffset < 0)
    {
        if (uImgIdx > 0)
        {
            uImgIdx--;
        }
        else
        {
            if (m_uImgCount <= 1)
            {
                return;
            }

            uImgIdx = m_uImgCount-1;
        }
    }

    m_nSwitchingOffset = nOffset;

    wstring strFile;
    cauto itr = m_mapImg.find(uImgIdx);
    if (itr != m_mapImg.end())
    {
        strFile = itr->second;
    }
    else
    {
        auto pSingerImg = m_singerImgMgr.getSingerImg(m_strSingerName, uImgIdx, false);
        if (NULL == pSingerImg)
        {
            //return;

            pSingerImg = m_singerImgMgr.getSingerImg(m_strSingerName, 0, false);
            uImgIdx = 0;
        }

        strFile = m_singerImgMgr.checkSingerImg(pSingerImg);
        if (strFile.empty())
        {
            return;
        }
    }

    m_nSwitchingOffset = 0;

    QPixmap pm(__WS2Q(strFile));
    m_cxImg = pm.width();
    m_cyImg = pm.height();
    m_brush = QBrush(pm);
    update();

    m_uImgIdx = uImgIdx;
    m_mapImg[m_uImgIdx] = strFile;

    if (m_uImgCount > 1)
    {
        if (m_uImgIdx < m_uImgCount-1)
        {
            uImgIdx = m_uImgIdx+1;
            if (m_mapImg.find(uImgIdx) == m_mapImg.end())
            {
                (void)m_singerImgMgr.checkSingerImg(m_strSingerName, uImgIdx, false);
            }
        }

        uImgIdx = (0 == m_uImgIdx) ? (m_uImgCount-1) : (m_uImgIdx-1);
        if (m_mapImg.find(uImgIdx) == m_mapImg.end())
        {
            (void)m_singerImgMgr.checkSingerImg(m_strSingerName, uImgIdx, false);
        }
    }
}

void CSingerImgDlg::_onTouchEvent(E_TouchEventType eType, const CTouchEvent& te)
{
    if (0 == m_cxImg)
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
                _switchImg(-1);
                return;
            }
            else if (dx <= -3)
            {
                _switchImg(1);
                return;
            }
        }
        else
        {
            if (dy >= 3)
            {
                _switchImg(-1);
                return;
            }
            else if (dy <= -3)
            {
                _switchImg(1);
                return;
            }
        }
    }

    if (te.x() < rect().center().x()-__size(100))
    {
        _switchImg(-1);
    }
    else
    {
        _switchImg(1);
    }
}
