#include "app.h"

#include "bkgdlg.h"
#include "ui_bkgdlg.h"

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

            painter.drawRectEx(rc, 2, cr, Qt::PenStyle::DotLine, __szRound);
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

CBkgDlg::CBkgDlg(class CApp& app)
    : m_app(app),
    m_colorDlg(app, *this),
    m_addbkgDlg(app, *this),
    m_bkgView(app, *this)
{
}

void CBkgDlg::init()
{
    ui.setupUi(this);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnColor, &CButton::signal_clicked, [&]() {
        m_colorDlg.show();
    });

#define __hbkgdir L"/bkg/hbkg/"
#define __vbkgdir L"/bkg/vbkg/"

    m_strHBkgDir = fsutil::workDir() + __hbkgdir;
    m_strVBkgDir = fsutil::workDir() + __vbkgdir;

    auto strFileName = to_wstring(time(NULL));

    if (!fsutil::existDir(m_strHBkgDir))
    {
        if (fsutil::createDir(m_strHBkgDir))
        {
            fsutil::copyFile(L"assets:" __hbkgdir L"win10" , m_strHBkgDir + strFileName);
        }
    }

    if (!fsutil::existDir(m_strVBkgDir))
    {
        if (fsutil::createDir(m_strVBkgDir))
        {
            fsutil::copyFile(L"assets:" __vbkgdir L"win10", m_strVBkgDir + strFileName);
        }
    }

    fsutil::findSubFile(m_strHBkgDir, [&](const wstring& strSubFile) {
        m_vecHBkgFile.push_back(strSubFile);
    });

    fsutil::findSubFile(m_strVBkgDir, [&](const wstring& strSubFile) {
        m_vecVBkgFile.push_back(strSubFile);
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

void CBkgDlg::show()
{
    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

    CDialog::show(m_app.mainWnd());
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

const QPixmap* CBkgDlg::pixmap(size_t uIdx)
{
    auto& vecPixmap = _vecPixmap();
    if (uIdx < vecPixmap.size())
    {
        return vecPixmap[uIdx];
    }

    auto& vecBkgFile = _vecBkgFile();
    if (uIdx >= vecBkgFile.size())
    {
        return NULL;
    }
    cauto strBkgFile = _bkgDir() + vecBkgFile[uIdx];

    m_lstPixmap.emplace_back(QPixmap());
    auto& pm = m_lstPixmap.back();
    if (pm.load(strBkgFile))
    {
#define __snapshotZoomout 900
        CPainter::zoomoutPixmap(pm, __snapshotZoomout);
    }

    vecPixmap.push_back(&pm);
    return &pm;
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
            _setBkg(vecBkgFile[uIdx]);
            close();
        }
        else
        {
            _showAddBkg();
        }

        /*m_thread.start([&](const bool& bRunSignal){
        });

        m_addbkgDlg.show(&m_olImgDir, [&](){
            m_thread.cancel();

            m_olImgDir.clear();
        });*/
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

    m_rootImgDir.setDir(strImgDir);

    m_thread.start([&](const bool& bRunSignal){
        m_rootImgDir.scan([&](CPath& dir, TD_XFileList& paSubFile) {
            if (paSubFile)
            {
                auto& imgDir = (CImgDir&)dir;
                if (imgDir.genSnapshot())
                {
                    if (bRunSignal)
                    {
                        m_addbkgDlg.addImgDir(imgDir);
                    }
                }
            }

            return bRunSignal;
        });
    });

    m_addbkgDlg.show(NULL, [&](){
        m_thread.cancel();

        m_rootImgDir.clear();
    });
}

void CBkgDlg::addBkg(const wstring& strFile)
{
    wstring strFileName = to_wstring(time(NULL));
    if (!fsutil::copyFile(strFile, _bkgDir() + strFileName))
    {
        return;
    }

    _vecBkgFile().push_back(strFileName);
    m_bkgView.update();

    _setBkg(strFileName);
    close();
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
        auto& strBkg = m_bHScreen ? m_app.getOption().strHBkg
                                  : m_app.getOption().strVBkg;
        if (strBkg == *vecBkgFile[uIdx])
        {
            _setBkg(L"");
        }

        fsutil::removeFile(_bkgDir() + vecBkgFile[uIdx]);
        vecBkgFile.erase(vecBkgFile.begin()+uIdx);

        auto& vecPixmap = _vecPixmap();
        if (uIdx < vecPixmap.size())
        {
            auto pm = vecPixmap[uIdx];
            vecPixmap.erase(vecPixmap.begin()+uIdx);

            for (auto itr = m_lstPixmap.begin(); itr != m_lstPixmap.end(); ++itr)
            {
                if (&*itr == pm)
                {
                    m_lstPixmap.erase(itr);
                    break;
                }
            }
        }

        m_bkgView.update();
    }
}

void CBkgDlg::_onClose()
{
    m_vecHPixmap.clear();
    m_vecVPixmap.clear();
    m_lstPixmap.clear();

    m_bkgView.reset();
}

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpeg", L"png", L"bmp");

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

bool CImgDir::genSnapshot()
{
    cauto files = CPath::files();
    for (m_itrSubFile = files.begin(); m_itrSubFile != files.end(); ++m_itrSubFile)
    {
        auto pSubFile = *m_itrSubFile;

#define __snapshotSize 150
        if (_loadImg(*pSubFile, m_pmSnapshot, __snapshotSize))
        {
            return true;
        }
    }

    return false;
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
        m_lstSubImgs.emplace_back(QPixmap(), pSubFile->absPath());
        m_lstSubImgs.back().first.swap(pm);
    }

    ++m_itrSubFile;

    return true;
}
