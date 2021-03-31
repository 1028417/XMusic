
#include "xmusic.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#define __thumbsRetain 6

#define __thumbs L".thumbs"
#define __thumbsFormat "JPG"

static Ui::BkgDlg ui;

template <class T=QPixmap>
static void _zoomoutBkg(T& pm, bool bHLayout, int szMax, int szMin, bool bCut)
{
    int cx = szMax;
    int cy = szMin;
    if (!bHLayout)
    {
        std::swap(cx, cy);
    }
    zoomoutPixmap(pm, cx, cy, bCut);
}

#define __szScreenMax __size(__cyBkg)
#define __szScreenMin __size(1200)

#define __thumbsRate 0.4f

template <class T=QPixmap>
inline static void _saveThumbs(T& pm, bool bHLayout, cwstr strFile)
{
    int szMax = MAX(g_screen.nMaxSide, __szScreenMax);
    int szMin = MAX(g_screen.nMinSide, __szScreenMin);
    _zoomoutBkg(pm, bHLayout, szMax * __thumbsRate, szMin * __thumbsRate, true);
    pm.save(__WS2Q(strFile + __thumbs), __thumbsFormat);
}

CBkgDlg::CThumbsBrush& CBkgDlg::_genThumbs(TD_Bkg& pm, bool bHLayout)
{
    _zoomoutBkg(pm, bHLayout, g_screen.nMaxSide * __thumbsRate, g_screen.nMinSide * __thumbsRate, false);
    m_lstThumbsBrush.emplace_back(pm);
    return m_lstThumbsBrush.back();
}

CBkgDlg::CThumbsBrush& CBkgDlg::_loadThumbs(const WString& strFile, bool bHLayout)
{
    TD_Bkg pm;
    if (!pm.load(strFile + __thumbs, __thumbsFormat))
    {
        pm.load(strFile);
        _saveThumbs(pm, bHLayout, strFile);
        return _genThumbs(pm, bHLayout);
    }

    m_lstThumbsBrush.emplace_back(pm);
    return m_lstThumbsBrush.back();
}

CBkgDlg::CBkgDlg(QWidget& parent) : CDialog(parent),
    m_option(__app.getOption()),
    m_lv(*this),
    m_addbkgDlg(*this),
    m_colorDlg(*this)
{
}

void CBkgDlg::init()
{
    m_colorDlg.init();
    m_addbkgDlg.init();

    m_lv.init();

    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnColor, &CButton::signal_clicked, [&]{
        m_colorDlg.show();
    });
}

void CBkgDlg::preInit()
{
    mtutil::concurrence([&]{
        _preinitBkg(true);
    }, [&]{
        _preinitBkg(false);
    });
}

void CBkgDlg::_preinitBkg(bool bHLayout)
{
    auto& strBkgDir = bHLayout?m_strHBkgDir:m_strVBkgDir;
    strBkgDir = g_strWorkDir + (bHLayout?L"/hbkg/":L"/vbkg/");
    if (!fsutil::existDir(strBkgDir))
    {
        (void)fsutil::createDir(strBkgDir);

#if __android // 安卓背景图片迁移
        cauto strOrgDir = __app.getModel().androidOrgPath(bHLayout?L"hbkg/":L"vbkg/");
        fsutil::findSubFile(strOrgDir, [&](tagFileInfo& fi){
            fsutil::copyFile(strOrgDir + fi.strName, strBkgDir + fi.strName);
        });
#endif
    }

    wstring strAppBkgDir = strBkgDir + __app.appVersion();
    if (!fsutil::existDir(strAppBkgDir))
    {
        strAppBkgDir.push_back('-');
        (void)fsutil::createDir(strAppBkgDir);

#if __android
        wstring strBkgSrc = L"assets:";
#else
        wstring strBkgSrc = __app.applicationDirPath().toStdWString();
#endif

        vector<const wchar_t*> lstSubDir {
            L"/bkg/", bHLayout?L"hbkg/":L"vbkg/", L"city/"
        };

        QPixmap pm;
        for (UINT uIdx = 0; uIdx < lstSubDir.size(); uIdx++)
        {
            strBkgSrc.append(lstSubDir[uIdx]);

            for (wchar_t wch = L'a'; wch <= L'z'; wch++)
            {
                auto strBkg = strBkgSrc + wch + L".jpg";
                auto strDstFile = strAppBkgDir + L'/' + wchar_t('0'+uIdx) + L'.' + wch;
                if (pm.load(__WS2Q(strBkg)))
                {
                    _saveThumbs(pm, bHLayout, strDstFile); // 确保先生成缩略图
                    fsutil::copyFile(strBkg, strDstFile);
                    if (!usleepex(10))
                    {
                        return;
                    }
                }
            }
        }

        auto t_strAppBkgDir = strAppBkgDir;
        strAppBkgDir.pop_back();

        QFile::rename(__WS2Q(t_strAppBkgDir), __WS2Q(strAppBkgDir));
    }

    auto& vecBkgFile = bHLayout?m_vecHBkgFile:m_vecVBkgFile;
    fsutil::findSubFile(strAppBkgDir, [&](tagFileInfo& fi) {
        if (strutil::endWith(fi.strName, __thumbs))
        {
            return;
        }

        vecBkgFile.emplace_back(false, __app.appVersion() + __wcPathSeparator + fi.strName);
    });

    fsutil::findFile(strBkgDir, [&](tagFileInfo& fi) {
        if (fi.bDir)
        {
            if (fi.strName != __app.appVersion())
            {
                (void)fsutil::removeDirTree(strBkgDir + fi.strName);
            }
        }
        else
        {
            if (strutil::endWith(fi.strName, __thumbs))
            {
                return;
            }

            vecBkgFile.insert(vecBkgFile.begin(), tagBkgFile(false, fi.strName));
        }
    });

    for (auto itr = vecBkgFile.begin(); itr != vecBkgFile.end()
         && itr-vecBkgFile.begin() < __thumbsRetain; ++itr)
    {
        cauto strFile = strBkgDir + itr->strFile;
        itr->br = &_loadThumbs(strFile, bHLayout);

        if (!usleepex(10))
        {
            return;
        }
    }

    cauto strBkg = bHLayout?m_option.strHBkg:m_option.strVBkg;
    if (!strBkg.empty())
    {
        (bHLayout?m_pmHBkg:m_pmVBkg).load(__WS2Q(strBkgDir + strBkg));
    }
}

