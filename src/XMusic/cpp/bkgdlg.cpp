#include "app.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#define __szZoomout 2160

#define __snapshotZoomout 900

#define __snapshotRetain 16

static int g_xsize = 0;

static Ui::BkgDlg ui;

CBkgView::CBkgView(class CApp& app, CBkgDlg& bkgDlg)
    : CListView(&bkgDlg),
    m_app(app),
    m_bkgDlg(bkgDlg)
{
    (void)m_pmX.load(":/img/btnX.png");
}

inline size_t CBkgView::getPageRowCount() const
{
    if (m_bkgDlg.bkgCount() <= 2)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

inline size_t CBkgView::getColumnCount() const
{
    return getPageRowCount();
}

size_t CBkgView::getRowCount() const
{
    auto uItemCount = 2+m_bkgDlg.bkgCount();
    auto uColCount = getColumnCount();

    auto uRowCount = (UINT)ceil((float)uItemCount/uColCount);
    if (uRowCount > 3)
    {
        if (uItemCount % uColCount == 0)
        {
            uRowCount++;
        }
    }

    return uRowCount;
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
    if (1 == uItem)
    {
        m_app.mainWnd().drawDefaultBkg(painter, rc, __szRound);
    }
    else
    {
        if (uItem >= 2)
        {
            auto pm = m_bkgDlg.pixmap(uItem-2);
            if (pm)
            {
                painter.drawPixmapEx(rc, *pm, __szRound);

                QRect rcX(rc.right()-g_xsize-5, rc.top()+5, g_xsize, g_xsize);
                painter.drawPixmap(rcX, m_pmX);
                return;
            }

            if (uItem < 9)
            {
                return;
            }
        }

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

void CBkgView::_onRowClick(tagLVRow& lvRow, const QMouseEvent& me)
{
    size_t uItem = lvRow.uRow * getColumnCount() + lvRow.uCol;
    if (uItem >= 2)
    {
        auto uIdx = uItem-2;
        if (uIdx < m_bkgDlg.bkgCount())
        {
            if (me.pos().x() >= lvRow.rc.right()-g_xsize && me.pos().y() <= lvRow.rc.top()+g_xsize)
            {
                m_bkgDlg.deleleBkg(uIdx);
                return;
            }
        }
        else
        {
            if (uItem<9)
            {
                return;
            }
        }
    }

    m_bkgDlg.setBkg(uItem);
}

inline UINT CBkgView::margin()
{
#define __margin __size(40)
    return __margin/(getColumnCount()-1);
}

CBkgDlg::CBkgDlg(QWidget& parent, class CApp& app) : CDialog(parent)
    , m_app(app),
    m_colorDlg(*this, app),
    m_addbkgDlg(*this, m_paImgDirs),
    m_lv(app, *this)
{
}

void CBkgDlg::preinit()
{
#if __windows
    CFolderDlg::preInit();
#endif

    cauto strWorkDir = strutil::fromStr(fsutil::workDir());
    cauto strAppVersion = m_app.appVersion();

    cauto fn = [&](bool bHBkg)
    {
        cauto strBkgDir = bHBkg?(m_strHBkgDir = strWorkDir + L"/hbkg/")
                              :(m_strVBkgDir = strWorkDir + L"/vbkg/");
        (void)fsutil::createDir(strBkgDir);

        wstring strAppBkgDir = strBkgDir + strAppVersion;
        if (!fsutil::existDir(strAppBkgDir))
        {
            (void)fsutil::createDir(strAppBkgDir);

#if __android
            wstring strBkgSrc = L"assets:";
#else
            wstring strBkgSrc = m_app.applicationDirPath().toStdWString();
#endif
            strBkgSrc.append(L"/bkg/");

            for (wchar_t wch = L'a'; wch <= L'z'; wch++)
            {
                auto strBkg = strBkgSrc + wch + L".jpg";
                (void)fsutil::copyFile(strBkg, strAppBkgDir + __wcPathSeparator + L"0." + wch);

                strBkg = strBkgSrc + (bHBkg?L"hbkg/":L"vbkg/") + wch + L".jpg";
                (void)fsutil::copyFile(strBkg, strAppBkgDir + __wcPathSeparator + L"1." + wch);

                strBkg = strBkgSrc + (bHBkg?L"hbkg/city/":L"vbkg/city/") + wch + L".jpg";
                (void)fsutil::copyFile(strBkg, strAppBkgDir + __wcPathSeparator + L"2." + wch);
            }
        }

        auto& vecBkgFile = bHBkg?m_vecHBkgFile:m_vecVBkgFile;
        fsutil::findSubFile(strAppBkgDir, [&](const wstring& strSubFile) {
            vecBkgFile.emplace_back(false, strAppVersion + __wcPathSeparator + strSubFile);
        });

        fsutil::findFile(strBkgDir, [&](const tagFileInfo& fileInfo) {
            cauto strPath = strBkgDir+fileInfo.strName;
            if (fileInfo.bDir)
            {
                if (fileInfo.strName != m_app.appVersion())
                {
                    (void)fsutil::removeDirTree(strPath);
                }
            }
            else
            {
                vecBkgFile.insert(vecBkgFile.begin(), tagBkgFile(false, fileInfo.strName));
            }
        });

        for (auto itr = vecBkgFile.begin(); itr != vecBkgFile.end()
             && itr-vecBkgFile.begin() < __snapshotRetain; ++itr)
        {
            itr->pm = _loadPixmap(strBkgDir + itr->strPath);
        }

        wstring strHBkg = m_app.getOption().strHBkg;
        if (!strHBkg.empty())
        {
            (void)m_pmHBkg.load(__WS2Q(m_strHBkgDir + strHBkg));
        }

        wString strVBkg = m_app.getOption().strVBkg;
        if (!strVBkg.empty())
        {
            (void)m_pmVBkg.load(__WS2Q(m_strVBkgDir + strVBkg));
        }
    };

    fn(true);
    fn(false);
}

void CBkgDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnColor, &CButton::signal_clicked, [&]() {
        m_colorDlg.show();
    });

    m_colorDlg.init();

    m_addbkgDlg.init();

    connect(this, &CBkgDlg::signal_founddir, this, &CBkgDlg::slot_founddir);
}

void CBkgDlg::_relayout(int cx, int cy)
{
    int sz = MAX(cx, cy)/11;
    int xMargin = sz/4;
    QRect rcReturn(xMargin, xMargin, sz-xMargin*2, sz-xMargin*2);
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    g_xsize = rcReturn.width()-__size(5);  

    static bool bHScreen = false;
    if (bHScreen != m_bHScreen)
    {
        bHScreen = m_bHScreen;
        m_lv.scroll(0);

#if !__windows
        m_lv.setVisible(false);
        __appAsync([&](){
            m_lv.setVisible(true);
        });
#endif
    }

    if (m_bHScreen)
    {
        ui.btnColor->setGeometry(xMargin, cy - rcReturn.top() - rcReturn.height()
                                 , rcReturn.width(), rcReturn.height());

        int x_bkgView = rcReturn.right()+xMargin;
        int cx_bkgView = cx-x_bkgView;
        int cy_bkgView = cx_bkgView*cy/cx;
        m_lv.setGeometry(x_bkgView - m_lv.margin()/2
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
        m_lv.setGeometry((cx-cx_bkgView)/2, y_bkgView, cx_bkgView, cy_bkgView);

        ui.labelTitle->setGeometry(0, 0, cx, y_bkgView);
        ui.labelTitle->setText("设置背景");
    }
}

inline wstring& CBkgDlg::_bkgDir()
{
    return m_bHScreen?m_strHBkgDir:m_strVBkgDir;
}

inline vector<tagBkgFile>& CBkgDlg::_vecBkgFile()
{
    return m_bHScreen?m_vecHBkgFile:m_vecVBkgFile;
}

const QPixmap* CBkgDlg::_loadPixmap(const WString& strBkgFile)
{
    m_lstPixmap.emplace_back(QPixmap());
    auto& pm = m_lstPixmap.back();
    if (pm.load(strBkgFile))
    {
        CPainter::zoomoutPixmap(pm, __snapshotZoomout);
    }

    return &pm;
}

size_t CBkgDlg::bkgCount() const
{
    return (m_bHScreen?m_vecHBkgFile:m_vecVBkgFile).size();
}

const QPixmap* CBkgDlg::pixmap(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx >= vecBkgFile.size())
    {
        return NULL;
    }

    auto& bkgFile = vecBkgFile[uIdx];
    if (bkgFile.pm)
    {
        return bkgFile.pm;
    }

    cauto strBkgFile = _bkgDir() + bkgFile.strPath;

    return bkgFile.pm = _loadPixmap(strBkgFile);
}

void CBkgDlg::_setBkg(const wstring& strFile)
{
    auto& pmBkg = m_bHScreen?m_pmHBkg:m_pmVBkg;
    if (!strFile.empty())
    {
        pmBkg = _loadBkg(_bkgDir() + strFile);
    }
    else
    {
        pmBkg = QPixmap();
    }

    _updateBkg(strFile);
}

QPixmap CBkgDlg::_loadBkg(const WString& strFile)
{
    QPixmap pm;
    if (pm.load(strFile))
    {
        auto szZoomout = __szZoomout;
        szZoomout = MAX(szZoomout, width());
        szZoomout = MAX(szZoomout, height());
        CPainter::zoomoutPixmap(pm, szZoomout);
    }

    return pm;
}

void CBkgDlg::_updateBkg(const wstring& strFile)
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

    m_app.mainWnd().updateBkg();
}

