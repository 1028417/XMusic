#include "app.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg(class CXMusicApp& app, CBkgDlg& bkgDlg) :
    //CDialog(bkgDlg),
    m_app(app),
    m_bkgDlg(bkgDlg),
    m_addbkgView(app, *this, m_paImgDirs)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

#if __android || __ios
    m_addbkgView.setFont(1.05);
#endif

    m_addbkgView.setTextColor(crText);

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });

    connect(this, &CAddBkgDlg::signal_founddir, this, &CAddBkgDlg::slot_founddir);
}

void CAddBkgDlg::show()
{
    m_paImgDirs.clear();

    CDialog::show();

    m_ImgRoot.startScan([&](CPath& dir) {
        CImgDir& imgDir = (CImgDir&)dir;
        if (imgDir.genSnapshot())
        {
            emit signal_founddir(&imgDir);
        }
    });
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    static const QRect rcReturnPrev = ui.btnReturn->geometry();
    QRect rcReturn = __rect(rcReturnPrev);
    if (cy >= __size(812*3)) // 针对全面屏刘海作偏移
    {
#define __yOffset __size(66)
        rcReturn.setTop(rcReturn.top() + __yOffset);
        rcReturn.setBottom(rcReturn.bottom() + __yOffset);
    }
    ui.btnReturn->setGeometry(rcReturn);

    int y_addbkgView = 0;
    if (m_addbkgView.isInRoot())
    {
        y_addbkgView = rcReturn.bottom() + rcReturn.top();
    }
    m_addbkgView.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);
}

bool CAddBkgDlg::_handleReturn()
{
    return m_addbkgView.upward();
}

void CAddBkgDlg::_onClose()
{
    m_ImgRoot.stopScan();

    m_paImgDirs.clear();

    m_addbkgView.clear();
}

void CAddBkgDlg::addBkg(const wstring& strFile)
{
    m_bkgDlg.addBkg(strFile);
    close();
}


map<void*, QPixmap> m_mapPixmaxp;
const QPixmap* getPixmap(CPath& path)
{
    auto itr = m_mapPixmaxp.find(&path);
    if (itr == m_mapPixmaxp.end())
    {
        if (!path.fileInfo().bDir)
        {
            auto& pm = m_mapPixmaxp[&path];
            pm.load(wsutil::toQStr(path.absPath()));
            return &pm;
        }

        return NULL;
    }

    return &itr->second;
}


CAddBkgView::CAddBkgView(class CXMusicApp& app, CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir) :
    CListView(&addbkgDlg)
    , m_app(app)
    , m_addbkgDlg(addbkgDlg)
    , m_paImgDirs(paImgDir)
{
}

size_t CAddBkgView::getColumnCount()
{
    if (m_pImgDir)
    {
        if (m_pImgDir->subImgs().size() <= 4)
        {
            return 2;
        }

        return 3;
    }

    return 1;
}

size_t CAddBkgView::getPageRowCount()
{
    if (m_pImgDir)
    {
        return getColumnCount();
    }

    UINT uRet = 8;
    int cy = m_addbkgDlg.height();
    if (cy > __size(2160))
    {
        uRet++;
        if (cy >= __size(2560))
        {
           uRet++;
        }
    }
    else if (cy < __size(1800))
    {
        uRet--;
        uRet = ceil((float)uRet*m_addbkgDlg.height()/__size(1800));
    }
    return uRet;
}

size_t CAddBkgView::getRowCount()
{
    if (m_pImgDir)
    {
        return (UINT)ceil((float)m_pImgDir->subImgs().size()/getColumnCount());
    }

    return m_paImgDirs.size();
}

void CAddBkgView::_onPaintRow(CPainter& painter, const tagLVRow& lvRow)
{
    cauto& rc = lvRow.rc;
    cauto& uRow = lvRow.uRow;
    if (m_pImgDir)
    {
        UINT uIdx = uRow * getColumnCount() + lvRow.uCol;

        cauto& subImgs = m_pImgDir->subImgs();
        if (uIdx < subImgs.size())
        {
            cauto& pm = subImgs[uIdx].second;
            painter.drawPixmapEx(rc, pm);

            QRect rcFrame(rc);
            rcFrame.setLeft(rcFrame.left()-1);
            rcFrame.setTop(rcFrame.top()-1);
            painter.drawFrame(1, rcFrame, 255,255,255,50);
        }
    }
    else
    {
        tagRowContext context;
        context.eStyle = E_RowStyle::IS_RightTip;

        m_paImgDirs.get(uRow, [&](CImgDir& imgDir){
            context.strText = imgDir.oppPath();
            context.pixmap = &imgDir.snapshot();
        });

        _paintRow(painter, lvRow, context);
    }
}

void CAddBkgView::_onRowClick(const tagLVRow& lvRow, const QMouseEvent&)
{
    if (m_pImgDir)
    {
        UINT uIdx = lvRow.uRow * getColumnCount() + lvRow.uCol;

        cauto& subImgs = m_pImgDir->subImgs();
        if (uIdx < subImgs.size())
        {
            m_addbkgDlg.addBkg(subImgs[uIdx].first->absPath());
        }
    }
    else
    {
        _saveScrollRecord(NULL);

        m_paImgDirs.get(lvRow.uRow, [&](CImgDir& imgDir){
            m_pImgDir = &imgDir;          
            update();

            m_app.setTimer(100, [=](){
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
        });

        m_addbkgDlg.relayout();
    }
}

bool CAddBkgView::upward()
{
    if (m_pImgDir)
    {
        m_pImgDir = NULL;
        reset();

        m_addbkgDlg.relayout();

        scroll(_scrollRecord(NULL));

        return true;
    }

    return false;
}

static const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpeg", L"bmp", L"png");

CPath* CImgDir::_newSubPath(const tagFileInfo& FileInfo)
{
    if (FileInfo.bDir)
    {
        return new CImgDir(FileInfo);
    }

    cauto& strExtName = wsutil::lowerCase_r(fsutil::GetFileExtName(FileInfo.strName));
    if (g_setImgExtName.includes(strExtName))
    {
        return new CPath(FileInfo);
    }

    return NULL;
}

#define __filterSize 640

inline static bool _loadImg(CPath& subFile, QPixmap& pm, UINT uZoomOutSize)
{
    if (!pm.load(wsutil::toQStr(subFile.absPath())))
    {
        return false;
    }

    if (pm.width()<__filterSize || pm.height()<__filterSize)
    {
        return false;
    }

    CPainter::zoomoutPixmap(pm, uZoomOutSize);

    return true;
}

bool CImgDir::genSnapshot()
{
    cauto& files = CPath::files();
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
    cauto& files = CPath::files();
    if (files.end() == m_itrSubFile)
    {
        return false;
    }

    auto pSubFile = *m_itrSubFile;

#define __zoomoutSize 600
    QPixmap pm;
    if (_loadImg(*pSubFile, pm, __zoomoutSize))
    {
        m_lstSubImgs.push_back({pSubFile, QPixmap()});
        auto& pr = m_lstSubImgs.back();
        pr.second.swap(pm);
    }

    ++m_itrSubFile;

    return true;
}
