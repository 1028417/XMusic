
#include "xmusic.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

extern XThread *g_thrGenSubImg;
extern UINT g_uMsScanYield;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg)
    : CDialog(bkgDlg)
    , m_bkgDlg(bkgDlg)
    , m_lv(*this)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

#if __windows
    ui.labelChooseDir->setFont(1.08, QFont::Weight::Normal, false, true);

    connect(ui.labelChooseDir, &CLabel::signal_click, [&]{
        cwstr strDir = _chooseDir();
        if (strDir.empty())
        {
            return;
        }

        if (g_thrGenSubImg)
        {
            g_thrGenSubImg->cancel(false);
        }

        m_lv.thrScan().cancel();

        if (g_thrGenSubImg)
        {
            g_thrGenSubImg->cancel();
        }

        m_lv.scanDir(strDir);
        update();
    });
#endif

    m_lv.setFont(1.0f, QFont::Weight::Normal);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &CAddBkgDlg::_handleReturn);
}

/*#define __MediaFilter L"所有支持格式|*.Jpg;*.Jpeg;*.Png;*.Bmp|Jpg文件(*.Jpg)|*.Jpg|Jpeg文件(*.Jpeg)|*.Jpeg \
    |Png文件(*.Png)|*.Png|位图文件(*.Bmp)|*.Bmp|"
    tagFileDlgOpt FileDlgOpt;
    FileDlgOpt.strTitle = L"选择背景图";
    FileDlgOpt.strFilter = __MediaFilter;
    FileDlgOpt.hWndOwner = hwnd();
    CFileDlg fileDlg(FileDlgOpt);
    wstring strFile = fileDlg.ShowOpenSingle();
    if (!strFile.empty())
    {
        this->addBkg(strFile);
    }*/

#if __windows
wstring CAddBkgDlg::_chooseDir()
{
    auto& strAddBkgDir = __app.getOption().strAddBkgDir;

    CFolderDlg FolderDlg;
    cauto strDir = FolderDlg.Show(hwnd(), strAddBkgDir.c_str(), L" 请选择图片目录");
    if (strDir.empty())
    {
        return L"";
    }

    if (fsutil::MatchPath(strDir, strAddBkgDir))
    {
        return L"";
    }

    strAddBkgDir = strDir;

    return strAddBkgDir;
}
#endif

void CAddBkgDlg::show()
{
    g_uMsScanYield = 1;

    if (!m_lv.thrScan().joinable())
    {
        wstring strAddBkgDir;
#if __windows
        strAddBkgDir = __app.getOption().strAddBkgDir;
        if (strAddBkgDir.empty() || !fsutil::existDir(strAddBkgDir))
        {
            //strAddBkgDir = _chooseDir();
            //if (strAddBkgDir.empty())
            //{
                //return;
                strAddBkgDir = fsutil::getHomeDir().toStdWString();
            //}
        }
        mtutil::thread([&]{
            CFolderDlg::preInit();
        });

#elif __android
        if (!requestAndroidSDPermission())
        {
            return;
        }
        strAddBkgDir = __sdcardDir;

#else
        strAddBkgDir = fsutil::getHomeDir().toStdWString();
#endif
        m_lv.scanDir(strAddBkgDir);
    }

    UINT uDotCount = 0;
    timerutil::setTimerEx(240, [=]()mutable{
        if (!this->isVisible())
        {
            return false;
        }

        if (!m_lv.thrScan())
        {
            ui.labelTitle->setText("添加背景");
            return false;
        }

        QString qsTitle = "正在扫描";
        for (UINT uIdx = 1; uIdx <= uDotCount; uIdx++)
        {
            qsTitle.append('.');
        }
        ui.labelTitle->setText(qsTitle);
        if (++uDotCount > 3)
        {
            uDotCount = 0;
        }
        return true;
    });

    CDialog::show([&]{
        (void)m_lv.handleReturn(true);
    });
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    m_uRowCount = CBkgDlg::caleRowCount(cy);

    int sz = MAX(cx, cy)/(CBkgDlg::caleRowCount(MAX(cx, cy))+1.6f);
    int cxMargin = sz/4;
    QRect rcReturn(cxMargin, cxMargin, sz-cxMargin*2, sz-cxMargin*2);
    if (checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    ui.labelTitle->move(rcReturn.right() + cxMargin, rcReturn.center().y() - ui.labelTitle->height()/2);

    ui.labelChooseDir->setVisible(false);

    int y_addbkgView = 0;
    if (m_lv.displayMode())
    {
        ui.labelTitle->setVisible(false);
    }
    else
    {
        y_addbkgView = rcReturn.bottom() + rcReturn.top();
        ui.labelTitle->setVisible(true);

#if __windows
        if (m_lv.inRoot())
        {
            ui.labelChooseDir->setVisible(true);
            ui.labelChooseDir->move(cx-ui.labelChooseDir->width()-cxMargin
                    , rcReturn.center().y()-ui.labelChooseDir->height()/2);
        }
#endif
    }

    m_lv.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);

    /*static BOOL bHLayout = -1;
    if (bHLayout != (BOOL)m_bHLayout)
    {
        bHLayout = m_bHLayout;
        m_lv.scroll(m_lv.scrollPos());
    }*/
}

