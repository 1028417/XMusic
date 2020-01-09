
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(QWidget& parent, class CApp& app) : CDialog(parent)
  , m_app(app)
  , m_MedialibView(app, *this, m_OuterDir)
{
}

void CMedialibDlg::_initOuter()
{
    cauto strMediaLibDir = m_app.getMediaLib().GetAbsPath();
    wstring strOuterDir;
#if __windows
    for (cauto wch : strMediaLibDir)
    {
        if (fsutil::checkPathTail(wch))
        {
            strOuterDir.append(L"/..");
        }
    }

#else
    strOuterDir = L"/..";
#endif
    m_OuterDir.setDir(strMediaLibDir, strOuterDir);
}

void CMedialibDlg::init()
{
    ui.setupUi(this);

    _initOuter();

#if __android || __ios
    m_MedialibView.setFont(1.05);
#endif
    m_MedialibView.init();

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_MedialibView.upward();
    });

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_MedialibView.play();
    });
}

void CMedialibDlg::_show()
{
    ui.labelTitle->setFont(1.15, QFont::Weight::DemiBold);

    CApp::async([&](){
        _resizeTitle();
    });

    CDialog::show();
}

size_t CMedialibDlg::getPageRowCount()
{
    UINT uRet = 10;
    int cy = height();
    /*if (cy >= __size(2560))
    {
       uRet++;
    }
    else*/ if (cy < __size(1800))
    {
        //uRet--;
        uRet = ceil((float)uRet*cy/__size(1800));
    }

    return uRet;
}

void CMedialibDlg::_relayout(int cx, int cy)
{
    int sz = cy/(1.1+getPageRowCount());
    int xMargin = sz/4;
//    if (cy < cx)
//    {
//#define __szOffset __size(6)
//        xMargin += __szOffset;
//        yReturn -= __szOffset;
//    }
    QRect rcReturn(xMargin, xMargin, sz-xMargin*2, sz-xMargin*2);

    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }

    ui.btnReturn->setGeometry(rcReturn);

    ui.btnUpward->setGeometry(rcReturn.right() + xMargin/2, rcReturn.top(), rcReturn.width(), rcReturn.height());

    ui.btnPlay->setGeometry(cx-rcReturn.right(), rcReturn.top(), rcReturn.width(), rcReturn.height());

    _resizeTitle();

    int y_MedialibView = rcReturn.bottom() + rcReturn.top();
    m_MedialibView.setGeometry(0, y_MedialibView, cx, cy-y_MedialibView);
}

void CMedialibDlg::_resizeTitle() const
{
    int xMargin = ui.btnReturn->x();
    auto pButton = ui.btnUpward->isVisible() ? ui.btnUpward : ui.btnReturn;
    int x_title = pButton->geometry().right() + xMargin;

    int cx_title = 0;
    if (ui.btnPlay->isVisible())
    {
        cx_title = ui.btnPlay->x() - xMargin - x_title;
    }
    else
    {
        cx_title = width() - xMargin - x_title;
    }

    ui.labelTitle->setGeometry(x_title, ui.btnReturn->y(), cx_title, ui.btnReturn->height());
}

void CMedialibDlg::updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton)
{
    ui.labelTitle->setText(strutil::toQstr(strTitle));

    ui.btnPlay->setVisible(bShowPlayButton);

/*#if __android// || __ios
    bShowUpwardButton = false;
#endif*/
    ui.btnUpward->setVisible(bShowUpwardButton);

    _resizeTitle();
}

void CMedialibDlg::_onClose()
{
    m_MedialibView.clear();

    m_OuterDir.clear();
}
