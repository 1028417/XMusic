#include "app.h"

#include "singerimgdlg.h"
#include "ui_singerimgdlg.h"

static Ui::SingerImgDlg ui;

CSingerImgDlg::CSingerImgDlg(CMedialibDlg& medialibDlg, CApp& app)
    : CDialog(medialibDlg)
    , m_medialibDlg(medialibDlg)
    , m_app(app)
{
}

void CSingerImgDlg::_relayout(int cx, int cy)
{

}
