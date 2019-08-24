#include "app.h"

#include "bkgdlg.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

const SSet<wstring>& g_setImgExtName = SSet<wstring>(L"jpg", L"jpeg", L"bmp", L"png");

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg(CBkgDlg& bkgDlg) :
    m_bkgDlg(bkgDlg)
    , m_addbkgView(*this, m_plDirs)
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

    //m_ImgRoot.SetDir(L"D:/dev/cpp/XMusic/bin");
    m_ImgRoot.startScan([&](CPath& dir) {
        for (auto pSubFile : dir.files())
        {
            QPixmap pm;
            if (!pm.load(wsutil::toQStr(pSubFile->absPath())))
            {
                continue;
            }

#define __filterSize 300
            if (pm.width()<__filterSize || pm.height()<__filterSize)
            {
                continue;
            }

#define __zoomoutSize 150
            CPainter::zoomoutPixmap(pm, __zoomoutSize);

            m_lstPixmap.push_back(pm);

            emit signal_founddir(&dir, &m_lstPixmap.back());

            break;
        }
    });
}

void CAddBkgDlg::close()
{
    m_ImgRoot.stopScan();

    m_plDirs.clear();
    m_lstPixmap.clear();

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


CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg, const TD_ImgDirList& plDirs)
    : CListView(&addbkgDlg)
    , m_addbkgDlg(addbkgDlg)
    , m_plDirs(plDirs)
{
}

UINT CAddBkgView::_picLayoutCount() const
{
    if (m_pDir)
    {
        if (m_pDir->files().size() <= 4)
        {
            return 2;
        }
    }

    return 3;
}

UINT CAddBkgView::getColumnCount()
{
    if (m_pDir)
    {
        return _picLayoutCount();
    }

    return 1;
}

UINT CAddBkgView::getPageRowCount()
{
    if (m_pDir)
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
    if (m_pDir)
    {
        return (UINT)ceil(m_pDir->files().size()/_picLayoutCount());
    }

    return m_plDirs.size();
}

void CAddBkgView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    auto uRow = lvRow.uRow;
    if (m_pDir)
    {
        UINT uIdx = uRow * _picLayoutCount() + lvRow.uCol;

        m_pDir->files().get(uIdx, [&](CPath& subFile){

        });
    }
    else
    {
        tagRowContext context;
        context.eStyle = E_RowStyle::IS_RightTip;

        m_plDirs.get(uRow, [&](cauto& pr){
            context.strText = pr.first->oppPath();
            context.pixmap = pr.second;
        });

        _paintRow(painter, rc, lvRow, context);
    }
}

void CAddBkgView::_onRowClick(const tagLVRow& lvRow, const QMouseEvent&)
{
    if (m_pDir)
    {

    }
    else
    {
        m_plDirs.get(lvRow.uRow, [&](cauto& pr){
            m_pDir = pr.first;
        });
        update();
    }
}

bool CAddBkgView::upward()
{
    if (m_pDir)
    {
        m_pDir = NULL;
        update();

        return true;
    }

    return false;
}
