
#include "xmusic.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

static UINT g_uMsScanYield = 1;

static XThread *g_thrGenSubImg = NULL;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg)
    : CDialog(bkgDlg)
    , m_bkgDlg(bkgDlg)
    , m_thrScan(__app.thread())
    , m_rootImgDir(m_thrScan.runSignal())
    , m_lv(*this, m_paImgDirs)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

#if __windows
    ui.labelChooseDir->setFont(1.08, QFont::Weight::Normal, false, true);

    connect(ui.labelChooseDir, &CLabel::signal_click, [&](){
        if (!_chooseDir())
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

        _scanDir(__app.getOption().strAddBkgDir);
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
bool CAddBkgDlg::_chooseDir()
{
    auto& strAddBkgDir = __app.getOption().strAddBkgDir;

    CFolderDlg FolderDlg;
    cauto strDir = FolderDlg.Show(hwnd(), strAddBkgDir.c_str(), L" 请选择图片目录");
    if (strDir.empty())
    {
        return false;
    }

    if (strutil::matchIgnoreCase(strDir, strAddBkgDir))
    {
        return false;
    }

    strAddBkgDir = strDir;

    return true;
}
#endif

void CAddBkgDlg::show()
{
    g_uMsScanYield = 1;

    if (!m_thrScan.joinable())
    {
#if __windows
        auto& strAddBkgDir = __app.getOption().strAddBkgDir;
        if (strAddBkgDir.empty() || !fsutil::existDir(strAddBkgDir))
        {
            //if (!_chooseDir())
            //{
                //return;
                strAddBkgDir = fsutil::getHomeDir().toStdWString();
            //}
        }
        //else {
        mtutil::thread([&](){
            CFolderDlg::preInit();
        });

        _scanDir(strAddBkgDir);

#elif __android
        if (!requestAndroidPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
        {
            return;
        }
        _scanDir(__sdcardDir);

#else
        _scanDir(fsutil::getHomeDir().toStdWString());
#endif
    }

    CDialog::show([&](){
        (void)m_lv.handleReturn();
    });
}

void CAddBkgDlg::_scanDir(cwstr strDir)
{
    m_rootImgDir.setDir(strDir);

    static UINT s_uSequence = 0;
    auto uSequence = ++s_uSequence;

    m_thrScan.start([&, uSequence](XT_RunSignal bRunSignal){
        // TODO 显示正在扫描目录

        CPath::scanDir(bRunSignal, m_rootImgDir, [&, uSequence](CPath& dir, TD_XFileList&){
            if (m_lv.imgDir() || !this->isVisible())
            {
                m_thrScan.usleepex(300);
            }

            if (!bRunSignal)
            {
                return;
            }

            __app.sync([&, uSequence](){
                if (uSequence != s_uSequence)
                {
                    return;
                }

                m_paImgDirs.add((CImgDir&)dir);

                this->update();
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
    if (m_lv.handleReturn())
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

wstring CImgDir::displayName() const
{
#if __android
    if (m_fi.pParent)
    {
        if (m_fi.pParent->parent())
        {
            return ((CImgDir*)m_fi.pParent)->displayName() + L'/' + m_fi.strName;
        }
        else
        {
            return L'/' + m_fi.strName;
        }
    }
    else
    {
        return L"根目录";
    }

#else
    return path();
#endif
}

#define __szSubIngFilter 640

inline static bool _loadSubImg(cwstr strFile, QPixmap& pm)
{
    if (!pm.load(__WS2Q(strFile)))
    {
        return false;
    }

    if (pm.width()<__szSubIngFilter || pm.height()<__szSubIngFilter)
    {
        return false;
    }

    return true;
}

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpe", L"jpeg", L"jfif", L"png", L"bmp");

#define __szSnapshot 160

XFile* CImgDir::_newSubFile(const tagFileInfo& fileInfo)
{
    if (!m_bRunSignal)
    {
        return NULL;
    }
    mtutil::usleep(g_uMsScanYield);

    cauto strExtName = strutil::lowerCase_r(fsutil::GetFileExtName(fileInfo.strName));
    if (!g_setImgExtName.includes(strExtName))
    {
        return NULL;
    }

    if (m_paSubFile.empty())
    {
        if (!_loadSubImg(XFile(fileInfo).path(), m_pmSnapshot))
        {
            if (!m_bRunSignal)
            {
                return NULL;
            }
            mtutil::usleep(g_uMsScanYield);

            m_pmSnapshot = QPixmap();
            return NULL;
        }

        auto&& temp = m_pmSnapshot.width() < m_pmSnapshot.height()
                ? m_pmSnapshot.scaledToWidth(__szSnapshot, Qt::SmoothTransformation)
                : m_pmSnapshot.scaledToHeight(__szSnapshot, Qt::SmoothTransformation);
        m_pmSnapshot.swap(temp);
    }

    return new XFile(fileInfo);
}

CPath* CImgDir::_newSubDir(const tagFileInfo& fileInfo)
{
    if (!m_bRunSignal)
    {
        return NULL;
    }
    mtutil::usleep(g_uMsScanYield*3);

    if (fileInfo.strName.front() == L'.')
    {
        return NULL;
    }
    if (fileInfo.strName == __pkgName)
    {
        return NULL;
    }

    return new CImgDir(m_bRunSignal, fileInfo);
}

const QPixmap* CImgDir::img(UINT uIdx) const
{
    if (uIdx < m_vecImgs.size())
    {
        return &m_vecImgs[uIdx].pm;
    }

    return NULL;
}

wstring CImgDir::imgPath(UINT uIdx) const
{
    if (uIdx < m_vecImgs.size())
    {
        return m_vecImgs[uIdx].strPath;
    }

    return L"";
}

#define __szSubimgZoomout 500
extern void zoomoutPixmap(QPixmap& pm, int cx, int cy);

void CImgDir::genSubImgs(CAddBkgView& lv)
{
    if (m_uPos >= m_paSubFile.size())
    {
        return;
    }

    if (m_vecImgs.capacity() == 0)
    {
        m_vecImgs.reserve(m_paSubFile.size());
    }

    if (g_thrGenSubImg)
    {
        g_thrGenSubImg->cancel();
    }
    else
    {
        g_thrGenSubImg = &__app.thread(); //new XThread;
    }
    g_thrGenSubImg->start([&](){
        do {
            if (!_genSubImgs(lv))
            {
                return;
            }

        } while (g_thrGenSubImg->usleepex(30));
    });
}

bool CImgDir::_genSubImgs(CAddBkgView& lv)
{
    wstring strFile;
    if (!m_paSubFile.get(m_uPos, [&](XFile& file){
        strFile = file.path();
    }))
    {
        return false;
    }

    m_uPos++;

    QPixmap pm;

#if __windows || __mac
    if (m_paSubFile.get(m_uPos, [&](XFile& file){
        m_uPos++;

        cauto strFile2 = file.path();
        QPixmap pm2;
        mtutil::concurrence([&](){
            (void)_loadSubImg(strFile, pm);
        }, [&](){
            (void)_loadSubImg(strFile2, pm2);
        });

        if (!pm.isNull() || !pm2.isNull())
        {
            __app.sync([&, strFile, pm, strFile2, pm2]()mutable{
                if (this != lv.imgDir())
                {
                     m_uPos-=2;
                     return;
                }

                if (!pm.isNull())
                {
                    _genSubImgs(strFile, pm);
                }

                if (!pm2.isNull())
                {
                    _genSubImgs(strFile2, pm2);
                }

                lv.update();
            });
        }
    }))
    {
        return true;
    }
#endif

    if (_loadSubImg(strFile, pm))
    {
        __app.sync([&, strFile, pm]()mutable{
            if (this != lv.imgDir())
            {
                m_uPos--;
                return;
            }

            _genSubImgs(strFile, pm);

            lv.update();
        });
    }

    return true;
}

void CImgDir::_genSubImgs(cwstr strFile, QPixmap& pm)
{
    auto prevCount = m_vecImgs.size();
    int szZoomout = g_screen.szScreenMax;
    if (prevCount >= 4)
    {
        szZoomout /= 3;

        auto n = prevCount/18;
        if (n > 1)
        {
            szZoomout /= pow(n, 0.3);
            if (prevCount % 18 == 0)
            {
                for (auto& bkgImg : m_vecImgs)
                {
                    zoomoutPixmap(bkgImg.pm, szZoomout, szZoomout);
                }
            }
        }
        else if (4 == prevCount)
        {
            for (auto& bkgImg : m_vecImgs)
            {
                zoomoutPixmap(bkgImg.pm, szZoomout, szZoomout);
            }
        }
    }
    else
    {
        szZoomout /= 2;
    }

    zoomoutPixmap(pm, szZoomout, szZoomout);

    m_vecImgs.emplace_back(pm, strFile);
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
            painter.drawPixmapEx(rcFrame, *pm);

            rcFrame.setLeft(rcFrame.left()-1);
            rcFrame.setTop(rcFrame.top()-1);
            painter.drawRectEx(rcFrame, QColor(255,255,255,50));
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](CImgDir& imgDir){
            auto eStyle = E_LVItemStyle::IS_ForwardButton | E_LVItemStyle::IS_BottomLine;
            tagLVItemContext context(lvItem, eStyle);
            context.strText = imgDir.displayName();
            context.setIcon(&imgDir.snapshot(), __size(-12));
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
            m_addbkgDlg.bkgDlg().addBkg(strFilePath);
        }
    }
    else
    {
        m_paImgDirs.get(lvItem.uItem, [&](CImgDir& imgDir){
            _showImgDir(imgDir);
        });
    }
}

void CAddBkgView::_showImgDir(CImgDir& imgDir)
{    
    g_uMsScanYield = 10;

    _saveScrollRecord(NULL);

    m_eScrollBar = E_LVScrollBar::LVSB_None;

    m_pImgDir = &imgDir;
    for (auto pImgDir : m_paImgDirs)
    {
        if (pImgDir != m_pImgDir)
        {
            m_pImgDir->cleanup();
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

    async(30, [&](){
        _genSubImgs();
    });
}*/

bool CAddBkgView::handleReturn()
{
    if (m_pImgDir)
    {
        if (g_thrGenSubImg)
        {
            g_thrGenSubImg->cancel(false);
        }

        m_pImgDir = NULL;

        reset();
        m_eScrollBar = E_LVScrollBar::LVSB_Left;
        scrollToItem(_scrollRecord(NULL));

        g_uMsScanYield = 1;

        return true;
    }

    g_uMsScanYield = 10;

    /*if (g_thrGenSubImg)
    {
        g_thrGenSubImg->join();
        delete g_thrGenSubImg;
        g_thrGenSubImg = NULL;
    }*/

    return false;
}
