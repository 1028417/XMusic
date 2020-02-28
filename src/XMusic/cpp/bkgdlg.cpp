#include "app.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#define __retainSnapshot 8

static int g_xsize = 0;

static Ui::BkgDlg ui;

CBkgView::CBkgView(class CApp& app, CBkgDlg& bkgDlg)
    : CListView(&bkgDlg),
    m_app(app),
    m_bkgDlg(bkgDlg)
{
    (void)m_pmX.load(":/img/btnX.png");
}

size_t CBkgView::getPageRowCount()
{
    if (m_bkgDlg.bkgCount() <= 3)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

inline size_t CBkgView::getColumnCount()
{
    if (m_bkgDlg.bkgCount() <= 3)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

size_t CBkgView::getRowCount()
{
    return (UINT)ceil((1.0+m_bkgDlg.bkgCount())/ getColumnCount());
}

void CBkgView::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{
    size_t uColumnCount = getColumnCount();

    int nMargin = margin();

    QRect rc = lvRow.rc;
    if (rc.width() > rc.height())
    {
        nMargin /= 2;
        rc.setLeft(rc.left() + nMargin);
        rc.setTop(rc.top() + nMargin);
        rc.setRight(rc.right() - nMargin);
        rc.setBottom(rc.bottom() - nMargin);
    }
    else
    {
        int cy = rc.height()-nMargin;
        int cx = 0;

        if (2 == uColumnCount)
        {
            cx = cy*m_bkgDlg.width()/m_bkgDlg.height();
            cx = MIN(cx, rc.width()-nMargin/2);
        }
        else
        {
            cx = rc.width()-nMargin;
        }

        if (0 == lvRow.uCol)
        {
            rc.setRect(rc.right()-nMargin/2-cx, rc.top(), cx, cy);
        }
        else
        {
            rc.setRect(rc.left()+nMargin/2, rc.top(), cx, cy);
        }
    }

    size_t uItem = lvRow.uRow * uColumnCount + lvRow.uCol;
    if (0 == uItem)
    {
        m_app.mainWnd().drawDefaultBkg(painter, rc, __szRound);
    }
    else
    {
        size_t uIdx = uItem-1;
        cauto pm = m_bkgDlg.pixmap(uIdx);
        if (pm)
        {
            painter.drawPixmapEx(rc, *pm, __szRound);

            QRect rcX(rc.right()-g_xsize-5, rc.top()+5, g_xsize, g_xsize);
            painter.drawPixmap(rcX, m_pmX);
        }
        else
        {
            rc.setLeft(rc.left()+2);
            rc.setRight(rc.right()-1);

            int r = g_crTheme.red();
            int g = g_crTheme.green();
            int b = g_crTheme.blue();
            QColor cr(r<128?r+128:r-128, g<128?g+128:g-128, b<128?b+128:b-128);

            int d = abs(cr.red()+cr.green()+cr.blue()-g_crTheme.red()-g_crTheme.green()-g_crTheme.blue());
            if (abs(d) == 128)
            {
                cr.setAlpha(100);
            }
            else
            {
                cr.setAlpha(50);
            }

            painter.drawRectEx(rc, cr, 2, Qt::PenStyle::DotLine, __szRound);
            cr.setAlpha(128);

            int len = MIN(rc.width(), rc.height())/4;
#define __szAdd __size(4)
            cauto pt = rc.center();
            rc.setRect(pt.x()-len/2, pt.y()-__szAdd/2, len, __szAdd);
            painter.fillRectEx(rc, cr, __szAdd/2);

            rc.setRect(pt.x()-__szAdd/2, pt.y()-len/2, __szAdd, len);
            painter.fillRect(rc, g_crTheme);
            painter.fillRectEx(rc, cr, __szAdd/2);
        }
    }
}

void CBkgView::_onRowClick(tagLVRow& lvRow, const QMouseEvent& me)
{
    size_t uIdx = lvRow.uRow * getColumnCount() + lvRow.uCol;

    if (uIdx != 0 && uIdx < m_bkgDlg.bkgCount())
    {
        if (me.pos().x() >= lvRow.rc.right()-g_xsize && me.pos().y() <= lvRow.rc.top()+g_xsize)
        {
            m_bkgDlg.deleleBkg(uIdx);
            return;
        }
    }

    m_bkgDlg.setBkg(uIdx);
}

inline UINT CBkgView::margin()
{
#define __margin __size(40)
    return __margin/(getColumnCount()-1);
}

CBkgDlg::CBkgDlg(QWidget& parent, class CApp& app) : CDialog(parent)
    , m_app(app),
    m_colorDlg(*this, app),
    m_addbkgDlg(*this, app),
    m_bkgView(app, *this)
{
}

void CBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(1.15, QFont::Weight::DemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnColor, &CButton::signal_clicked, [&]() {
        m_colorDlg.show();
    });

    cauto strWorkDir = strutil::toWstr(fsutil::workDir());
    m_strHBkgDir = strWorkDir + L"/hbkg/";
    m_strVBkgDir = strWorkDir + L"/vbkg/";

#if __android
    wstring strBkgSrc = L"assets:";
#else
    wstring strBkgSrc = m_app.applicationDirPath().toStdWString();
#endif

    if (!fsutil::existDir(m_strHBkgDir))
    {
        (void)fsutil::createDir(m_strHBkgDir);

        for (UINT uIdx = 1; uIdx < 20; uIdx++)
        {
            cauto strFile = to_wstring(uIdx);
            cauto strBkg = strBkgSrc + L"/hbkg/" + strFile + L".jpg";

            if (!fsutil::copyFile(strBkg, m_strHBkgDir + strFile))
            {
                break;
            }
        }
    }

    if (!fsutil::existDir(m_strVBkgDir))
    {
        (void)fsutil::createDir(m_strVBkgDir);

        for (UINT uIdx = 1; uIdx < 20; uIdx++)
        {
            cauto strFile = to_wstring(uIdx);
            cauto strBkg = strBkgSrc + L"/vbkg/" + strFile + L".jpg";

            if (!fsutil::copyFile(strBkg, m_strVBkgDir + strFile))
            {
                break;
            }
        }
    }

    fsutil::findSubFile(m_strHBkgDir, [&](const wstring& strSubFile) {
        cauto itr = m_vecHBkgFile.insert(m_vecHBkgFile.begin(), {strSubFile, NULL});
        if (m_vecHBkgFile.size() <= __retainSnapshot)
        {
            itr->second = _loadPixmap(m_strHBkgDir+strSubFile);
        }
    });

    fsutil::findSubFile(m_strVBkgDir, [&](const wstring& strSubFile) {
        cauto itr = m_vecVBkgFile.insert(m_vecVBkgFile.begin(), {strSubFile, NULL});
        if (m_vecVBkgFile.size() <= __retainSnapshot)
        {
            itr->second = _loadPixmap(m_strVBkgDir+strSubFile);
        }
    });

    cauto strHBkg = m_app.getOption().strHBkg;
    if (!strHBkg.empty())
    {
        (void)m_pmHBkg.load(m_strHBkgDir + strHBkg);
    }

    cauto strVBkg = m_app.getOption().strVBkg;
    if (!strVBkg.empty())
    {
        (void)m_pmVBkg.load(m_strVBkgDir + strVBkg);
    }

    m_colorDlg.init();

    m_addbkgDlg.init();
}

void CBkgDlg::_relayout(int cx, int cy)
{
    int sz = cy>cx ? cy/11.1 : cy/6.1;
    int xMargin = sz/4;
    QRect rcReturn(xMargin, xMargin, sz-xMargin*2, sz-xMargin*2);
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    g_xsize = rcReturn.width()-__size(5);

    m_bHScreen = cx>cy;
    if (m_bHScreen)
    {
        ui.btnColor->setGeometry(xMargin, cy - rcReturn.top() - rcReturn.height()
                                 , rcReturn.width(), rcReturn.height());

        int x_bkgView = rcReturn.right()+xMargin;
        int cx_bkgView = cx-x_bkgView;
        int cy_bkgView = cx_bkgView*cy/cx;
        m_bkgView.setGeometry(x_bkgView - m_bkgView.margin()/2
                              , (cy-cy_bkgView)/2, cx_bkgView, cy_bkgView);

        ui.labelTitle->setGeometry(0, 0, x_bkgView, cy);
        ui.labelTitle->setWordWrap(true);
        ui.labelTitle->setText("设\n置\n背\n景");
    }
    else
    {
        ui.btnColor->setGeometry(cx-xMargin-rcReturn.width()
                                 , rcReturn.top(), rcReturn.width(), rcReturn.height());

        int y_bkgView = rcReturn.bottom() + rcReturn.top();
        int cy_bkgView = cy-y_bkgView;
        int cx_bkgView = cy_bkgView*cx/cy;
        m_bkgView.setGeometry((cx-cx_bkgView)/2, y_bkgView, cx_bkgView, cy_bkgView);

        ui.labelTitle->setGeometry(0, 0, cx, y_bkgView);
        ui.labelTitle->setText("设置背景");
    }
}

const QPixmap* CBkgDlg::_loadPixmap(const WString& strBkgFile)
{
    m_lstPixmap.emplace_back(QPixmap());
    auto& pm = m_lstPixmap.back();
    if (pm.load(strBkgFile))
    {
#define __snapshotZoomout 900
        CPainter::zoomoutPixmap(pm, __snapshotZoomout);
    }

    return &pm;
}

const QPixmap* CBkgDlg::pixmap(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx >= vecBkgFile.size())
    {
        return NULL;
    }

    auto& pr = vecBkgFile[uIdx];
    if (pr.second)
    {
        return pr.second;
    }

    cauto strBkgFile = _bkgDir() + pr.first;

    return pr.second = _loadPixmap(strBkgFile);
}