bool CAddBkgDlg::_handleReturn()
{
    if (m_lv.handleReturn(false))
    {
        relayout();
        repaint();
    }
    else
    {
        close();
    }
    return true;
}

void CAddBkgDlg::addBkg(cwstr strFile)
{
    static bool bFlag = false;
    if (bFlag)
    {
        return;
    }
    bFlag = true;

    m_bkgDlg.addBkg(strFile);

#if __windows
    this->close();
    m_bkgDlg.close();
#else
    m_bkgDlg.close();
    this->close();
#endif

    bFlag = false;
}


CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg) :
    CListView(&addbkgDlg, E_LVScrollBar::LVSB_Left)
    , m_addbkgDlg(addbkgDlg)
    , m_thrScan(__app.thread())
    , m_rootImgDir(m_thrScan.signal())
    , m_olBkgDir(m_thrScan.signal())
{
}

size_t CAddBkgView::getColCount() const
{
    if (m_pImgDir && m_pImgDir != &m_olBkgDir)
    {
        if (m_pImgDir->imgCount() <= 4)
        {
            return 2;
        }
        else
        {
            return 3;
        }
    }
    else
    {
        if (m_addbkgDlg.isHLayout() && getItemCount() > getRowCount())
        {
            return 2;
        }

        return 1;
    }
}

size_t CAddBkgView::getRowCount() const
{
    if (imgDir())
    {
        return getColCount();
    }

    return m_addbkgDlg.rowCount();
}

size_t CAddBkgView::getItemCount() const
{
    if (m_pImgDir == &m_olBkgDir)
    {
        return m_olBkgDir.count();
    }
    else if (m_pImgDir)
    {
        return m_pImgDir->imgCount();
    }
    else
    {
        return m_paImgDirs.size();
    }
}

void CAddBkgView::_paintRow(CPainter& painter, tagLVItem& lvItem, IImgDir& imgDir)
{
    auto eStyle = E_LVItemStyle::IS_ForwardButton | E_LVItemStyle::IS_BottomLine;
    tagLVItemContext context(lvItem, eStyle);
    context.strText = imgDir.displayName();
    context.setIcon(imgDir.icon(), __size(-12));
    CListView::_paintRow(painter, context);
}

void CAddBkgView::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    if (m_pImgDir == &m_olBkgDir)
    {
        m_pImgDir->dirs().get(lvItem.uItem, [&](CPath& subDir){
            _paintRow(painter, lvItem, (IImgDir&)subDir);
        });
    }
    else if (m_pImgDir)
    {
        auto pm = m_pImgDir->img(lvItem.uItem);
        if (pm)
        {
            QRect rcFrame(lvItem.rc);
            painter.drawImgEx(rcFrame, *pm);

            rcFrame.setLeft(rcFrame.left()-1);
            rcFrame.setTop(rcFrame.top()-1);
            painter.drawRectEx(rcFrame, QColor(255,255,255,50));
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](IImgDir& imgDir){
            _paintRow(painter, lvItem, imgDir);
        });
    }
}

void CAddBkgView::_onItemClick(tagLVItem& lvItem, const QMouseEvent&)
{
    if (m_pImgDir == &m_olBkgDir)
    {
        m_olBkgDir.dirs().get(lvItem.uItem, [&](CPath& subDir){
            _showImgDir((IImgDir&)subDir);
        });
    }
    else if (m_pImgDir)
    {
        cauto strFilePath = m_pImgDir->imgPath(lvItem.uItem);
        if (!strFilePath.empty())
        {
            m_addbkgDlg.addBkg(strFilePath);
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](IImgDir& imgDir){
            _showImgDir(imgDir);
        });
    }
}

void CAddBkgView::_showImgDir(IImgDir& imgDir)
{    
    g_uMsScanYield = 10;

    _saveScrollRecord(NULL);

    m_eScrollBar = E_LVScrollBar::LVSB_None;

    m_pImgDir = &imgDir;
    for (auto pImgDir : m_paImgDirs)
    {
        if (pImgDir != m_pImgDir)
        {
            pImgDir->cleanup();
        }
    }

    m_addbkgDlg.relayout();
    m_addbkgDlg.repaint(); //update();

    m_pImgDir->genSubImgs(*this); //_genSubImgs();
}

/*void CAddBkgView::_genSubImgs()
{
    if (NULL == m_pImgDir)
    {
        return;
    }

    if (!m_pImgDir->genSubImgs())
    {
        return;
    }

    update();

    async(30, [&]{
        _genSubImgs();
    });
}*/

