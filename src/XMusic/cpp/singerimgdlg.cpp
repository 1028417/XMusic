#include "app.h"

#include "singerimgdlg.h"
#include "ui_singerimgdlg.h"

static Ui::SingerImgDlg ui;

CSingerImgDlg::CSingerImgDlg(CMedialibDlg& medialibDlg, CApp& app)
    : CDialog(medialibDlg)
    , m_medialibDlg(medialibDlg)
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

    if (m_pm.isNull())
    {
        return;
    }

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    auto cxSrc = m_pm.width();
    auto cySrc = m_pm.height();

    auto cxDst = rc.width();
    auto cyDst = rc.height();

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

    painter.drawPixmap(QRect(xDst, yDst, cxDst, cyDst), m_pm, QRect(0, 0, cxSrc, cySrc));
}

void CSingerImgDlg::show(cwstr strSingerName)
{
    m_strSingerName = strSingerName;
    m_uSingerImgIdx = 0;
    _switchImg(0);

    CDialog::show([&](){
        m_pm = QPixmap();
    });
}

void CSingerImgDlg::updateSingerImg()
{
    if (m_pm.isNull())
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
        if (0 == uSingerImgIdx)
        {
            return;
        }
        uSingerImgIdx--;
    }

    auto strFile = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, uSingerImgIdx, false);
    if (strFile.empty())
    {
        return;
    }

    strFile = m_app.getSingerImgMgr().dir() + strFile;
    if (!fsutil::existFile(strFile))
    {
        return;
    }

    m_uSingerImgIdx = uSingerImgIdx;
    (void)m_pm.load(__WS2Q(strFile));
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