void CBkgDlg::_setBkg(const wstring& strFile)
{
    m_app.getOption().bUseThemeColor = false;

    if (m_bHScreen)
    {
        m_app.getOption().strHBkg = strFile;
    }
    else
    {
        m_app.getOption().strVBkg = strFile;
    }

    QPixmap& pmBkg = m_bHScreen?m_pmHBkg:m_pmVBkg;
    if (!strFile.empty())
    {
        (void)pmBkg.load(_bkgDir() + strFile);
    }
    else
    {
        pmBkg = QPixmap();
    }

    m_app.mainWnd().updateBkg();
}

void CBkgDlg::setBkg(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (0 == uIdx)
    {
        _setBkg(L"");
        close();
    }
    else
    {
        uIdx--;
        if (uIdx < vecBkgFile.size())
        {
            _setBkg(vecBkgFile[uIdx].first);
            close();
        }
        else
        {
            _showAddBkg();
        }
    }
}

void CBkgDlg::_showAddBkg()
{
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
    }

    return;*/

    static CFolderDlg FolderDlg;
    cauto strImgDir = FolderDlg.Show(hwnd(), NULL, L" 添加背景", L"请选择图片目录");
    if (strImgDir.empty())
    {
        return;
    }
#else
    cauto strImgDir = m_app.getMediaLib().GetAbsPath() + L"/..";
