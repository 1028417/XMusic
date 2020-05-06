#include "app.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

#define __snapshotRetain 7

static int g_xsize = 0;

static Ui::BkgDlg ui;

CBkgView::CBkgView(class CApp& app, CBkgDlg& bkgDlg)
    : CListView(bkgDlg),
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
            auto br = m_bkgDlg.brush(uItem-2);
            if (br)
            {
                painter.drawPixmapEx(rc, *br, QRect(0,0,br->m_cx,br->m_cy), __szRound);

                QRect rcX(rc.right()-g_xsize-5, rc.top()+5, g_xsize, g_xsize);
                painter.drawPixmap(rcX, m_pmX);

                static UINT s_uSequence = 0;
                s_uSequence++;
                if (lvRow.uCol == uColumnCount-1)
                {
                    auto uPageRowCount = getPageRowCount();
                    UINT uFloorRow = ceil(scrollPos()+uPageRowCount-1);
                    if ( lvRow.uRow == uFloorRow)
                    {
                        auto uSequence = s_uSequence;
                        CApp::async(300, [=](){
                            if (uSequence != s_uSequence || !isVisible())
                            {
                                return;
                            }

                            for (UINT uCol=0; uCol<uColumnCount; uCol++)
                            {
                                auto uItem = (uFloorRow+1) * uColumnCount + uCol;
                                if (uItem >= 2)
                                {
                                    m_bkgDlg.brush(uItem-2);
                                }
                            }
                        });
                    }
                }

                return;
            }

            if (uItem < 9)
            {
                return;
            }
        }

        rc.setLeft(rc.left()+2);
        rc.setRight(rc.right()-1);

        /*int r = g_crBkg.red();
        int g = g_crBkg.green();
        int b = g_crBkg.blue();
        QColor cr(r<128?r+128:r-128, g<128?g+128:g-128, b<128?b+128:b-128);
        int d = abs(cr.red()+cr.green()+cr.blue()-g_crBkg.red()-g_crBkg.green()-g_crBkg.blue());
        if (abs(d) == 128)
        {
            cr.setAlpha(100);
        }
        else
        {
            cr.setAlpha(50);
        }*/
        auto cr = g_crFore;
        cr.setAlpha(100);

        painter.drawRectEx(rc, cr, 2, Qt::PenStyle::DotLine, __szRound);

        cr.setAlpha(128);

        int len = MIN(rc.width(), rc.height())/4;
#define __szAdd __size(4)
        cauto pt = rc.center();
        rc.setRect(pt.x()-len/2, pt.y()-__szAdd/2, len, __szAdd);
        painter.fillRectEx(rc, cr, __szAdd/2);

        rc.setRect(pt.x()-__szAdd/2, pt.y()-len/2, __szAdd, len);
        painter.fillRect(rc, g_crBkg);
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