size_t CBkgDlg::caleRowCount(int cy)
{
    /*UINT uRowCount = 9;
    if (cy >= __size(__cyBkg))
    {
       uRowCount++;
    }
    else if (cy <= __size(1920))
    {
        uRowCount--;
        if (cy < __size(1800))
        {
            uRowCount = round((float)uRowCount*cy/__size(1800));
        }
    }*/

    return round(10.0f*cy/__size(2160));
}

void CBkgDlg::_relayout(int cx, int cy)
{
    int sz = MAX(cx, cy)/(CBkgDlg::caleRowCount(MAX(cx, cy))+1.6f);
    int cyMargin = sz/4;
    int cxMargin = cyMargin;
    QRect rcReturn(cxMargin, cyMargin, sz-cxMargin*2, sz-cyMargin*2);
    UINT cyBangs = checkIPhoneXBangs(cx, cy);
    if (cyBangs)
    {
        rcReturn.moveTop(cyBangs - cyMargin);
    }
    else
    {
        UINT cyStatusBar = checkAndroidStatusBar();
        if (cyStatusBar)
        {
            rcReturn.moveTop(cyMargin + cyStatusBar);
        }
    }
    ui.btnReturn->setGeometry(rcReturn);

    auto szBtn = rcReturn.width();
    m_lv.setButtonSize(szBtn-__size(5));

    if (cx > cy)
    {
        ui.btnColor->setGeometry(cxMargin, cy - cxMargin - szBtn, szBtn, szBtn);

        int x_bkgView = rcReturn.right()+cxMargin;
        int cx_bkgView = cx-x_bkgView;
        int cy_bkgView = cx_bkgView*cy/cx;
        m_lv.setGeometry(x_bkgView - m_lv.margin()/2
                              , (cy-cy_bkgView)/2, cx_bkgView, cy_bkgView);

        ui.labelTitle->setGeometry(0, rcReturn.bottom(), x_bkgView, ui.btnColor->y()- rcReturn.bottom());
        ui.labelTitle->setWordWrap(true);
        ui.labelTitle->setText("设\n置\n背\n景");
    }
    else
    {
        ui.btnColor->setGeometry(cx-cxMargin-szBtn, rcReturn.top(), szBtn, szBtn);

        int y_bkgView = rcReturn.bottom() + cyMargin;
        int cy_bkgView = cy-y_bkgView;
        int cx_bkgView = cy_bkgView*cx/cy;
        m_lv.setGeometry((cx-cx_bkgView)/2, y_bkgView, cx_bkgView, cy_bkgView);

        ui.labelTitle->setGeometry(0, rcReturn.top(), cx, rcReturn.height());
        ui.labelTitle->setText("设置背景");
    }

    static BOOL bHLayout = -1;
    if (bHLayout != (BOOL)m_bHLayout)
    {
        bHLayout = m_bHLayout;
        m_lv.scroll(0);
    }
}

size_t CBkgDlg::bkgCount() const
{
    return (m_bHLayout?m_vecHBkgFile:m_vecVBkgFile).size();
}

CBkgDlg::CThumbsBrush* CBkgDlg::thumbsBrush(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx >= vecBkgFile.size())
    {
        return NULL;
    }

    auto& bkgFile = vecBkgFile[uIdx];
    if (bkgFile.br)
    {
        return bkgFile.br;
    }

    cauto strFile = _bkgDir() + bkgFile.strFile;
    bkgFile.br = &_loadThumbs(strFile, m_bHLayout);
    return bkgFile.br;
}

bool CBkgDlg::_setBkg(int nIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (nIdx >= (int)vecBkgFile.size())
    {
        return false;
    }

    auto& pm = m_bHLayout?m_pmHBkg:m_pmVBkg;
    if (nIdx >= 0)
    {
        cauto strFile = vecBkgFile[nIdx].strFile;
        (void)pm.load(_bkgDir() + strFile);
        _updateBkg(strFile);
    }
    else
    {
        pm = QPixmap();
        _updateBkg(L"");
    }

    return true;
}

