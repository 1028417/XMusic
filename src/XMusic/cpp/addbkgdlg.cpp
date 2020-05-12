#include "app.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

static TSignal<pair<wstring, UINT>> g_sgnLoadImg;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg, CApp& app)
    : CDialog(bkgDlg)
    , m_bkgDlg(bkgDlg)
    , m_app(app)
    , m_rootImgDir(m_thread.runSignal())
    , m_lv(*this, app, m_paImgDirs)
{
    mtutil::thread([&](){
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
    });
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &CAddBkgDlg::_handleReturn);
}

void CAddBkgDlg::show()
{
    cauto fnScan = [&](){
        cauto bRunSignal = m_thread.runSignal();
        CPath::scanDir(bRunSignal, m_rootImgDir, [&](CPath& dir, TD_XFileList&){
            if (!m_lv.isInRoot())
            {
                mtutil::usleep(300);
                if (!bRunSignal)
                {
                    return;
                }
            }

            m_app.sync([&](){
                m_paImgDirs.add((CImgDir&)dir);

                this->update();
            });
        });
    };

#if __windows
/*#define __MediaFilter L"所有支持图片|*.Jpg;*.Jpeg;*.Png;*.Bmp|Jpg文件(*.Jpg)|*.Jpg|Jpeg文件(*.Jpeg)|*.Jpeg \
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

    static CFolderDlg FolderDlg;
    cauto strImgDir = FolderDlg.Show(hwnd(), NULL, L" 添加背景", L"请选择图片目录");
    if (strImgDir.empty())
    {
        return;
    }

    m_rootImgDir.setDir(strImgDir);
    m_thread.start([&](){
        fnScan();

        m_thread.usleepex(-1);

        m_rootImgDir.clear();
        m_paImgDirs.clear();
    });

    CDialog::show([&](){
        m_thread.cancel(false);
        m_thread.detach();
    });
#else

    if (!m_thread.joinable())
    {
        m_rootImgDir.setDir(__medialib.path() + L"/..");
        m_thread.start([&](){
            fnScan();
        });
    }

    CDialog::show();
#endif
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

    int y_addbkgView = 0;
    if (m_lv.isInRoot())
    {
        y_addbkgView = rcReturn.bottom() + rcReturn.top();
        ui.labelTitle->setVisible(true);
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
    if (m_lv.isInRoot())
    {
        close();
    }
    else
    {
        m_lv.upward();
        relayout();
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

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpeg", L"jfif", L"png", L"bmp");

CPath* CImgDir::_newSubDir(const tagFileInfo& fileInfo)
{
    if (!m_bRunSignal)
    {
        return NULL;
    }

    return new CImgDir(m_bRunSignal, fileInfo);
}

#define __szSubIngFilter 640

inline static bool _loadSubImg(const XFile& subFile, QPixmap& pm)
{
    if (!pm.load(__WS2Q(subFile.path())))
    {
        return false;
    }

    if (pm.width()<__szSubIngFilter || pm.height()<__szSubIngFilter)
    {
        return false;
    }

    return true;
}

#define __szSnapshot 160

XFile* CImgDir::_newSubFile(const tagFileInfo& fileInfo)
{
    if (!m_bRunSignal)
    {
        return NULL;
    }

    cauto strExtName = strutil::lowerCase_r(fsutil::GetFileExtName(fileInfo.strName));
    if (!g_setImgExtName.includes(strExtName))
    {
        return NULL;
    }

    if (m_paSubFile.empty())
    {
        mtutil::usleep(1);
        if (!_loadSubImg(XFile(fileInfo), m_pmSnapshot))
        {
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

void CImgDir::loadImg(int nIdx, const function<void(UINT, QPixmap&)>& cb)
{
    if (-1 == nIdx)
    {
        m_paSubFile.get(m_uPos, [&](XFile& file){
            g_sgnLoadImg.set({file.path(), m_uPos});
        });
    }
}

#define __szSubimgZoomout 500

extern void zoomoutPixmap(QPixmap& pm, int cx, int cy);

bool CImgDir::genSubImgs()
{
    return files().get(m_uPos, [&](XFile& file){
        QPixmap pm;
        if (_loadSubImg(file, pm))
        {
            int szZoomout = g_szScreenMax*0.88f;
            auto count = m_vecImgs.size();
            if (count >= 4)
            {
                szZoomout /= 3;

                if (4 == count)
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
            m_vecImgs.emplace_back(pm, file.path());
        }

        m_uPos++;
    });
}

/*class CResImgDir : public CPath, public IImgDir
{
public:
    CResImgDir() = default;

private:
    virtual size_t imgCount() const
    {
        return 0;
    }

    virtual const QPixmap* snapshot(int nIdx=-1) const
    {
        return NULL;
    }

    virtual wstring path(int nIdx=-1) const
    {
        return L"";
    }

    virtual bool genSubImgs()
    {
        return false;
    }
};*/

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
        m_paImgDirs.get(lvItem.uItem, [&](IImgDir& imgDir){
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

        m_paImgDirs.get(lvItem.uItem, [&](IImgDir& imgDir){
            showImgDir(imgDir);
        });

        m_addbkgDlg.relayout();
    }
}

void CAddBkgView::showImgDir(IImgDir& imgDir)
{
    m_pImgDir = &imgDir;
    m_eScrollBar = E_LVScrollBar::LVSB_None;
    update();

    /*m_pImgDir->loadImg(-1, [&](UINT, QPixmap&){
        m_app.sync([&](){
            update();
        });
    });
    return;*/

    timerutil::setTimerEx(50, [=](){
        if (NULL == m_pImgDir)
        {
            return false;
        }

        if (!m_pImgDir->genSubImgs())
        {
            return false;
        }

        update();

        return true;
    });
}

void CAddBkgView::upward()
{
    reset();

    m_eScrollBar = E_LVScrollBar::LVSB_Left;

    m_pImgDir = NULL;

    scroll(_scrollRecord(NULL));
}
