
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

wstring COuterDir::init(const wstring& strMediaLibDir)
{    
#if __windows
    for (cauto wch : strMediaLibDir)
    {
        if (fsutil::checkSeparator(wch))
        {
            m_strOuterDir.append(L"/..");
        }
    }

    m_strMediaLibDir = strMediaLibDir;
#else
    m_strMediaLibDir = fsutil::GetFileName(strMediaLibDir);

    m_strOuterDir = L"/..";
#endif

    cauto strAbsPath = strMediaLibDir + m_strOuterDir;
    CPath::setDir(strAbsPath);
    return strAbsPath;
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

    auto pSubDir = new COuterDir(fileInfo, m_strMediaLibDir);

#else
    if (fileInfo.strName == m_strMediaLibDir)
    {
        return NULL;
    }

    auto pSubDir = new CMediaDir(fileInfo);
#endif

    mtutil::usleep(1);

    if (NULL == m_fi.pParent)
    {
        if (!pSubDir->files())
        {
            if (!pSubDir->dirs().any([](CPath& childDir){
                return childDir.files() || childDir.dirs();
            }))
            {
                delete pSubDir;
                return NULL;
            }
        }
    }

    return pSubDir;
}

CMedialibDlg::CMedialibDlg(QWidget& parent, class CApp& app) : CDialog(parent)
  , m_app(app)
  , m_MedialibView(app, *this, m_OuterDir)
{
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
}

/*this->setGeometry(m_app.mainWnd().geometry());
_resizeTitle();
//__appAsync([&](){
//    _resizeTitle();
//});*/

#define __show() \
    m_OuterDir.init(m_app.getMediaLib().GetAbsPath()); \
    CDialog::show();

void CMedialibDlg::show()
{
    m_MedialibView.showRoot();

    __appAsync([&](){
        m_OuterDir.findFile();
    });

    __show();
}

void CMedialibDlg::showMediaSet(CMediaSet& MediaSet)
{
    m_MedialibView.showMediaSet(MediaSet);

    __show();
}

void CMedialibDlg::showMedia(CMedia& media)
{
    m_MedialibView.hittestMedia(media);

    __show();
}

bool CMedialibDlg::showFile(const wstring& strPath)
{
    CMediaRes *pMediaRes = m_app.getMediaLib().findSubFile(strPath);
    if (NULL == pMediaRes)
    {
        auto strOuterDir = m_OuterDir.init(m_app.getMediaLib().GetAbsPath());
        strOuterDir = fsutil::GetOppPath(m_app.getMediaLib().absPath(), strOuterDir);
        if (!fsutil::CheckSubPath(strOuterDir, strPath))
        {
            return false;
        }

        cauto t_strPath = __substr(strPath, strOuterDir.size());
        pMediaRes = (CMediaRes*)m_OuterDir.findSubFile(t_strPath);
        if(NULL == pMediaRes)
        {
            return false;
        }
    }
    else
    {
        m_OuterDir.init(m_app.getMediaLib().GetAbsPath());
    }

    m_MedialibView.hittestFile(*pMediaRes);

    CDialog::show();

    return true;
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
