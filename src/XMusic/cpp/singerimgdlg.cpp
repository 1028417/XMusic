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

void CSingerImgDlg::show(cwstr strSingerName)
{
    m_strSingerName = strSingerName;
    m_uSingerImgIdx = 0;

    CDialog::show([&](){
        m_pm = QPixmap();
    });
}

void CSingerImgDlg::_onPaint(CPainter& painter, cqrc rc)
{
    CDialog::_onPaint(painter, rc);

    if (m_pm.isNull())
    {
        auto strFile = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, m_uSingerImgIdx);
        if (strFile.empty())
        {
            return;
        }

        strFile = m_app.getSingerImgMgr().checkSingerImg(strFile);
        if (strFile.empty())
        {
            return;
        }

        if (!m_pm.load(__WS2Q(strFile)))
        {
            return;
        }
    }

    painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

    auto cxSrc = m_pm.width();
    auto cySrc = m_pm.height();
    auto fHWRate = (float)cxSrc/cySrc;

    int xDst = 0, yDst = 0;
    auto cxDst = rc.width();
    auto cyDst = rc.height();
    if (fHWRate > (float)cxDst/cyDst)
    {
        xDst = (cxDst - cyDst/fHWRate)/2;
        cxDst -= xDst*2;
    }
    else
    {
        yDst = (cyDst - cxDst*fHWRate)/2;
        cyDst -= yDst*2;
    }

    painter.drawPixmap(QRect(xDst, yDst, cxDst, cyDst), m_pm, QRect(0, 0, cxSrc, cySrc));
}