inline static void zoomoutPixmap(QPixmap &pm, int cx, int cy)
{
    if (pm.isNull())
    {
        return;
    }

    if ((float)pm.height()/pm.width() > (float)cy/cx)
    {
        if (pm.width() > cx)
        {
            auto&& temp = pm.scaledToWidth(cx, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
    else
    {
        if (pm.height() > cy)
        {
            auto&& temp = pm.scaledToHeight(cy, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
}

static void zoomoutPixmap(bool bHBkg, QPixmap& pm, UINT uDiv=0)
{
    int cx = 0;
    int cy = 0;
    if (bHBkg)
    {
        cx = g_szScreenMax;
        cy = g_szScreenMin;
    }
    else
    {
        cx = g_szScreenMin;
        cy = g_szScreenMax;
    }

    if (uDiv > 1)
    {
        cx = 0.9f*cx/uDiv;
        cy = 0.9f*cy/uDiv;
    }

    zoomoutPixmap(pm, cx, cy);
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

            QPixmap pm;
            for (wchar_t wch = L'a'; wch <= L'z'; wch++)
            {
                auto strBkg = strBkgSrc + wch + L".jpg";
                auto strDstFile = strAppBkgDir + __wcPathSeparator + L"0." + wch;
                if (pm.load(__WS2Q(strBkg)))
                {
                    zoomoutPixmap(bHBkg, pm);
                    pm.save(__WS2Q(strDstFile), "JPG", 89);
                    mtutil::usleep(10);
                }

                strBkg = strBkgSrc + (bHBkg?L"hbkg/":L"vbkg/") + wch + L".jpg";
                strDstFile = strAppBkgDir + __wcPathSeparator + L"1." + wch;
                if (fsutil::copyFile(strBkg, strDstFile))
                {
                    mtutil::usleep(10);
                }

                strBkg = strBkgSrc + (bHBkg?L"hbkg/city/":L"vbkg/city/") + wch + L".jpg";
                strDstFile = strAppBkgDir + __wcPathSeparator + L"2." + wch;
                if (fsutil::copyFile(strBkg, strDstFile))
                {
                    mtutil::usleep(10);
                }
            }
        }

        auto& vecBkgFile = bHBkg?m_vecHBkgFile:m_vecVBkgFile;
        fsutil::findSubFile(strAppBkgDir, [&](cwstr strSubFile) {
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
            QPixmap pm(strBkgDir + itr->strPath);
            itr->br = &_addbr(pm);

            mtutil::usleep(10);
        }

        wstring strHBkg = m_app.getOption().strHBkg;
        if (!strHBkg.empty())
        {
            (void)m_pmHBkg.load(__WS2Q(m_strHBkgDir + strHBkg));
        }

        wstring strVBkg = m_app.getOption().strVBkg;
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

    if (m_bHLayout)
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

    static BOOL bHLayout = -1;
    if (bHLayout != (BOOL)m_bHLayout)
    {
        bHLayout = m_bHLayout;
        m_lv.scroll(0);
    }
}

inline wstring& CBkgDlg::_bkgDir()
{
    return m_bHLayout?m_strHBkgDir:m_strVBkgDir;
}

inline vector<tagBkgFile>& CBkgDlg::_vecBkgFile()
{
    return m_bHLayout?m_vecHBkgFile:m_vecVBkgFile;
}

inline CBkgBrush& CBkgDlg::_addbr(QPixmap& pm)
{
    zoomoutPixmap(m_bHLayout, pm, 2);

    m_lstBr.emplace_back(pm);

    return m_lstBr.back();
}

size_t CBkgDlg::bkgCount() const
{
    return (m_bHLayout?m_vecHBkgFile:m_vecVBkgFile).size();
}

CBkgBrush* CBkgDlg::brush(size_t uIdx)
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

    cauto strBkgFile = _bkgDir() + bkgFile.strPath;

    QPixmap pm(strBkgFile);
    auto& br = _addbr(pm);
    return bkgFile.br = &br;
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
        cwstr strFile = vecBkgFile[nIdx].strPath;
        (void)pm.load(__WS2Q(_bkgDir() + strFile));
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
    m_app.getOption().bUseBkgColor = false;
    if (m_bHLayout)
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
    if (0 == uItem)
    {
        _showAddBkg();
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

    m_addbkgDlg.show([&](){
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

void CBkgDlg::addBkg(cwstr strFile)
{
    static bool bFlag = false;
    if (bFlag)
    {
        return;
    }
    bFlag = true;

    auto& pm = m_bHLayout?m_pmHBkg:m_pmVBkg;
    (void)pm.load(__WS2Q(strFile));
    zoomoutPixmap(m_bHLayout, pm);

    cauto strFileName = to_wstring(time(0));
    cauto strDstFile = _bkgDir() + strFileName;
    pm.save(__WS2Q(strDstFile), "JPG", 100);

    auto& vecBkgFile = _vecBkgFile();
    vecBkgFile.insert(vecBkgFile.begin(), tagBkgFile(false, strFileName, &_addbr(pm)));
    //update();

    _updateBkg(strFileName);
    this->close();
    m_addbkgDlg.close();
#if __windows
    m_app.setForeground();
#endif

    bFlag = false;
}

void CBkgDlg::deleleBkg(size_t uIdx)
{
    auto& vecBkgFile = _vecBkgFile();
    if (uIdx < vecBkgFile.size())
    {
        cauto bkgFile = vecBkgFile[uIdx];

        cauto strBkg = m_bHLayout ? m_app.getOption().strHBkg : m_app.getOption().strVBkg;
        if (strBkg == bkgFile.strPath)
        {
            _setBkg(-1);
        }

        if (bkgFile.br)
        {
            for (auto itr = m_lstBr.begin(); itr != m_lstBr.end(); ++itr)
            {
                if (&*itr == bkgFile.br)
                {
                    m_lstBr.erase(itr);
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
    set<CBkgBrush*> setDeleleBrush;
    if (m_vecHBkgFile.size() > __snapshotRetain)
    {
        for (auto itr = m_vecHBkgFile.begin()+__snapshotRetain; itr != m_vecHBkgFile.end(); ++itr)
        {
            setDeleleBrush.insert(itr->br);
            itr->br = NULL;
        }
    }

    if (m_vecVBkgFile.size() > __snapshotRetain)
    {
        for (auto itr = m_vecVBkgFile.begin()+__snapshotRetain; itr != m_vecVBkgFile.end(); ++itr)
        {
            setDeleleBrush.insert(itr->br);
            itr->br = NULL;
        }
    }

    for (auto itr = m_lstBr.begin(); itr != m_lstBr.end(); )
    {
        if (setDeleleBrush.find(&*itr) != setDeleleBrush.end())
        {
           itr = m_lstBr.erase(itr);
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

#define __szSubIngFilter 640

inline static bool _loadSubImg(XFile& subFile, QPixmap& pm)
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

void CImgDir::scan(cwstr strDir, cfn_void_t<CImgDir&> cb)
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

    m_vecImgs.clear();
}

#define __szSnapshot 160

bool CImgDir::_genSnapshot(TD_XFileList& paSubFile)
{
    for (m_itrSubFile = paSubFile.begin(); m_itrSubFile != paSubFile.end(); ++m_itrSubFile)
    {
        if (_loadSubImg(**m_itrSubFile, m_pmSnapshot))
        {
            auto&& temp = m_pmSnapshot.width() < m_pmSnapshot.height()
                    ? m_pmSnapshot.scaledToWidth(__szSnapshot, Qt::SmoothTransformation)
                    : m_pmSnapshot.scaledToHeight(__szSnapshot, Qt::SmoothTransformation);
            m_pmSnapshot.swap(temp);

            //m_vecSubImgs.reserve(paSubFile.size());
            return true;
        }
        m_pmSnapshot = QPixmap();
    }

    return false;
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

bool CImgDir::genSubImgs()
{
    cauto files = CPath::files();
    if (files.end() == m_itrSubFile)
    {
        return false;
    }

    auto pSubFile = *m_itrSubFile;

    QPixmap pm;
    if (_loadSubImg(*pSubFile, pm))
    {
        int szZoomout = g_szScreenMax*0.9f;
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
        m_vecImgs.emplace_back(pm, pSubFile->path());
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
