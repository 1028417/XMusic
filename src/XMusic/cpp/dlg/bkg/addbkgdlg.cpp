
#include "xmusic.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg) : //CDialog(bkgDlg),
    m_bkgDlg(bkgDlg),
    m_lv(*this),
    m_loadingLabel(this)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    ui.btnReturn->onClicked(this, &CAddBkgDlg::_handleReturn);

#if __windows
    ui.labelChooseDir->onClicked([&]{
        cwstr strDir = _chooseDir();
        if (strDir.empty())
        {
            return;
        }

        m_lv.scanDir(strDir);
        update();
    });

    ui.labelChooseDir->setFont(1.08, TD_FontWeight::Normal, false, true);
#endif

    ui.labelTitle->setFont(__titleFontSize, TD_FontWeight::DemiBold);

    m_lv.adjustFont(TD_FontWeight::Normal);
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
    auto& strAddBkgDir = g_app.getOption().strAddBkgDir;

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

extern UINT g_uMsScanYield;

void CAddBkgDlg::show()
{
    g_uMsScanYield = 1;

    static bool s_bFlag = false;
    if (!s_bFlag)
    {
        s_bFlag = true;

        wstring strAddBkgDir;
#if __windows
        strAddBkgDir = g_app.getOption().strAddBkgDir;
        if (strAddBkgDir.empty() || !fsutil::existDir(strAddBkgDir))
        {
            //strAddBkgDir = _chooseDir();
            //if (strAddBkgDir.empty())
            //{
                //return;
                strAddBkgDir = fsutil::getHomeDir();
            //}
        }
        /*卡mtutil::thread([&]{
            CFolderDlg::preInit();
        });*/

#elif __android
        if (!requestAndroidSDPermission())
        {
            return;
        }
        strAddBkgDir = __sdcardDir;

#else
        strAddBkgDir = fsutil::getHomeDir();
#endif
        m_lv.scanDir(strAddBkgDir);
    }
    else
    {
        if (m_lv.thrScan())
        {
            showLoading(true);
        }
    }

    CDialog::show([&]{
        (void)m_lv.handleReturn(true);
    });
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    m_uRowCount = CBkgDlg::caleRowCount(cy);

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

    ui.labelTitle->move(rcReturn.right() + cxMargin, rcReturn.center().y() - ui.labelTitle->height()/2);
    if (m_lv.displayMode() == 0)
    {
        ui.labelTitle->setVisible(true);
        if (m_lv.inRoot())
        {
            ui.labelTitle->setText("添加背景");
        }
        else
        {
            ui.labelTitle->setText(__WS2Q(__olBkgDir));
        }
    }
    else
    {
        ui.labelTitle->setVisible(false);
    }

    ui.labelChooseDir->setVisible(false);

    int y_addbkgView = 0;
    if (!m_lv.displayMode())
    {
        y_addbkgView = rcReturn.bottom() + cyMargin;

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

    auto y_loadingLabel = y_addbkgView+(m_lv.height()-m_loadingLabel.height())/2;
    m_loadingLabel.move((cx-m_loadingLabel.width())/2, y_loadingLabel);

    /*static BOOL bHLayout = -1;
    if (bHLayout != (BOOL)m_bHLayout)
    {
        bHLayout = m_bHLayout;
        m_lv.scroll(m_lv.scrollPos());
    }*/
}

void CAddBkgDlg::showLoading(bool bShow)
{
    m_loadingLabel.show(bShow);
}

bool CAddBkgDlg::_handleReturn()
{
    if (m_lv.handleReturn(false))
    {
        _relayout(width(), height());
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
    , m_thrScan(g_app.thread())
    , m_rootImgDir(m_thrScan.signal())
{
    m_pmOlBkg.load(__png(olBkg));
}

size_t CAddBkgView::getColCount() const
{
    if (m_pImgDir && m_pImgDir != &m_olBkgDir)
    {
        if (m_pImgDir->bkgCount(isHLayout()) <= 4)
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
        if (isHLayout() && getItemCount() > getRowCount())
        {
            return 2;
        }

        return 1;
    }
}

size_t CAddBkgView::getRowCount() const
{
    if (m_pImgDir && m_pImgDir != &m_olBkgDir)
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
        return m_pImgDir->bkgCount(isHLayout());
    }
    else
    {
        return m_paImgDirs.size();
    }
}

void CAddBkgView::_onPaint(CPainter& painter, int cx, int cy)
{
    CListView::_onPaint(painter, cx, cy);

    if (NULL == m_pImgDir || m_pImgDir == &m_olBkgDir)
    {
        return;
    }

    auto uCol = getColCount(); //= m_pImgDir->bkgCount()>=9?3:2;
    auto uExpectCount = UINT(ceil(this->scrollPos())+uCol)*uCol;
    auto uCount = getItemCount();
    if (uCount < uExpectCount && m_pImgDir->downloading(isHLayout()))
    {
        showLoading(true);
    }
    else
    {
        showLoading(false);
    }
}

void CAddBkgView::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    if (m_pImgDir == &m_olBkgDir)
    {
        m_pImgDir->dirs().get(lvItem.uItem, [&](CPath& subDir){
            _paintRow(painter, lvItem, (CImgDir&)subDir);
        });
    }
    else if (m_pImgDir)
    {
        auto pImg = m_pImgDir->bkgImg(isHLayout(), lvItem.uItem);
        if (pImg)
        {
            QRect rcFrame(lvItem.rc);
            painter.drawImgEx(rcFrame, *pImg);

            rcFrame.setLeft(rcFrame.left()-1);
            rcFrame.setTop(rcFrame.top()-1);
            painter.drawRectEx(rcFrame, QColor(255,255,255,50));
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](CImgDir& imgDir){
            _paintRow(painter, lvItem, imgDir);
        });
    }
}