#endif

    m_thread.start([&, strImgDir](const bool& bRunSignal){
        m_rootImgDir.scan(strImgDir, bRunSignal, [&](CImgDir& imgDir) {
            m_addbkgDlg.addImgDir(imgDir);
        });
    });

    m_addbkgDlg.show(NULL, [&](){
        m_thread.cancel();

        m_rootImgDir.clear();
    });
}

void CBkgDlg::addBkg(const wstring& strFile)
{
    wstring strFileName = to_wstring(time(0));
    if (!fsutil::copyFile(strFile, _bkgDir() + strFileName))
    {
        return;
    }

    _setBkg(strFileName);

    auto& vecBkgFile = _vecBkgFile();
    vecBkgFile.insert(vecBkgFile.begin(), {strFileName,NULL});

    close(); //m_bkgView.update();
}

void CBkgDlg::deleleBkg(size_t uIdx)
{
    if (0 == uIdx)
    {
        return;
    }
    uIdx--;

    auto& vecBkgFile = _vecBkgFile();
    if (uIdx < vecBkgFile.size())
    {
        cauto pr = vecBkgFile[uIdx];

        cauto strBkg = m_bHScreen ? m_app.getOption().strHBkg : m_app.getOption().strVBkg;
        if (strBkg == *pr.first)
        {
            _setBkg(L"");
        }

        if (pr.second)
        {
            for (auto itr = m_lstPixmap.begin(); itr != m_lstPixmap.end(); ++itr)
            {
                if (&*itr == pr.second)
                {
                    m_lstPixmap.erase(itr);
                    break;
                }
            }
        }

        fsutil::removeFile(_bkgDir() + pr.first);
        vecBkgFile.erase(vecBkgFile.begin()+uIdx);

        m_bkgView.update();
    }
}

