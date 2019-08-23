#include "view.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg()
    : m_addbkgView(*this, m_paDirs)
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

    ui.btnUpward->setVisible(false);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_addbkgView.upward();
    });

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });

    connect(this, &CAddBkgDlg::signal_founddir, this, &CAddBkgDlg::slot_founddir);
}

void CAddBkgDlg::show()
{
    CDialog::show();

    m_root.startScan([&](CPath& dir) {
        emit signal_founddir(&dir);
    });
}

void CAddBkgDlg::close()
{
    m_paDirs.clear();

    m_root.stopScan();
    m_root.Clear();

    CDialog::close();
}

void CAddBkgDlg::_relayout(int cx, int cy)
{
    cauto& rcReturn = ui.btnReturn->geometry();
    int y_margin = rcReturn.top();

    QRect rcUpward(cx-rcReturn.right(), y_margin, rcReturn.width(), rcReturn.height());
    ui.btnUpward->setGeometry(rcUpward);

    //_resizeTitle();

    int y_addbkgView = rcReturn.bottom() + y_margin;
    m_addbkgView.setGeometry(0, y_addbkgView, cx, cy-y_addbkgView);
}

bool CAddBkgDlg::_handleReturn()
{
    if (m_addbkgView.isInRoot())
    {
        this->close();
    }
    else
    {
        m_addbkgView.upward();
    }

    return true;
}


CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg, const TD_PathList& paDirs)
    : CListView(&addbkgDlg, 1, 10)
    , m_addbkgDlg(addbkgDlg)
    , m_paDirs(paDirs)
{
}

#define __PicColCount 4

UINT CAddBkgView::getRowCount()
{
    if (m_pDir)
    {
        return MAX(1, m_pDir->files().size()/__PicColCount);
    }

    return m_paDirs.size();
}

UINT CAddBkgView::getColumnCount()
{
    if (m_pDir)
    {
        return __PicColCount;
    }

    return 1;
}

void CAddBkgView::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    if (m_pDir)
    {
        UINT uIdx = lvRow.uRow;

        m_pDir->files().get(uIdx, [&](CPath& subFile){

        });
    }
    else
    {
        m_paDirs.get(lvRow.uRow, [&](CPath& dir){
            tagRowContext context(E_RowStyle::IS_RightTip, dir.GetName());
            _paintRow(painter, rc, lvRow, context);
        });
    }
}



