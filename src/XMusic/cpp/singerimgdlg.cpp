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
        cauto strFile = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, m_uSingerImgIdx);
        if (strFile.empty())
        {
            return;
        }

        if (!m_pm.load(__WS2Q(strFile)))
        {
            return;
        }
    }

    painter.drawPixmapEx(rc, m_pm);
}
