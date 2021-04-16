
#include "xmusic.h"

#include "colordlg.h"

#include "widget/colorbar.h"

#include "widget/groupframe.h"

#include "ui_colordlg.h"

static Ui::ColorDlg ui;

CColorDlg::CColorDlg(CBkgDlg& bkgDlg) : //CDialog(bkgDlg),
    m_bkgDlg(bkgDlg)
{
}

void CColorDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, TD_FontWeight::DemiBold);

    CFont font(1.08f);
    ui.groupBkgColor->setFont(font);
    ui.groupFontColor->setFont(font);

    ui.btnApplyBkgColor->setFont(font);

    ui.btnApplyBkgColor->onClicked([&]{
        ui.btnApplyBkgColor->setVisible(false);

         g_app.getOption().bUseBkgColor = true;

        this->update();
        m_bkgDlg.update();
         g_app.mainWnd().updateBkg();
    });

    this->regUISlot(&CColorDlg::slot_buttonClicked, &CButton::signal_clicked, {
                        ui.btnSubBkgRed, ui.btnAddBkgRed, ui.btnSubBkgGreen, ui.btnAddBkgGreen
                        , ui.btnSubBkgBlue, ui.btnAddBkgBlue, ui.btnSubFontRed, ui.btnAddFontRed
                        , ui.btnSubFontGreen, ui.btnAddFontGreen, ui.btnSubFontBlue, ui.btnAddFontBlue
                    });

    this->regUISlot(&CColorDlg::slot_barValueChanged, &CColorBar::signal_valueChanged, {
                        ui.barBkgRed, ui.barBkgGreen, ui.barBkgBlue
                        , ui.barFontRed, ui.barFontGreen, ui.barFontBlue
                    });

    ui.btnReturn->connect_dlgClose(this);
}

void CColorDlg::show()
{
    ui.btnApplyBkgColor->setVisible(! g_app.getOption().bUseBkgColor);

    setWidgetTextColor(ui.groupBkgColor, g_crFore);
    setWidgetTextColor(ui.groupFontColor, g_crFore);

    ui.barBkgRed->setColor(E_BarColor::BC_Red, g_crBkg.red());
    ui.barBkgGreen->setColor(E_BarColor::BC_Green, g_crBkg.green());
    ui.barBkgBlue->setColor(E_BarColor::BC_Blue, g_crBkg.blue());

    ui.barFontRed->setColor(E_BarColor::BC_Red, g_crFore.red());
    ui.barFontGreen->setColor(E_BarColor::BC_Green, g_crFore.green());
    ui.barFontBlue->setColor(E_BarColor::BC_Blue, g_crFore.blue());

    CDialog::show();
}