bool CAddBkgView::handleReturn(bool bClose)
{
    reset();

    bool bRet = false;
    if (m_pImgDir)
    {
        bRet = true;

        if (m_pImgDir != &m_olBkgDir)
        {
            if (g_thrGenSubImg)
            {
                g_thrGenSubImg->cancel(false);
            }
        }

        m_eScrollBar = E_LVScrollBar::LVSB_Left;
        scrollToItem(_scrollRecord(NULL));

        if (m_pImgDir->parent() == &m_olBkgDir)
        {
            m_pImgDir = &m_olBkgDir;
        }
        else
        {
            m_pImgDir = NULL;
        }
    }

    /*if (g_thrGenSubImg)
    {
        g_thrGenSubImg->join();
        delete g_thrGenSubImg;
        g_thrGenSubImg = NULL;
    }*/

    if (bClose)
    {
        for (auto pImgDir : m_paImgDirs)
        {
            pImgDir->cleanup();
        }

        g_uMsScanYield = 10;
    }
    else
    {
        g_uMsScanYield = 1;
    }
    return bRet;
}

void CAddBkgView::scanDir(cwstr strDir)
{
    m_paImgDirs.clear();

    cauto strOlBkgDir = g_strWorkDir + __wcPathSeparator + L"线上";
    fsutil::createDir(strOlBkgDir);
    m_olBkgDir.setDir(strOlBkgDir);
    m_paImgDirs.add(m_olBkgDir);

    static UINT s_uSequence = 0;
    m_thrScan.start([&, strDir](signal_t bRunSignal){
        _downloadBkg();

        auto uSequence = ++s_uSequence;
        m_rootImgDir.setDir(strDir);
        CPath::scanDir(bRunSignal, m_rootImgDir, [&, uSequence](CPath& dir, TD_XFileList&){
            if (this->imgDir() || !m_addbkgDlg.isVisible())
            {
                m_thrScan.usleep(300);
            }
            if (!bRunSignal)
            {
                return;
            }

            __app.sync([&, uSequence]{
                if (uSequence == s_uSequence)
                {
                    m_paImgDirs.add((CImgDir&)dir);
                    this->update();
                }
            });
        });
    });
}

#include "../../../Common2.1/3rd/curl/include/curl/curl.h"

void CAddBkgView::_downloadBkg()
{
    if (m_thrDownload)
    {
        return;
    }

    cauto lstOlBkg = __app.getModel().olBkg();
    for (cauto olBkg : lstOlBkg)
    {
        tagFileInfo fileInfo;
        fileInfo.pParent = &m_olBkgDir;
        fileInfo.strName = olBkg.catName;
        auto pDir = new COlBkgDir(m_thrDownload, fileInfo, olBkg);
        m_olBkgDir.tryAdd(*pDir);

        for (auto pFile : pDir->files())
        {
            if (!fsutil::existFile(pFile->path()))
            {
                m_mapOlBkg[pDir].push_back(pFile);
            }
        }
    }

    if (m_mapOlBkg.empty())
    {
        return;
    }

    m_thrDownload = &__app.thread();
    m_thrDownload->start([&](signal_t bRunSignal) {
        tagCurlOpt curlOpt(true);
        CDownloader downloader(curlOpt);

        auto& bkgDlg = __app.mainWnd().bkgDlg();
        while (bRunSignal)
        {
            auto itr = m_mapOlBkg.begin();
            auto pDir = itr->first;
            auto& lstFiles = itr->second;
            auto pFile = lstFiles.front();

            cauto strUrl = pDir->url(*pFile);
            CByteBuffer bbfBkg;
            int nRet = downloader.syncDownload(bRunSignal, strUrl, bbfBkg);
            if (!bRunSignal)
            {
                break;
            }

            if (0 == nRet)
            {
                cauto strFile = pFile->path();
                if (OFStream::writefilex(strFile, true, bbfBkg))
                {
                    __app.sync([&, pDir]{
                        m_olBkgDir.tryAdd(*pDir);
                        bkgDlg.update();
                    });
                }
            }
            else if (CURLcode::CURLE_COULDNT_RESOLVE_PROXY == nRet
                    || CURLcode::CURLE_COULDNT_RESOLVE_HOST == nRet
                    || CURLcode::CURLE_COULDNT_CONNECT == nRet)
            {
                if (!m_thrDownload->usleep(5000))
                {
                    break;
                }
                continue;
            }

            auto itrFile = std::find(lstFiles.begin(), lstFiles.end(), pFile);
            if (itrFile != lstFiles.end())
            {
                lstFiles.erase(itrFile);
                if (lstFiles.empty())
                {
                    itr = m_mapOlBkg.erase(itr);
                    if (itr == m_mapOlBkg.end())
                    {
                        break;
                    }
                }
            }
        }
    });
}
