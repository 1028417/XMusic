#include "app.h"

#include "singerimgdlg.h"
#include "ui_singerimgdlg.h"

static Ui::SingerImgDlg ui;

CSingerImgDlg::CSingerImgDlg(CMedialibDlg& medialibDlg, CApp& app)
    : CDialog(medialibDlg)
    , m_app(app)
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
    if (cxSrc <= cxDst && cySrc <= cyDst)
    {
        xDst = (cxDst - cxSrc)/2;
        yDst = (cyDst - cySrc)/2;
        cxDst = cxSrc;
        cyDst = cySrc;
    }
    else
    {
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

    QRect rcPos;
    if (m_cxImg+__size(30) > cxDst  && m_cyImg+__size(30) > cyDst)
    {
        _genRect(cxDst, cyDst, m_cxImg, m_cyImg, rcPos);
        painter.drawPixmap(QRect(0,0,cxDst,cxDst), m_brush, rcPos);
    }
    else
    {
        _genRect(m_cxImg, m_cyImg, cxDst, cyDst, rcPos);
        painter.drawPixmap(rcPos, m_brush, QRect(0,0,m_cxImg,m_cyImg), __szRound);
    }
}

void CSingerImgDlg::show(cwstr strSingerName)
{
    m_uImgCount = m_app.getSingerImgMgr().getSingerImgCount(strSingerName);
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
        auto pSingerImg = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, uImgIdx, false);
        if (NULL == pSingerImg)
        {
            //return;

            pSingerImg = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, 0, false);
            uImgIdx = 0;
        }

        strFile = m_app.getSingerImgMgr().checkSingerImg(pSingerImg);
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
                (void)m_app.getSingerImgMgr().checkSingerImg(m_strSingerName, uImgIdx, false);
            }
        }

        uImgIdx = (0 == m_uImgIdx) ? (m_uImgCount-1) : (m_uImgIdx-1);
        if (m_mapImg.find(uImgIdx) == m_mapImg.end())
        {
            (void)m_app.getSingerImgMgr().checkSingerImg(m_strSingerName, uImgIdx, false);
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