void CBkgDlg::_onClosed()
{
    set<const QPixmap*> setDelelePixmap;
    if (m_vecHBkgFile.size() > __retainSnapshot)
    {
        for (auto itr = m_vecHBkgFile.begin()+__retainSnapshot; itr != m_vecHBkgFile.end(); ++itr)
        {
            setDelelePixmap.insert(itr->second);
            itr->second = NULL;
        }
    }

    if (m_vecVBkgFile.size() > __retainSnapshot)
    {
        for (auto itr = m_vecVBkgFile.begin()+__retainSnapshot; itr != m_vecVBkgFile.end(); ++itr)
        {
            setDelelePixmap.insert(itr->second);
            itr->second = NULL;
        }
    }

    for (auto itr = m_lstPixmap.begin(); itr != m_lstPixmap.end(); )
    {
        if (setDelelePixmap.find(&*itr) != setDelelePixmap.end())
        {
           itr = m_lstPixmap.erase(itr);
        }
        else
        {
            ++itr;
        }
    }

    m_bkgView.reset();
}

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpeg", L"jfif", L"png", L"bmp");

XFile* CImgDir::_newSubFile(const tagFileInfo& fileInfo)
{
    cauto strExtName = strutil::lowerCase_r(fsutil::GetFileExtName(fileInfo.strName));
    if (g_setImgExtName.includes(strExtName))
    {
        return new XFile(fileInfo);
    }

    return NULL;
}

inline static bool _loadImg(XFile& subFile, QPixmap& pm, UINT uZoomOutSize)
{
    if (!pm.load(strutil::toQstr(subFile.absPath())))
    {
        return false;
    }

#define __filterSize 640
    if (pm.width()<__filterSize || pm.height()<__filterSize)
    {
        return false;
    }

    CPainter::zoomoutPixmap(pm, uZoomOutSize);

    return true;
}

void CImgDir::scan(const wstring& strDir, const bool& bRunSignal, cfn_void_t<CImgDir&> cb)
{
    CPath::setDir(strDir);
    CPath::scan([&, cb](CPath& dir, TD_XFileList& paSubFile) {
        if (paSubFile)
        {
            auto& imgDir = (CImgDir&)dir;
            if (imgDir._genSnapshot(paSubFile))
            {
                if (bRunSignal)
                {
                    cb(imgDir);
                }
            }
        }

        return bRunSignal;
    });
}

/*class CResImgDir : public CPath, public IImgDir
{
public:
    CResImgDir(){}

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

void CImgDir::_onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile)
{
    //paSubDir.add(new CResImgDir);
    CPath::_onFindFile(paSubDir,  paSubFile);
}

bool CImgDir::_genSnapshot(TD_XFileList& paSubFile)
{
    for (m_itrSubFile = paSubFile.begin(); m_itrSubFile != paSubFile.end(); ++m_itrSubFile)
    {
        auto pSubFile = *m_itrSubFile;

#define __snapshotSize 150
        if (_loadImg(*pSubFile, m_pmSnapshot, __snapshotSize))
        {
            m_vecSubImgs.reserve(paSubFile.size());
            return true;
        }
    }

    return false;
}

const QPixmap* CImgDir::snapshot(int nIdx) const
{
    if (nIdx < 0)
    {
        return &m_pmSnapshot;
    }

    if ((size_t)nIdx < m_vecSubImgs.size())
    {
        return &m_vecSubImgs[nIdx].first;
    }

    return NULL;
}

wstring CImgDir::path(int nIdx) const
{
    if (nIdx < 0)
    {
        return CPath::oppPath();
    }

    if ((size_t)nIdx < m_vecSubImgs.size())
    {
        return m_vecSubImgs[nIdx].second;
    }

    return L"";
}

bool CImgDir::genSubImgs()
{
    cauto files = CPath::files();
    if (files.end() == m_itrSubFile)
    {
        return false;
    }

    auto pSubFile = *m_itrSubFile;

#define __subimgZoomout 500
    QPixmap pm;
    if (_loadImg(*pSubFile, pm, __subimgZoomout))
    {
        m_vecSubImgs.emplace_back(QPixmap(), pSubFile->absPath());
        m_vecSubImgs.back().first.swap(pm);
    }

    ++m_itrSubFile;

    return true;
}
