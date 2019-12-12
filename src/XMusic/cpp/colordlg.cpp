#include "app.h"

#include "colordlg.h"
#include "ui_colordlg.h"

static Ui::ColorDlg ui;

CColorDlg::CColorDlg(class CXMusicApp& app, CBkgDlg& bkgDlg) : CDialog((QWidget&)bkgDlg),
    m_app(app)
{
    ui.setupUi(this);

    ui.barBkgRed->setColor(E_BarColor::BC_Red);
    ui.barBkgGreen->setColor(E_BarColor::BC_Green);
    ui.barBkgBlue->setColor(E_BarColor::BC_Blue);
    ui.barFontRed->setColor(E_BarColor::BC_Red);
    ui.barFontGreen->setColor(E_BarColor::BC_Green);
    ui.barFontBlue->setColor(E_BarColor::BC_Blue);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);
}

void CColorDlg::_relayout(int cx, int cy)
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

#define __margin 100
    cauto yClient = rcReturn.bottom() + __margin;

    if (cy > cx)
    {
        ui.groupBkgColor->setGeometry(__margin, yClient, cx-__margin*2, (cy-yClient)/2-__margin);

        ui.groupFontColor->move(__margin, ui.groupBkgColor->geometry().bottom()+__margin);
    }
    else
    {
        ui.groupBkgColor->setGeometry(__margin, yClient, cx/2-__margin*1.5, cy-__margin-yClient);

        ui.groupFontColor->move(cx/2+__margin/2, yClient);
    }
    ui.groupFontColor->resize(ui.groupBkgColor->size());

    int cx_group = ui.groupBkgColor->rect().width();
    int cy_group = ui.groupBkgColor->rect().height();

    int x = ui.btnSubBkgRed->x();
    int y = cy_group/3.85;

    ui.btnSubBkgRed->move(x, y-ui.btnSubBkgRed->height()/2);
    ui.btnAddBkgRed->move(cx_group-x-ui.btnAddBkgRed->width(), ui.btnSubBkgRed->y());

    int x_bar = ui.barBkgRed->x();
    int cx_bar = cx_group-x_bar*2;
    int cy_bar = ui.barBkgRed->height();
    ui.barBkgRed->setGeometry(x_bar, y-cy_bar/2, cx_bar, cy_bar);

    ui.btnSubBkgGreen->move(x, ui.btnSubBkgRed->y()+y);
    ui.btnAddBkgGreen->move(ui.btnAddBkgRed->x(), ui.btnSubBkgGreen->y());
    ui.barBkgGreen->setGeometry(x_bar, ui.barBkgRed->y()+y, cx_bar, cy_bar);

    y+=y;
    ui.btnSubBkgBlue->move(x, ui.btnSubBkgRed->y()+y);
    ui.btnAddBkgBlue->move(ui.btnAddBkgRed->x(), ui.btnSubBkgBlue->y());
    ui.barBkgBlue->setGeometry(x_bar, ui.barBkgRed->y()+y, cx_bar, cy_bar);

    ui.btnSubFontRed->setGeometry(ui.btnSubBkgRed->geometry());
    ui.btnAddFontRed->setGeometry(ui.btnAddBkgRed->geometry());
    ui.barFontRed->setGeometry(ui.barBkgRed->geometry());

    ui.btnSubFontBlue->setGeometry(ui.btnSubBkgBlue->geometry());
    ui.btnAddFontBlue->setGeometry(ui.btnAddBkgBlue->geometry());
    ui.barFontBlue->setGeometry(ui.barBkgBlue->geometry());

    ui.btnSubFontGreen->setGeometry(ui.btnSubBkgGreen->geometry());
    ui.btnAddFontGreen->setGeometry(ui.btnAddBkgGreen->geometry());
    ui.barFontGreen->setGeometry(ui.barBkgGreen->geometry());
}

void CColorDlg::show()
{
    ui.labelTitle->setTextColor(m_crText);
    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

    SList<QWidget*> lstWidget {ui.groupBkgColor, ui.btnSubBkgRed, ui.btnSubBkgGreen, ui.btnSubBkgBlue
                              , ui.groupFontColor, ui.btnSubFontRed, ui.btnSubFontGreen, ui.btnSubFontBlue};
    for (auto widget : lstWidget)
    {
        QPalette pe = ui.btnSubBkgRed->palette();
        pe.setColor(QPalette::WindowText, m_crText);
        widget->setPalette(pe);
    }

    CDialog::show(true);
}