void CBkgDlg::_updateBkg(cwstr strFile)
{
    m_option.bUseBkgColor = false;
    if (m_bHLayout)
    {
        m_option.strHBkg = strFile;
    }
    else
    {
        m_option.strVBkg = strFile;
    }

    __app.mainWnd().updateBkg();
}

void CBkgDlg::handleLVClick(size_t uItem)
{
    if (0 == uItem)
    {
        m_addbkgDlg.show();
    }
    else if (1 == uItem)
    {
        _setBkg(-1);
        close();
    }
    else
    {
        uItem-=2;
        if (_setBkg(uItem))
        {
            close();
        }
        else
        {
            m_addbkgDlg.show();
        }
    }
}

void CBkgDlg::addBkg(cwstr strFile)
{
    auto& pmBkg = m_bHLayout?m_pmHBkg:m_pmVBkg;
    (void)pmBkg.load(__WS2Q(strFile));
    int szMax = MAX(g_screen.nMaxSide, __szScreenMax);
    int szMin = MAX(g_screen.nMinSide, __szScreenMin);
    _zoomoutBkg(pmBkg, m_bHLayout, szMax, szMin, false);

    cauto strFileName = to_wstring(time(0));
    cauto strDstFile = _bkgDir() + strFileName;

    //auto pm = pmBkg.toImage();
    auto pm = pmBkg;
    _saveThumbs(pm, m_bHLayout, strDstFile); // 确保先生成缩略图

    auto& vecBkgFile = _vecBkgFile();
    vecBkgFile.insert(vecBkgFile.begin(), tagBkgFile(false, strFileName, &_genThumbs(pm, m_bHLayout)));
    //update();

    pmBkg.save(__WS2Q(strDstFile), "JPG");

    _zoomoutBkg(pmBkg, m_bHLayout, g_screen.nMaxSide, g_screen.nMinSide, false);
    _updateBkg(strFileName);
}

void CBkgDlg::deleleBkg(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx < vecBkgFile.size())
    {
        cauto bkgFile = vecBkgFile[uIdx];

        cauto strBkg = m_bHLayout ? m_option.strHBkg : m_option.strVBkg;
        if (strBkg == bkgFile.strFile)
        {
            _setBkg(-1);
        }

        if (bkgFile.br)
        {
            for (auto itr = m_lstThumbsBrush.begin(); itr != m_lstThumbsBrush.end(); ++itr)
            {
                if (&*itr == bkgFile.br)
                {
                    m_lstThumbsBrush.erase(itr);
                    break;
                }
            }
        }

        cauto strFile = _bkgDir() + bkgFile.strFile;
        fsutil::removeFile(strFile);
        fsutil::removeFile(strFile + __thumbs);
        vecBkgFile.erase(vecBkgFile.begin()+uIdx);

        m_lv.update();
    }
}

void CBkgDlg::switchBkg(bool bHLayout, bool bNext)
{
    m_bHLayout = bHLayout;
    cauto vecBkgFile = _vecBkgFile();

    cauto strFile = bHLayout ? m_option.strHBkg : m_option.strVBkg;
    int nIdx = -1;
    if (!strFile.empty())
    {
        for (UINT uIdx = 0; uIdx < vecBkgFile.size(); uIdx++)
        {
            if (vecBkgFile[uIdx].strFile == strFile)
            {
                nIdx = uIdx;
                break;
            }
        }
    }

    if (!m_option.bUseBkgColor)
    {
        if (bNext)
        {
            nIdx++;
            if (nIdx >= (int)vecBkgFile.size())
            {
                nIdx = -1;
            }
        }
        else
        {
            if (-1 == nIdx)
            {
                nIdx = vecBkgFile.size()-1;
            }
            else
            {
                nIdx--;
            }
        }
    }

    _setBkg(nIdx);
}

void CBkgDlg::_onClosed()
{
    set<CThumbsBrush*> setDeleleBrush;
    if (m_vecHBkgFile.size() > __thumbsRetain)
    {
        for (auto itr = m_vecHBkgFile.begin()+__thumbsRetain; itr != m_vecHBkgFile.end(); ++itr)
        {
            setDeleleBrush.insert(itr->br);
            itr->br = NULL;
        }
    }

    if (m_vecVBkgFile.size() > __thumbsRetain)
    {
        for (auto itr = m_vecVBkgFile.begin()+__thumbsRetain; itr != m_vecVBkgFile.end(); ++itr)
        {
            setDeleleBrush.insert(itr->br);
            itr->br = NULL;
        }
    }

    for (auto itr = m_lstThumbsBrush.begin(); itr != m_lstThumbsBrush.end(); )
    {
        if (setDeleleBrush.find(&*itr) != setDeleleBrush.end())
        {
           itr = m_lstThumbsBrush.erase(itr);
        }
        else
        {
            ++itr;
        }
    }

    m_lv.reset();
}
