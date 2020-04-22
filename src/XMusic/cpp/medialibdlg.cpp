
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

wstring COuterDir::init()
{
     cauto strMediaLibDir = __medialib.path();
#if __windows
    for (auto wch : strMediaLibDir)
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
    return m_strOuterDir;
}

CPath* COuterDir::_newSubDir(const tagFileInfo& fileInfo)
{
#if __windows
    wstring strSubDir = this->path() + __wcPathSeparator + fileInfo.strName;
    QString qsSubDir = QDir(__WS2Q(strSubDir)).absolutePath();
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
  , m_lv(*this, app, m_OuterDir)
  , m_wholeTrackDlg(*this, app)
{
}

void CMedialibDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    m_lv.setFont(1.04, QFont::Weight::Normal);
    m_lv.init();

    ui.frameFilterLanguage->setAttribute(Qt::WA_TranslucentBackground);

    SList<CLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK
                , ui.labelDemandKR, ui.labelDemandEN, ui.labelDemandEUR};
    for (auto label : lstLabels)
    {
        label->setFont(1.055, QFont::Weight::Normal, false, true);

        connect(label, &CLabel::signal_click, this, &CMedialibDlg::slot_labelClick);
    }

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_lv.upward();
    });

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_lv.play();
    });
}

/*this->setGeometry(m_app.mainWnd().geometry());
_resizeTitle();
//__appAsync([&](){
//    _resizeTitle();
//});*/

void CMedialibDlg::show()
{
    m_lv.showRoot();

    __appAsync([&](){
        m_OuterDir.findFile();
    });

    m_OuterDir.init();
    CDialog::show();
}

void CMedialibDlg::showMediaSet(CMediaSet& MediaSet)
{
    m_lv.showMediaSet(MediaSet);

    m_OuterDir.init();
    CDialog::show();
}

void CMedialibDlg::showMedia(CMedia& media)
{
    m_lv.hittestMedia(media);

    m_OuterDir.init();
    CDialog::show();
}

bool CMedialibDlg::showMediaRes(const wstring& strPath)
{
    cauto strOuterDir = m_OuterDir.init();

    CMediaRes *pMediaRes = __medialib.findSubFile(strPath);
    if (NULL == pMediaRes)
    {
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
    m_lv.hittestFile(*pMediaRes);

    __appAsync([=]() {
        tryShowWholeTrack(*pMediaRes);
    });

    CDialog::show();

    return true;
}

size_t CMedialibDlg::getPageRowCount(int cy)
{
    UINT uRet = 10;
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
    int sz = cy/(1.1+getPageRowCount(cy));
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

    auto& frameFilterLanguage = *ui.frameFilterLanguage;
    frameFilterLanguage.setGeometry(cx-frameFilterLanguage.width()-xMargin
                                    , rcReturn.center().y()-frameFilterLanguage.height()/2
                                    , frameFilterLanguage.width(), frameFilterLanguage.height());

    int x_btnPlay = cx - __playIconMagin - rcReturn.width();
    ui.btnPlay->setGeometry(x_btnPlay, rcReturn.top(), rcReturn.width(), rcReturn.height());

    _resizeTitle();

    int y_MedialibView = rcReturn.bottom() + rcReturn.top();
    m_lv.setGeometry(0, y_MedialibView, cx, cy-y_MedialibView);

    m_wholeTrackDlg.relayout(ui.btnReturn->geometry(), ui.btnPlay->geometry(), m_lv.geometry());
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

void CMedialibDlg::updateHead(const WString& strTitle)
{
    E_LabelTextOption lto = E_LabelTextOption::LTO_AutoFit;
    bool bShowFilterLanguage =  false;
    bool bShowUpwardButton = false;
    bool bShowPlayButton = false;

    auto pDir = m_lv.currentDir();
    if (pDir)
    {
        lto = E_LabelTextOption::LtO_Elided;
        bShowUpwardButton = true;
        bShowPlayButton = pDir->files();
    }
    else
    {
        auto pMediaSet = m_lv.currentMediaSet();
        if (pMediaSet)
        {
            bShowUpwardButton = true;
            bShowPlayButton = E_MediaSetType::MST_Singer==pMediaSet->m_eType
                    || E_MediaSetType::MST_Album==pMediaSet->m_eType
                    || E_MediaSetType::MST_Playlist==pMediaSet->m_eType;

            if (&m_app.getPlaylistMgr() == pMediaSet)
            {
                bShowFilterLanguage = true;
            }
        }
    }

    ui.labelTitle->setText(strTitle, lto);

    ui.frameFilterLanguage->setVisible(bShowFilterLanguage);

    ui.btnPlay->setVisible(bShowPlayButton);

/*#if __android// || __ios
    bShowUpwardButton = false;
#endif*/
    ui.btnUpward->setVisible(bShowUpwardButton);

    _resizeTitle();
}

void CMedialibDlg::_onClosed()
{
    m_lv.clear();

    m_OuterDir.clear();
}

void CMedialibDlg::slot_labelClick(CLabel *label, const QPoint&)
{
    static map<CLabel*, E_LanguageType> mapLabels {
        {ui.labelDemandCN, E_LanguageType::LT_CN}
        , {ui.labelDemandHK, E_LanguageType::LT_HK}
        , {ui.labelDemandKR, E_LanguageType::LT_KR}
        , {ui.labelDemandEN, E_LanguageType::LT_EN}
        , {ui.labelDemandEUR, E_LanguageType::LT_EUR}
    };
    auto uLanguage = (UINT)mapLabels[label];

    cauto paMediaSet = m_lv.currentSubSets();
    //paMediaSet.get((UINT)m_MedialibView.scrollPos(), [&](const CMediaSet& MediaSet){
    //    if (MediaSet.property().language() != uLanguage) {
            int nRow = paMediaSet.find([&](const CMediaSet& MediaSet){
                return MediaSet.property().language() & uLanguage;
            });
            if (nRow >= 0)
            {
                m_lv.scroll(nRow);
            }
    //    }
    //});
}
