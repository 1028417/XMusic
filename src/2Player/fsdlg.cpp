#include "view.h"

#include "fsdlg.h"
#include "ui_fsdlg.h"

static Ui::FsDlg ui;

CFsDlg::CFsDlg()
{
    ui.setupUi(this);

    /*m_fsView.setTextColor(crText);
    m_fsView.setFont(0.5);
    m_fsView.init();*/

    ui.btnUpward->setVisible(false);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        //(void)m_MedialibView.handleReturn();
    });

    connect(ui.btnReturn, &CButton::signal_clicked, [&](CButton*) {
        this->close();
    });
}

void CFsDlg::_relayout(int cx, int cy)
{
    cauto& rcReturn = ui.btnReturn->geometry();
    int y_margin = rcReturn.top();

    QRect rcUpward(cx-rcReturn.right(), y_margin, rcReturn.width(), rcReturn.height());
    ui.btnUpward->setGeometry(rcUpward);

    //_resizeTitle();

    //int y_FsView = rcReturn.bottom() + y_margin;
    //m_MedialibView.setGeometry(0, y_FsView, cx, cy-y_FsView);
}
