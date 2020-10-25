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

    int xDst = 0, yDst = 0;
    if (m_cxImg <= cxDst && m_cyImg <= cyDst)
    {
        xDst = (cxDst - m_cxImg)/2;
        yDst = (cyDst - m_cyImg)/2;
        cxDst = m_cxImg;
        cyDst = m_cyImg;
    }
    else
    {
        auto fHWRate = (float)m_cyImg/m_cxImg;
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

    painter.drawPixmap(QRect(xDst, yDst, cxDst, cyDst), m_brush, QRect(0, 0, m_cxImg, m_cyImg), __szRound);
}

void CSingerImgDlg::show(cwstr strSingerName)
{
    m_strSingerName = strSingerName;
    m_uSingerImgIdx = 0;
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
        m_uSingerImgIdx = 0;
        _switchImg(0);
    }
}

void CSingerImgDlg::_switchImg(int nOffset)
{
    auto uSingerImgIdx = m_uSingerImgIdx;
    if (nOffset > 0)
    {
        uSingerImgIdx++;
    }
    else if (nOffset < 0)
    {
        if (uSingerImgIdx > 0)
        {
            uSingerImgIdx--;
        }
        else
        {
            cauto mapFile = m_app.getSingerImgMgr().fileMap();
            cauto itr = mapFile.find(m_strSingerName);
            if (itr != mapFile.end())
            {
                uSingerImgIdx = itr->second.size();
                while (uSingerImgIdx > 1)
                {
                    uSingerImgIdx--;

                    auto strFile = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, uSingerImgIdx, false);
                    if (!strFile.empty())
                    {
                        break;
                    }
                }
            }

            if (0 == uSingerImgIdx)
            {
                return;
            }
        }
    }

    auto strFile = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, uSingerImgIdx, false);
    if (strFile.empty())
    {
        //return;

        strFile = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, 0, false);
        uSingerImgIdx = 0;
    }

    strFile = m_app.getSingerImgMgr().dir() + strFile;
    if (!fsutil::existFile(strFile))
    {
        return;
    }

    m_uSingerImgIdx = uSingerImgIdx;

    QPixmap pm(__WS2Q(strFile));
    m_cxImg = pm.width();
    m_cyImg = pm.height();
    m_brush = QBrush(pm);
    update();
}

void CSingerImgDlg::_onTouchEvent(E_TouchEventType eType, const CTouchEvent& te)
{
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
