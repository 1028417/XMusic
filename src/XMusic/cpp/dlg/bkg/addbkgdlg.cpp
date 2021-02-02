
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
    , m_thrScan(__app.thread())
    , m_rootImgDir(m_thrScan.signal())
    , m_lv(*this, m_paImgDirs)
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

        m_thrScan.cancel();

        if (g_thrGenSubImg)
        {
            g_thrGenSubImg->cancel();
        }

        m_paImgDirs.clear();
        update();

        _scanDir(strDir);
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

    if (!m_thrScan.joinable())
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
        _scanDir(strAddBkgDir);
    }

    CDialog::show([&]{
        (void)m_lv.handleReturn(true);
    });
}

void CAddBkgDlg::_scanDir(cwstr strDir)
{
    UINT uDotCount = 0;
    timerutil::setTimerEx(240, [=]()mutable{
        if (!m_thrScan)
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

    static UINT s_uSequence = 0;
    m_thrScan.start([&, strDir](signal_t bRunSignal){
        auto uSequence = ++s_uSequence;
        m_rootImgDir.setDir(strDir);

        CPath::scanDir(bRunSignal, m_rootImgDir, [&, uSequence](CPath& dir, TD_XFileList&){
            if (m_lv.imgDir() || !this->isVisible())
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
    if (m_lv.imgDir() == NULL)
    {
        y_addbkgView = rcReturn.bottom() + rcReturn.top();
        ui.labelTitle->setVisible(true);

#if __windows
        ui.labelChooseDir->setVisible(true);
        ui.labelChooseDir->move(cx-ui.labelChooseDir->width()-cxMargin
                                        , rcReturn.center().y()-ui.labelChooseDir->height()/2);
#endif
    }
    else
    {
        ui.labelTitle->setVisible(false);
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
    if (m_lv.imgDir())
    {
        m_lv.handleReturn(false);
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


CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir) :
    CListView(&addbkgDlg, E_LVScrollBar::LVSB_Left)
    , m_addbkgDlg(addbkgDlg)
    , m_paImgDirs(paImgDir)
{
}

size_t CAddBkgView::getColCount() const
{
    if (m_pImgDir)
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
    if (m_pImgDir)
    {
        return getColCount();
    }

    return m_addbkgDlg.rowCount();
}

size_t CAddBkgView::getItemCount() const
{
    if (m_pImgDir)
    {
        return m_pImgDir->imgCount();
    }

    return m_paImgDirs.size();
}

void CAddBkgView::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    if (m_pImgDir)
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
            auto eStyle = E_LVItemStyle::IS_ForwardButton | E_LVItemStyle::IS_BottomLine;
            tagLVItemContext context(lvItem, eStyle);
            context.strText = imgDir.displayName();
            context.setIcon(imgDir.icon(), __size(-12));
            _paintRow(painter, context);
        });
    }
}

void CAddBkgView::_onItemClick(tagLVItem& lvItem, const QMouseEvent&)
{
    if (m_pImgDir)
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

void CAddBkgView::handleReturn(bool bClose)
{
    reset();

    if (m_pImgDir)
    {
        if (g_thrGenSubImg)
        {
            g_thrGenSubImg->cancel(false);
        }

        m_pImgDir = NULL;

        m_eScrollBar = E_LVScrollBar::LVSB_Left;
        scrollToItem(_scrollRecord(NULL));
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
}
