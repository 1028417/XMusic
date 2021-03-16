
#include "xmusic.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

#include <QMovie>

#define __olBkgDir L"网上图库"

static Ui::AddBkgDlg ui;

XThread *g_thrGenSubImg = NULL;
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

    auto movie = new QMovie(this);
    movie->setFileName(":/img/loading.gif");
    ui.labelLoading->setMovie(movie);
    ui.labelLoading->resize(200,200);
    ui.labelLoading->setStyleSheet("QWidget{background-color:rgb(255, 255, 255, 128); \
                                   border-top-left-radius:8px; border-top-right-radius:8px; \
                                   border-bottom-left-radius:8px; border-bottom-right-radius:8px;}");
    ui.labelLoading->setVisible(false);
    ui.labelLoading->raise();

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

#if __windows
    ui.labelChooseDir->setFont(1.08, QFont::Weight::Normal, false, true);

    connect(ui.labelChooseDir, &CLabel::signal_click, [&]{
        cwstr strDir = _chooseDir();
        if (strDir.empty())
        {
            return;
        }

        m_lv.thrScan().cancel();

        if (g_thrGenSubImg)
        {
            g_thrGenSubImg->cancel();
        }

        m_lv.scanDir(strDir, ui.labelLoading);
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
        m_lv.scanDir(strAddBkgDir, ui.labelLoading);
    }

    CDialog::show([&]{
        (void)m_lv.handleReturn(true);
    });
}

void CAddBkgDlg::onShowImgDir()
{
    ui.labelLoading->movie()->stop();
    ui.labelLoading->setVisible(false);

    _relayout(width(), height());
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
        y_addbkgView = rcReturn.bottom() + rcReturn.top();

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

    ui.labelLoading->move((cx-ui.labelLoading->width())/2
                          , y_addbkgView+(m_lv.height()-ui.labelLoading->height())/2);

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
    , m_thrScan(__app.thread())
    , m_rootImgDir(m_thrScan.signal())
{
    m_pmOlBkg.load(":/img/olBkg.png");
}

size_t CAddBkgView::getColCount() const
{
    if (m_pImgDir && m_pImgDir != &m_olBkgDir)
    {
        if (m_pImgDir->imgCount(m_addbkgDlg.isHLayout()) <= 4)
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
        return m_pImgDir->imgCount(m_addbkgDlg.isHLayout());
    }
    else
    {
        return m_paImgDirs.size();
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
        auto pm = m_pImgDir->img(m_addbkgDlg.isHLayout(), lvItem.uItem);
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
        m_paImgDirs.get(lvItem.uItem, [&](CImgDir& imgDir){
            _paintRow(painter, lvItem, imgDir);
        });
    }
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
        cauto strFilePath = m_pImgDir->imgPath(m_addbkgDlg.isHLayout(), lvItem.uItem);
        if (!strFilePath.empty())
        {
            m_addbkgDlg.addBkg(strFilePath);
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](CImgDir& imgDir){
            if (0 == lvItem.uItem)
            {
                m_olBkgDir.initOlBkg(*this);
            }

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

    if (m_pImgDir != &m_olBkgDir)
    {
        if (g_thrGenSubImg)
        {
            g_thrGenSubImg->cancel();
        }
        else
        {
            g_thrGenSubImg = &__app.thread();
        }
        auto pImgDir = m_pImgDir;
        g_thrGenSubImg->start(10, [&, pImgDir]{
            auto uGenCount = (this->scrollPos()+4)*3;
            if (!pImgDir->genSubImg(*this, uGenCount, *g_thrGenSubImg))
            {
                g_thrGenSubImg->usleep(100);
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

    if (bClose)
    {
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
    }
    return bRet;
}

void CAddBkgView::scanDir(cwstr strDir, QLabel *labelLoading)
{
    labelLoading->movie()->start();
    labelLoading->setVisible(true);

    m_paImgDirs.clear();

    cauto strOlBkgDir = g_strWorkDir + __wcPathSeparator + __olBkgDir;
    fsutil::createDir(strOlBkgDir);
    m_olBkgDir.setDir(strOlBkgDir);
    m_paImgDirs.add(m_olBkgDir);

    static UINT s_uSequence = 0;
    m_rootImgDir.setDir(strDir);
    m_thrScan.start([&, labelLoading](signal_t bRunSignal){
        auto uSequence = ++s_uSequence;
        CPath::scanDir(bRunSignal, m_rootImgDir, [&, uSequence](CPath& dir, TD_XFileList&){
            if (this->imgDir() || !m_addbkgDlg.isVisible())
            {
                m_thrScan.usleep(100);
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

        m_thrScan.usleep(100);
        if (!bRunSignal)
        {
            return;
        }
        __app.sync([labelLoading]{
            labelLoading->movie()->stop();
            labelLoading->setVisible(false);
        });
    });
}
