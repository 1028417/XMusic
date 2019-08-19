#include "view.h"

#include "addbkgdlg.h"
#include "ui_addbkgdlg.h"

static Ui::AddBkgDlg ui;

CAddBkgDlg::CAddBkgDlg()
    : m_addbkgView(*this)
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
    //m_addbkgView.init();

    ui.btnUpward->setVisible(false);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_addbkgView.upward();
    });

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });
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
    : CListViewEx(&addbkgDlg)
    , m_addbkgDlg(addbkgDlg)
{
}

UINT CAddBkgView::getRootCount()
{
    return 0;
}

bool CAddBkgView::_genRootRowContext(const tagLVRow&, tagRowContext&)
{
    return false;
}
