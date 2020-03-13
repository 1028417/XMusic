
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

void COuterDir::setDir(const wstring& strMediaLibDir, const wstring& strOuterDir)
{
    m_strOuterDir = strOuterDir;
    CPath::setDir(strMediaLibDir + strOuterDir);

#if __windows
    m_strMediaLibDir = strMediaLibDir;
#else
    m_strMediaLibDir = fsutil::GetFileName(strMediaLibDir);
#endif
}

CPath* COuterDir::_newSubDir(const tagFileInfo& fileInfo)
{
#if __windows
    wstring strSubDir = this->absPath() + __wcPathSeparator + fileInfo.strName;
    QString qsSubDir = QDir(strutil::toQstr(strSubDir)).absolutePath();
    strSubDir = QDir::toNativeSeparators(qsSubDir).toStdWString();
    if (strutil::matchIgnoreCase(strSubDir, m_strMediaLibDir))
    {
        return NULL;
    }

    return new COuterDir(fileInfo, m_strMediaLibDir);

#else
    if (fileInfo.strName == m_strMediaLibDir)
    {
        return NULL;
    }

    auto pSubDir = new CMediaDir(fileInfo);
#if __android
    if (pSubDir->dirs().empty() && pSubDir->files().empty())
    {
        delete pSubDir;
    }
#endif
    return pSubDir;
#endif
}

CMediaRes* COuterDir::findSubFile(const wstring& strSubFile)
{
    if (__substr(strSubFile, 1, 2) != L"..")
    {
        return NULL;
    }

    cauto t_strSubFile = __substr(strSubFile, m_strOuterDir.size());
    //strutil::replace_r(strutil::replace_r(strSubFile, L"/.."), L"\\..");

    return CMediaDir::findSubFile(t_strSubFile);
}

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
        if (fsutil::checkSeparator(wch))
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

    ui.labelTitle->setFont(1.18, QFont::Weight::DemiBold);

    m_MedialibView.setFont(1.05, QFont::Weight::Normal);
    m_MedialibView.init();

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_MedialibView.upward();
    });

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_MedialibView.play();
    });

    _initOuter();
}

void CMedialibDlg::_show()
{
    __appAsync([&](){
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

    int x_btnPlay = cx - __playIconMagin - rcReturn.width();
    ui.btnPlay->setGeometry(x_btnPlay, rcReturn.top(), rcReturn.width(), rcReturn.height());

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

void CMedialibDlg::updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton, bool bAutoFitText)
{
    ui.labelTitle->setText(strutil::toQstr(strTitle), bAutoFitText?
                               E_LabelTextOption::LTO_AutoFit:E_LabelTextOption::LtO_Elided);

    ui.btnPlay->setVisible(bShowPlayButton);

/*#if __android// || __ios
    bShowUpwardButton = false;
#endif*/
    ui.btnUpward->setVisible(bShowUpwardButton);

    _resizeTitle();
}

void CMedialibDlg::_onClosed()
{
    m_MedialibView.clear();

    m_OuterDir.clear();
}