void CColorDlg::_relayout(int cx, int cy)
{
    int sz = MAX(cx, cy)/(CBkgDlg::caleRowCount(MAX(cx, cy))+1.6f);
    int cyMargin = sz/4;
    int cxMargin = cyMargin;
    QRect rcReturn(cxMargin, cyMargin, sz-cxMargin*2, sz-cyMargin*2);
    UINT cyBangs = checkIPhoneXBangs(cx, cy);
    if (cyBangs)
    {
        rcReturn.moveTop(cyBangs - cyMargin);
    }
    else
    {
        UINT cyStatusBar = checkAndroidStatusBar();
        if (cyStatusBar)
        {
            rcReturn.moveTop(cyMargin + cyStatusBar);
        }
    }
    ui.btnReturn->setGeometry(rcReturn);

    ui.labelTitle->move(rcReturn.right() + cxMargin, rcReturn.center().y() - ui.labelTitle->height()/2);

    cauto yClient = rcReturn.bottom() + cyMargin;

    int xFrame = cxMargin * 2;
    if (cy > cx)
    {
        ui.groupBkgColor->setGeometry(xFrame, yClient, cx-xFrame*2, (cy-yClient)/2-xFrame);

        ui.groupFontColor->move(xFrame, ui.groupBkgColor->geometry().bottom()+xFrame);
    }
    else
    {
        ui.groupBkgColor->setGeometry(xFrame, yClient, cx/2-xFrame*1.5, cy-xFrame-yClient);

        ui.groupFontColor->move(cx/2+xFrame/2, yClient);
    }
    ui.groupFontColor->resize(ui.groupBkgColor->size());

    int cx_group = ui.groupBkgColor->rect().width();
    int cy_group = ui.groupBkgColor->rect().height();

    ui.btnApplyBkgColor->move((cx_group-ui.btnApplyBkgColor->width())/2
                              , cy_group-__size(40)-ui.btnApplyBkgColor->height());

    int x = __size(40);
    int y = cy_group/3.94;

    int szBtn = rcReturn.width()-3;
    ui.btnSubBkgRed->setGeometry(x, y-ui.btnSubBkgRed->height()/2, szBtn, szBtn);
    ui.btnAddBkgRed->setGeometry(cx_group-x-ui.btnAddBkgRed->width(), ui.btnSubBkgRed->y(), szBtn, szBtn);

    int x_bar = 2*x + szBtn;
    int cx_bar = cx_group-x_bar*2+1;
    int cy_bar = __size(41);

    for (auto pBar : SList<CColorBar*>({ui.barBkgRed, ui.barBkgGreen, ui.barBkgBlue
                                       , ui.barFontRed, ui.barFontGreen, ui.barFontBlue}))
    {
        pBar->setMargin((cy_bar-__size10)/2);
    }

    ui.barBkgRed->setGeometry(x_bar, y-cy_bar/2, cx_bar, cy_bar);

    ui.btnSubBkgGreen->setGeometry(x, ui.btnSubBkgRed->y()+y, szBtn, szBtn);
    ui.btnAddBkgGreen->setGeometry(ui.btnAddBkgRed->x(), ui.btnSubBkgGreen->y(), szBtn, szBtn);
    ui.barBkgGreen->setGeometry(x_bar, ui.barBkgRed->y()+y, cx_bar, cy_bar);

    y+=y;
    ui.btnSubBkgBlue->setGeometry(x, ui.btnSubBkgRed->y()+y, szBtn, szBtn);
    ui.btnAddBkgBlue->setGeometry(ui.btnAddBkgRed->x(), ui.btnSubBkgBlue->y(), szBtn, szBtn);
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

static void _modifyColor(CColorBar *pBar, int8_t offset)
{
    int value = pBar->value();
    value += offset;
    value = MAX(0, value);
    value = MIN(255, value);
    pBar->setValue((uint8_t)value);
}

void CColorDlg::slot_buttonClicked(CButton *pButton)
{
    if (ui.btnSubBkgRed == pButton)
    {
        _modifyColor(ui.barBkgRed, -1);
    }
    else if (ui.btnAddBkgRed == pButton)
    {
        _modifyColor(ui.barBkgRed, 1);
    }
    else if (ui.btnSubBkgGreen == pButton)
    {
        _modifyColor(ui.barBkgGreen, -1);
    }
    else if (ui.btnAddBkgGreen == pButton)
    {
        _modifyColor(ui.barBkgGreen, 1);
    }
    else if (ui.btnSubBkgBlue == pButton)
    {
        _modifyColor(ui.barBkgBlue, -1);
    }
    else if (ui.btnAddBkgBlue == pButton)
    {
        _modifyColor(ui.barBkgBlue, 1);
    }
    else if (ui.btnSubFontRed == pButton)
    {
        _modifyColor(ui.barFontRed, -1);
    }
    else if (ui.btnAddFontRed == pButton)
    {
        _modifyColor(ui.barFontRed, 1);
    }
    else if (ui.btnSubFontGreen == pButton)
    {
        _modifyColor(ui.barFontGreen, -1);
    }
    else if (ui.btnAddFontGreen == pButton)
    {
        _modifyColor(ui.barFontGreen, 1);
    }
    else if (ui.btnSubFontBlue == pButton)
    {
        _modifyColor(ui.barFontBlue, -1);
    }
    else if (ui.btnAddFontBlue == pButton)
    {
        _modifyColor(ui.barFontBlue, 1);
    }
}

void CColorDlg::slot_barValueChanged(QWidget *pBar, uint8_t uValue)
{
    (void)uValue;

    if (ui.barBkgRed == pBar || ui.barBkgGreen == pBar || ui.barBkgBlue == pBar)
    {
        int crBkg = QRGB(ui.barBkgRed->value(), ui.barBkgGreen->value(), ui.barBkgBlue->value());
        g_crBkg.setRgb(crBkg);
         g_app.getOption().crBkg = (UINT)crBkg;
    }
    else if (ui.barFontRed == pBar || ui.barFontGreen == pBar || ui.barFontBlue == pBar)
    {
        int crFore = QRGB(ui.barFontRed->value(), ui.barFontGreen->value(), ui.barFontBlue->value());
        g_crFore.setRgb(crFore);
         g_app.getOption().crFore = (UINT)crFore;

        //setWidgetTextColor(ui.groupBkgColor, g_crFore);
        //setWidgetTextColor(ui.groupFontColor, g_crFore);
    }

    this->update();
    m_bkgDlg.update();
     g_app.mainWnd().update();
}