void CAddBkgView::_paintRow(CPainter& painter, tagLVItem& lvItem, CImgDir& imgDir)
{
    auto eStyle = E_LVItemStyle::IS_ForwardButton | E_LVItemStyle::IS_BottomLine;
    tagLVItemContext context(lvItem, eStyle);
    context.strText = imgDir.displayName();
    cqpm pm = (&imgDir == &m_olBkgDir)?m_pmOlBkg:imgDir.icon();
    context.setIcon(pm, __size(-12));
    CListView::_paintRow(painter, context);
}

void CAddBkgView::_onItemClick(tagLVItem& lvItem, const QMouseEvent&)
{
    if (m_pImgDir == &m_olBkgDir)
    {
        m_olBkgDir.dirs().get(lvItem.uItem, [&](CPath& subDir){
            _showImgDir((CImgDir&)subDir);
        });
    }
    else if (m_pImgDir)
    {
        cauto strFile = m_pImgDir->bkgFile(isHLayout(), lvItem.uItem);
        if (!strFile.empty())
        {
            m_addbkgDlg.addBkg(strFile);
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](CImgDir& imgDir) {
            _showImgDir(imgDir);
        });
    }
}

void CAddBkgView::_showImgDir(CImgDir& imgDir)
{    
    g_uMsScanYield = 10;

    _saveScrollRecord(NULL);

    m_pImgDir = &imgDir;
    if (m_pImgDir != &m_olBkgDir)
    {
        m_eScrollBar = E_LVScrollBar::LVSB_None;

        for (auto pDir : m_olBkgDir.dirs())
        {
            if (pDir != m_pImgDir)
            {
                ((CImgDir*)pDir)->cleanup();
            }
        }
        for (auto pImgDir : m_paImgDirs)
        {
            if (pImgDir != m_pImgDir)
            {
                pImgDir->cleanup();
            }
        }
    }

    m_addbkgDlg.onShowImgDir();
    m_addbkgDlg.repaint();

    if (m_pImgDir == &m_olBkgDir)
    {
        showLoading(m_olBkgDir.downloading());
    }
    else
    {
        showLoading(false);

        if (m_thrGenSubImg)
        {
            m_thrGenSubImg->cancel();
        }
        else
        {
            m_thrGenSubImg = & g_app.thread();
        }
        auto pImgDir = m_pImgDir;
        m_thrGenSubImg->start(100, [&, pImgDir]{
            if (!pImgDir->genSubImg(*this, *m_thrGenSubImg))
            {
                m_thrGenSubImg->usleep(300);
            }
            return true;
        });
    }
}

bool CAddBkgView::isHLayout() const
{
    return m_addbkgDlg.isHLayout();
}

bool CAddBkgView::handleReturn(bool bClose)
{
    reset();

    bool bRet = false;
    if (m_pImgDir)
    {
        bRet = true;

        if (m_pImgDir != &m_olBkgDir)
        {
            if (m_thrGenSubImg)
            {
                m_thrGenSubImg->cancel(false);
            }
        }

        if (m_pImgDir->parent() == &m_olBkgDir)
        {
            m_pImgDir = &m_olBkgDir;
        }
        else
        {
            m_pImgDir = NULL;
        }

        m_eScrollBar = E_LVScrollBar::LVSB_Left;
        scrollToItem(_scrollRecord(NULL));
    }

    if (bClose)
    {
        showLoading(false);

        for (auto pDir : m_olBkgDir.dirs())
        {
            ((CImgDir*)pDir)->cleanup();
        }
        for (auto pImgDir : m_paImgDirs)
        {
            pImgDir->cleanup();
        }

        g_uMsScanYield = 10;
    }
    else
    {
        g_uMsScanYield = 1;

        if (m_pImgDir) // == &m_olBkgDir)
        {
            showLoading(m_olBkgDir.downloading());
        }
        else
        {
            showLoading(m_thrScan);
        }
    }
    return bRet;
}

void CAddBkgView::showLoading(bool bShow)
{
    m_addbkgDlg.showLoading(bShow);
}

void CAddBkgView::scanDir(cwstr strDir)
{
    m_thrScan.cancel();

    if (m_thrGenSubImg)
    {
        m_thrGenSubImg->cancel();
    }

    m_paImgDirs.clear();
    m_paImgDirs.add(m_olBkgDir);

    m_rootImgDir.setDir(strDir);

    static UINT s_uSequence = 0;
    m_thrScan.start([&](signal_t bRunSignal){
        g_app.sync([&]{
           showLoading(true);
        });

        if (0 == s_uSequence)
        {
            m_olBkgDir.initOlBkg(*this);
        }
        auto uSequence = ++s_uSequence;

        m_rootImgDir.scanDir(bRunSignal, [&, uSequence](CImgDir& imgDir){
            if (m_pImgDir || !m_addbkgDlg.isVisible())
            {
                m_thrScan.usleep(30);
            }

            g_app.sync([&, uSequence]{
                if (uSequence == s_uSequence)
                {
                    m_paImgDirs.add(imgDir);
                    this->update();
                }
            });
        });

        (void)m_thrScan.usleep(100); //空目录视觉效果
        g_app.sync([&]{
            showLoading(false);
         });
    });
}
