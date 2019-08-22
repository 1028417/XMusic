#include "view.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg() : m_addbkgView(*this)
{
}

void CAddBkgDlg::init()
{
    ui.setupUi(this);

    QColor crText(32, 128, 255);
    ui.labelTitle->setTextColor(crText);
    ui.labelTitle->setFont(2, E_FontWeight::FW_SemiBold);

    m_addbkgView.setTextColor(crText);
    m_addbkgView.setFont(0.5);

    ui.btnUpward->setVisible(false);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_addbkgView.upward();
    });

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });

    connect(this, &CAddBkgDlg::signal_founddir, this, &CAddBkgDlg::slot_founddir);

    m_sdcard.SetDir(L"c:/");
}

void CAddBkgDlg::show()
{
    if (m_thread.joinable())
    {
        m_thread.join();
    }

    m_thread = std::thread([&](){
        m_sdcard.scan([&](CPath& dir, TD_PathList& paSubFile) {
            if (paSubFile)
            {
                emit signal_founddir(&dir);
            }

            return true;
        });

    });

    CDialog<>::show();
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
        return false;
    }

    m_addbkgView.upward();

    return true;
}

CAddBkgView::CAddBkgView(CAddBkgDlg& addbkgDlg)
    : CListView(&addbkgDlg)
    , m_addbkgDlg(addbkgDlg)
{
}

UINT CAddBkgView::getRowCount()
{
    if (m_pDir)
    {
        return m_pDir->files().size();
    }

    return m_paDirs.size();
}

UINT CAddBkgView::getColumnCount()
{
    if (m_pDir)
    {

    }

    return 1;
}

void CAddBkgView::_onPaintRow(CPainter&, QRect&, const tagLVRow&)
{

}



