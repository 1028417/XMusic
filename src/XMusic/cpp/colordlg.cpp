#include "app.h"

#include "colordlg.h"
#include "ui_colordlg.h"

static Ui::ColorDlg ui;

CColorDlg::CColorDlg(class CXMusicApp& app, CBkgDlg& bkgDlg) : CDialog((QWidget&)bkgDlg),
    m_app(app)
{
    ui.setupUi(this);

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

    QRect rcBkgGroup;
    QRect rcFontGroup;
    if (cy > cx)
    {
        rcBkgGroup.setRect(__margin, yClient, cx-__margin*2, (cy-yClient)/2-__margin);

        rcFontGroup.setRect(__margin, rcBkgGroup.bottom()+__margin, rcBkgGroup.width(), rcBkgGroup.height());
    }
    else
    {
        rcBkgGroup.setRect(__margin, yClient, cx/2-__margin*1.5, cy-__margin-yClient);

        rcFontGroup.setRect(cx/2+__margin/2, yClient, rcBkgGroup.width(), rcBkgGroup.height());
    }
    ui.groupBkgColor->setGeometry(rcBkgGroup);
    ui.groupFontColor->setGeometry(rcFontGroup);

    int y = rcBkgGroup.height()/4;
    ui.labelBkgRed->move(ui.labelBkgRed->x(), y-ui.labelBkgRed->height()/2);
    ui.sliderBkgRed->setGeometry(ui.sliderBkgRed->x(), y-ui.sliderBkgRed->height()/2
                              , rcBkgGroup.width()-ui.sliderBkgRed->x()-ui.labelBkgRed->x()
                              , ui.sliderBkgRed->height());

    ui.labelBkgGreen->move(ui.labelBkgRed->x(), ui.labelBkgRed->y()+y);
    ui.sliderBkgGreen->setGeometry(ui.sliderBkgRed->x(), ui.sliderBkgRed->y()+y
                                   , ui.sliderBkgRed->width(), ui.sliderBkgRed->height());

    y+=y;
    ui.labelBkgBlue->move(ui.labelBkgRed->x(), ui.labelBkgRed->y()+y);
    ui.sliderBkgBlue->setGeometry(ui.sliderBkgRed->x(), ui.sliderBkgRed->y()+y
                                   , ui.sliderBkgRed->width(), ui.sliderBkgRed->height());

    ui.labelFontRed->setGeometry(ui.labelBkgRed->geometry());
    ui.sliderFontRed->setGeometry(ui.sliderBkgRed->geometry());
    ui.labelFontBlue->setGeometry(ui.labelBkgBlue->geometry());
    ui.sliderFontBlue->setGeometry(ui.sliderBkgBlue->geometry());
    ui.labelFontGreen->setGeometry(ui.labelBkgGreen->geometry());
    ui.sliderFontGreen->setGeometry(ui.sliderBkgGreen->geometry());
}

void CColorDlg::show()
{
    ui.labelTitle->setTextColor(m_crText);
    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

    SList<QWidget*> lstWidget {ui.groupBkgColor, ui.labelBkgRed, ui.labelBkgGreen, ui.labelBkgBlue
                              , ui.groupFontColor, ui.labelFontRed, ui.labelFontGreen, ui.labelFontBlue};
    for (auto widget : lstWidget)
    {
        QPalette pe = ui.labelBkgRed->palette();
        pe.setColor(QPalette::WindowText, m_crText);
        widget->setPalette(pe);
    }

    CDialog::show(true);
}
