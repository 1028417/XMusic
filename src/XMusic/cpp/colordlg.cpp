#include "app.h"

#include "colordlg.h"
#include "ui_colordlg.h"

static Ui::ColorDlg ui;

CColorDlg::CColorDlg(class CXMusicApp& app, CBkgDlg& bkgDlg) : CDialog((QWidget&)bkgDlg),
    m_app(app)
{
    ui.setupUi(this);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    for (auto pButton : SList<CButton*>({ui.btnSubBkgRed, ui.btnAddBkgRed, ui.btnSubBkgGreen, ui.btnAddBkgGreen
                                        , ui.btnSubBkgBlue, ui.btnAddBkgBlue, ui.btnSubFontRed, ui.btnAddFontRed
                                        , ui.btnSubFontGreen, ui.btnAddFontGreen, ui.btnSubFontBlue, ui.btnAddFontBlue}))
    {
        connect(pButton, &CButton::signal_clicked, this, &CColorDlg::slot_buttonClicked);
    }

    for (auto pBar : SList<CColorBar*>({ui.barBkgRed, ui.barBkgGreen, ui.barFontBlue
                                       , ui.barFontRed, ui.barFontGreen, ui.barFontBlue}))
    {
        connect(pBar, &CColorBar::signal_click, this, &CColorDlg::slot_barClicked);
    }
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

    ui.barBkgRed->setColor(E_BarColor::BC_Red, 0);
    ui.barBkgGreen->setColor(E_BarColor::BC_Green, 0);
    ui.barBkgBlue->setColor(E_BarColor::BC_Blue, 0);
    ui.barFontRed->setColor(E_BarColor::BC_Red, 0);
    ui.barFontGreen->setColor(E_BarColor::BC_Green, 0);
    ui.barFontBlue->setColor(E_BarColor::BC_Blue, 0);

    CDialog::show(true);
}

void CColorDlg::slot_buttonClicked(CButton *pButton)
{
    if (ui.btnSubBkgRed == pButton)
    {
        modifyColor(ui.barBkgRed, -1);
    }
    else if (ui.btnAddBkgRed == pButton)
    {
        modifyColor(ui.barBkgRed, 1);
    }
    else if (ui.btnSubBkgGreen == pButton)
    {
        modifyColor(ui.barBkgGreen, -1);
    }
    else if (ui.btnAddBkgGreen == pButton)
    {
        modifyColor(ui.barBkgGreen, 1);
    }
    else if (ui.btnSubBkgBlue == pButton)
    {
        modifyColor(ui.barBkgBlue, -1);
    }
    else if (ui.btnAddBkgBlue == pButton)
    {
        modifyColor(ui.barBkgBlue, 1);
    }
    else if (ui.btnSubFontRed == pButton)
    {
        modifyColor(ui.barFontRed, -1);
    }
    else if (ui.btnAddFontRed == pButton)
    {
        modifyColor(ui.barFontRed, 1);
    }
    else if (ui.btnSubFontGreen == pButton)
    {
        modifyColor(ui.barFontGreen, -1);
    }
    else if (ui.btnAddFontGreen == pButton)
    {
        modifyColor(ui.barFontGreen, -1);
    }
    else if (ui.btnSubFontBlue == pButton)
    {
        modifyColor(ui.barFontBlue, -1);
    }
    else if (ui.btnAddFontBlue == pButton)
    {
        modifyColor(ui.barFontBlue, -1);
    }
}

void CColorDlg::modifyColor(CColorBar *pBar, int8_t offset)
{
    int value = pBar->value();
    value += offset;
    value = MAX(0, value);
    value = MIN(255, value);
    pBar->setValue((uint8_t)value);
}

void CColorDlg::slot_barClicked(CColorBar *pBar, const QPoint& pos)
{
    pBar->setValue(pos.x()/pBar->width()*255);
}
