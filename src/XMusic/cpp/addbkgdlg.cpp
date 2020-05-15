#include "app.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

//static TSignal<pair<wstring, UINT>> g_sgnLoadImg;

static UINT g_uMsScanYield = 1;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg, CApp& app)
    : CDialog(bkgDlg)
    , m_bkgDlg(bkgDlg)
    , m_app(app)
    , m_thrScan(m_app.thread())
    , m_rootImgDir(m_thrScan.runSignal())
    , m_lv(*this, app, m_paImgDirs)
{
    /*mtutil::thread([&](){
        wstring strFile;
        UINT uIdx = 0;
        while (true)
        {
            bool bRet = g_sgnLoadImg.wait([&](pair<wstring, UINT>& pr){
                if (!pr.first.empty())
                {
                    strFile.swap(pr.first);
                    uIdx = pr.second;
                    return true;
                }

                return false;
            });
            if (!bRet)
            {
                break;
            }
        }
    });*/
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

#if __windows
    ui.labelChooseDir->setFont(1.055, QFont::Weight::Normal, false, true);

    connect(ui.labelChooseDir, &CLabel::signal_click, [&](){
        if (!_chooseDir())
        {
            return;
        }

        m_thrScan.cancel();

        m_paImgDirs.clear();
        update();

        _scanDir(m_app.getCtrl().getOption().strAddBkgDir);
    });
#endif

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
    auto& strAddBkgDir = m_app.getCtrl().getOption().strAddBkgDir;

    CFolderDlg FolderDlg;
    cauto strDir = FolderDlg.Show(this->isVisible()?hwnd():m_bkgDlg.hwnd(), strAddBkgDir.c_str(), L" 请选择图片目录");
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
        auto& strAddBkgDir = m_app.getCtrl().getOption().strAddBkgDir;
        if (strAddBkgDir.empty() || !fsutil::existDir(strAddBkgDir))
        {
            if (!_chooseDir())
            {
                return;
            }
        }
        _scanDir(strAddBkgDir);

#else
        _scanDir(__medialib.path() + L"/..");
#endif
    }

    CDialog::show();
}

void CAddBkgDlg::_scanDir(cwstr strDir)
{
    m_rootImgDir.setDir(strDir);

    m_thrScan.start([&](){
        CPath::scanDir(m_thrScan.runSignal(), m_rootImgDir, [&](CPath& dir, TD_XFileList&){
            if (!m_lv.isInRoot() || !this->isVisible())
            {
                if (!m_thrScan.usleepex(300))
                {
                    return;
                }
            }

            m_app.sync([&](){
                m_paImgDirs.add((CImgDir&)dir);

                this->update();
            });
        });

        m_thrScan.usleepex(-1);

        m_rootImgDir.clear();
        m_paImgDirs.clear();
    });
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    m_uRowCount = CBkgDlg::caleRowCount(cy);

    int sz = MAX(cx, cy)/(CBkgDlg::caleRowCount(MAX(cx, cy))+1.6f);
    int xMargin = sz/4;
    QRect rcReturn(xMargin, xMargin, sz-xMargin*2, sz-xMargin*2);
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    ui.labelTitle->move(rcReturn.right() + xMargin, rcReturn.center().y() - ui.labelTitle->height()/2);

    ui.labelChooseDir->setVisible(false);

    int y_addbkgView = 0;
    if (m_lv.isInRoot())
    {
        y_addbkgView = rcReturn.bottom() + rcReturn.top();
        ui.labelTitle->setVisible(true);

#if __windows
        ui.labelChooseDir->setVisible(true);
        ui.labelChooseDir->move(cx-ui.labelChooseDir->width()-xMargin
                                        , rcReturn.center().y()-ui.labelChooseDir->height()/2);
#endif
    }
    else
    {
        ui.labelTitle->setVisible(false);
    }

    m_lv.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);

    static BOOL bHLayout = -1;
    if (bHLayout != (BOOL)m_bHLayout)
    {
        bHLayout = m_bHLayout;
        m_lv.scroll(m_lv.scrollPos());
    }
}

bool CAddBkgDlg::_handleReturn()
{
    if (m_lv.handleReturn())
    {
        relayout();
    }
    else
    {
        close();
    }

    return true;
}

wstring CImgDir::displayName() const
{
#if __windows
    return path();

#elif __android
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
    return QDir(__WS2Q(path())).absolutePath().toStdWString();
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

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpeg", L"jfif", L"png", L"bmp");

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

bool CImgDir::genSubImgs()
{
    wstring strFile;
    if (!m_paSubFile.get(m_uPos, [&](XFile& file){
        strFile = file.path();
    }))
    {
        return false;
    }

    cauto fn = [&](){
        QPixmap pm;
        (void)_loadSubImg(strFile, pm);
        _genSubImgs(pm, strFile);
        m_uPos++;
    };

#if __windows || __mac
    if (m_paSubFile.get(m_uPos+1, [&](XFile& file){
        cauto strFile = file.path();
        QPixmap pm;
        mtutil::concurrence(fn, [&](){
            (void)_loadSubImg(strFile, pm);
        });
        _genSubImgs(pm, strFile);
        m_uPos++;
    }))
    {
        return true;
    }
#endif

    fn();

    return true;
}

void CImgDir::_genSubImgs(QPixmap& pm, cwstr strFile)
{
    if (pm.isNull())
    {
        return;
    }

    auto prevCount = m_vecImgs.size();
    if (0 == prevCount)
    {
        m_vecImgs.reserve(m_paSubFile.size()-m_uPos);
    }

    int szZoomout = g_szScreenMax;
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

CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg, CApp& app, const TD_ImgDirList& paImgDir) :
    CListView(&addbkgDlg, E_LVScrollBar::LVSB_Left)
    , m_addbkgDlg(addbkgDlg)
    , m_app(app)
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
            auto eStyle = E_LVItemStyle::IS_MultiLine
                    | E_LVItemStyle::IS_RightTip | E_LVItemStyle::IS_BottomLine;
            tagLVItemContext context(lvItem, eStyle);
            context.strText = imgDir.displayName();
            context.pmIcon = &imgDir.snapshot();
            _paintRow(painter, context);
        });
    }
}

void CAddBkgView::_onRowClick(tagLVItem& lvItem, const QMouseEvent&)
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
        _saveScrollRecord(NULL);

        m_paImgDirs.get(lvItem.uItem, [&](CImgDir& imgDir){
            _showImgDir(imgDir);
        });

        m_addbkgDlg.relayout();
    }
}

void CAddBkgView::_showImgDir(CImgDir& imgDir)
{
    g_uMsScanYield = 10;

    m_pImgDir = &imgDir;
    if (m_pPrevImgDir && m_pPrevImgDir != m_pImgDir)
    {
        m_pPrevImgDir->cleanup();
    }
    m_pPrevImgDir = m_pImgDir;

    m_eScrollBar = E_LVScrollBar::LVSB_None;
    update();

    CApp::async([&](){
        _genSubImgs();
    });
}

void CAddBkgView::_genSubImgs()
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

    CApp::async(30, [&](){
        _genSubImgs();
    });
}

bool CAddBkgView::handleReturn()
{
    if (m_pImgDir)
    {        
        g_uMsScanYield = 1;

        reset();

        m_eScrollBar = E_LVScrollBar::LVSB_Left;

        m_pImgDir = NULL;

        scroll(_scrollRecord(NULL));

        return true;
    }

    if (m_pPrevImgDir)
    {
        m_pPrevImgDir->cleanup();
        m_pPrevImgDir = NULL;
    }

    g_uMsScanYield = 10;

    return false;
}