void CBkgDlg::setBkg(size_t uItem)
{
    auto& vecBkgFile = _vecBkgFile();
    if (0 == uItem)
    {
        _showAddBkg();
    }
    else if (1 == uItem)
    {
        _setBkg(L"");
        close();
    }
    else
    {
        uItem-=2;
        if (uItem < vecBkgFile.size())
        {
            _setBkg(vecBkgFile[uItem].strPath);
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

    m_rootImgDir.scan(strImgDir, [&](CImgDir& imgDir) {
        _addImgDir(imgDir);
    });

    m_addbkgDlg.show(NULL, [&](){
        m_rootImgDir.clear();
        m_paImgDirs.clear();
    });
#else

    cauto strImgDir = __medialib.path() + L"/..";

    m_rootImgDir.scan(strImgDir, [&](CImgDir& imgDir) {
        _addImgDir(imgDir);
    });

    m_addbkgDlg.show();
#endif
}

void CBkgDlg::addBkg(const wstring& strFile)
{
    QPixmap& pmBkg = m_bHScreen?m_pmHBkg:m_pmVBkg;
    pmBkg = _loadBkg(strFile);

    m_lstPixmap.push_back(pmBkg);
    auto& pm = m_lstPixmap.back();
    CPainter::zoomoutPixmap(pm, __snapshotZoomout);

    cauto strFileName = to_wstring(time(0));
    auto& vecBkgFile = _vecBkgFile();
    vecBkgFile.insert(vecBkgFile.begin(), tagBkgFile(false, strFileName, &pm));
    //update();

    _updateBkg(strFileName);

    cauto strDstFile = _bkgDir() + strFileName;
    pmBkg.save(__WS2Q(strDstFile), "JPG");

    this->close();
    m_addbkgDlg.close();
#if __windows
    m_app.setForeground();
#endif
}

void CBkgDlg::deleleBkg(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx < vecBkgFile.size())
    {
        cauto bkgFile = vecBkgFile[uIdx];

        cauto strBkg = m_bHScreen ? m_app.getOption().strHBkg : m_app.getOption().strVBkg;
        if (strBkg == bkgFile.strPath)
        {
            _setBkg(L"");
        }

        if (bkgFile.pm)
        {
            for (auto itr = m_lstPixmap.begin(); itr != m_lstPixmap.end(); ++itr)
            {
                if (&*itr == bkgFile.pm)
                {
                    m_lstPixmap.erase(itr);
                    break;
                }
            }
        }

        fsutil::removeFile(_bkgDir() + bkgFile.strPath);
        vecBkgFile.erase(vecBkgFile.begin()+uIdx);

        m_lv.update();
    }
}

void CBkgDlg::_onClosed()
{
    set<const QPixmap*> setDelelePixmap;
    if (m_vecHBkgFile.size() > __snapshotRetain)
    {
        for (auto itr = m_vecHBkgFile.begin()+__snapshotRetain; itr != m_vecHBkgFile.end(); ++itr)
        {
            setDelelePixmap.insert(itr->pm);
            itr->pm = NULL;
        }
    }

    if (m_vecVBkgFile.size() > __snapshotRetain)
    {
        for (auto itr = m_vecVBkgFile.begin()+__snapshotRetain; itr != m_vecVBkgFile.end(); ++itr)
        {
            setDelelePixmap.insert(itr->pm);
            itr->pm = NULL;
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

    m_lv.reset();
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
    if (!pm.load(__WS2Q(subFile.path())))
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

void CImgDir::scan(const wstring& strDir, cfn_void_t<CImgDir&> cb)
{
    if (m_thread.joinable())
    {
        return;
    }

    CPath::setDir(strDir);

    m_thread.start([&, cb](const bool& bRunSignal){
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
    });
}

void CImgDir::_onClear()
{
    m_thread.cancel();

    m_vecSubImgs.clear();
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
        return CPath::path();
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
        m_vecSubImgs.emplace_back(QPixmap(), pSubFile->path());
        m_vecSubImgs.back().first.swap(pm);
    }

    ++m_itrSubFile;

    return true;
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
