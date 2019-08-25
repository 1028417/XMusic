#include "app.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg) :
    m_bkgDlg(bkgDlg)
    , m_addbkgView(*this, m_paImgDirs)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(2, E_FontWeight::FW_SemiBold);

    m_addbkgView.setTextColor(crText);
#if __android
    m_addbkgView.setFont(0.5);
#endif

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        (void)this->_handleReturn();
    });

    connect(this, &CAddBkgDlg::signal_founddir, this, &CAddBkgDlg::slot_founddir);
}

void CAddBkgDlg::show()
{
    CDialog::show();

#if !__android
    m_ImgRoot.SetDir(L"D:/dev/cpp/XMusic/bin");
#endif
    m_ImgRoot.startScan([&](CPath& dir) {
        CImgDir& imgDir = (CImgDir&)dir;
        if (imgDir.genSnapshot())
        {
            emit signal_founddir(&imgDir);
        }
    });
}

void CAddBkgDlg::close()
{
    m_ImgRoot.stopScan();

    m_paImgDirs.clear();
    m_ImgRoot.Clear();

    CDialog::close();
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    int y_addbkgView = ui.btnReturn->geometry().bottom() + ui.btnReturn->y();
    m_addbkgView.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);
}

bool CAddBkgDlg::_handleReturn()
{
    if (!m_addbkgView.upward())
    {
        this->close();
    }

    return true;
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


CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg, const TD_ImgDirList& paImgDir)
    : CListView(&addbkgDlg)
    , m_addbkgDlg(addbkgDlg)
    , m_paImgDirs(paImgDir)
{
}

UINT CAddBkgView::_picLayoutCount() const
{
    if (m_pImgDir)
    {
        if (m_pImgDir->files().size() <= 4)
        {
            return 2;
        }
    }

    return 3;
}

UINT CAddBkgView::getColumnCount()
{
    if (m_pImgDir)
    {
        return _picLayoutCount();
    }

    return 1;
}

UINT CAddBkgView::getPageRowCount()
{
    if (m_pImgDir)
    {
        return _picLayoutCount();
    }

    UINT uRet = 8;
    int cy = m_addbkgDlg.height();
    if (cy > 2160)
    {
       uRet++;

        if (cy > 2340)
        {
           uRet++;
        }
    }
    else if (cy < 1800)
    {
        uRet--;
        uRet = ceil((float)uRet*m_addbkgDlg.height()/1800);
    }
    return uRet;
}

UINT CAddBkgView::getRowCount()
{
    if (m_pImgDir)
    {
        return (UINT)ceil(m_pImgDir->files().size()/_picLayoutCount());
    }

    return m_paImgDirs.size();
}

void CAddBkgView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    auto uRow = lvRow.uRow;
    if (m_pImgDir)
    {
        UINT uIdx = uRow * _picLayoutCount() + lvRow.uCol;

        m_pImgDir->files().get(uIdx, [&](CPath& subFile){

        });
    }
    else
    {
        tagRowContext context;
        context.eStyle = E_RowStyle::IS_RightTip;

        m_paImgDirs.get(uRow, [&](CImgDir& imgDir){
            context.strText = imgDir.oppPath();
            context.pixmap = &imgDir.snapshot();
        });

        _paintRow(painter, rc, lvRow, context);
    }
}

void CAddBkgView::_onRowClick(const tagLVRow& lvRow, const QMouseEvent&)
{
    if (m_pImgDir)
    {
    }
    else
    {
        _saveScrollRecord(NULL);

        m_paImgDirs.get(lvRow.uRow, [&](CImgDir& imgDir){
            imgDir.genSubImgs();

            m_pImgDir = &imgDir;
        });

        update();
    }
}

bool CAddBkgView::upward()
{
    if (m_pImgDir)
    {
        m_pImgDir = NULL;
        update();

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

#define __filterSize 720

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

#define __zoomoutSize 150
        if (_loadImg(*pSubFile, m_pmSnapshot, __zoomoutSize))
        {
            return true;
        }
    }

    return false;
}

void CImgDir::genSubImgs()
{
    cauto& files = CPath::files();
    for (; m_itrSubFile != files.end(); ++m_itrSubFile)
    {
        auto pSubFile = *m_itrSubFile;

        QPixmap pm;
        if (_loadImg(*pSubFile, pm, 600))
        {
            m_lstSubImgs.push_back({pSubFile, QPixmap()});
            auto& pr = m_lstSubImgs.back();
            pr.second.swap(pm);
        }
    }
}
