#include "app.h"

#include "colordlg.h"

#include "colorbar.h"

#include "groupframe.h"

#include "ui_colordlg.h"

static Ui::ColorDlg ui;

CColorDlg::CColorDlg(class CApp& app, CBkgDlg& bkgDlg)
    : m_app(app),
    m_bkgDlg(bkgDlg)
{
}

void CColorDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(1.15, E_FontWeight::FW_SemiBold);

    CFont font(1.05);
    ui.groupBkgColor->setFont(font);
    ui.groupBkgColor->setAttribute(Qt::WA_TranslucentBackground);
    ui.groupFontColor->setFont(font);
    ui.groupFontColor->setAttribute(Qt::WA_TranslucentBackground);

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    for (auto pButton : SList<CButton*>({ui.btnSubBkgRed, ui.btnAddBkgRed, ui.btnSubBkgGreen, ui.btnAddBkgGreen
                                        , ui.btnSubBkgBlue, ui.btnAddBkgBlue, ui.btnApplyBkgColor
                                        , ui.btnSubFontRed, ui.btnAddFontRed, ui.btnSubFontGreen, ui.btnAddFontGreen
                                        , ui.btnSubFontBlue, ui.btnAddFontBlue}))
    {
        connect(pButton, &CButton::signal_clicked, this, &CColorDlg::slot_buttonClicked);
    }

    for (auto pBar : SList<CColorBar*>({ui.barBkgRed, ui.barBkgGreen, ui.barBkgBlue
                                       , ui.barFontRed, ui.barFontGreen, ui.barFontBlue}))
    {
        connect(pBar, &CColorBar::signal_valueChanged, this, &CColorDlg::slot_barValueChanged);
    }
}

void CColorDlg::show()
{
    setWidgetTextColor(ui.groupBkgColor, g_crText);
    setWidgetTextColor(ui.groupFontColor, g_crText);

    ui.barBkgRed->setColor(E_BarColor::BC_Red, g_crTheme.red());
    ui.barBkgGreen->setColor(E_BarColor::BC_Green, g_crTheme.green());
    ui.barBkgBlue->setColor(E_BarColor::BC_Blue, g_crTheme.blue());

    ui.barFontRed->setColor(E_BarColor::BC_Red, g_crText.red());
    ui.barFontGreen->setColor(E_BarColor::BC_Green, g_crText.green());
    ui.barFontBlue->setColor(E_BarColor::BC_Blue, g_crText.blue());

    CDialog::show(m_bkgDlg);
}

void CColorDlg::_relayout(int cx, int cy)
{
    int sz = cy>cx ? cy/11.1 : cy/6.1;
    int xMargin = sz/4;
    QRect rcReturn(xMargin, xMargin, sz-xMargin*2, sz-xMargin*2);
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    ui.labelTitle->move(rcReturn.right() + xMargin, rcReturn.center().y() - ui.labelTitle->height()/2);

    cauto yClient = rcReturn.bottom() + xMargin;

    int xFrame = xMargin * 2;
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

    ui.btnApplyBkgColor->move((cx_group-ui.btnApplyBkgColor->width())/2, cy_group-__size(30)-ui.btnApplyBkgColor->height());

    int x = __size(40);
    int y = cy_group/3.94;

    int szButton = rcReturn.width()-3;
    ui.btnSubBkgRed->setGeometry(x, y-ui.btnSubBkgRed->height()/2, szButton, szButton);
    ui.btnAddBkgRed->setGeometry(cx_group-x-ui.btnAddBkgRed->width(), ui.btnSubBkgRed->y(), szButton, szButton);

    int x_bar = 2*x + szButton;
    int cx_bar = cx_group-x_bar*2+1;
    int cy_bar = __size(40);

    for (auto pBar : SList<CColorBar*>({ui.barBkgRed, ui.barBkgGreen, ui.barBkgBlue
                                       , ui.barFontRed, ui.barFontGreen, ui.barFontBlue}))
    {
        pBar->setMargin((cy_bar-__size(11))/2);
    }

    ui.barBkgRed->setGeometry(x_bar, y-cy_bar/2, cx_bar, cy_bar);

    ui.btnSubBkgGreen->setGeometry(x, ui.btnSubBkgRed->y()+y, szButton, szButton);
    ui.btnAddBkgGreen->setGeometry(ui.btnAddBkgRed->x(), ui.btnSubBkgGreen->y(), szButton, szButton);
    ui.barBkgGreen->setGeometry(x_bar, ui.barBkgRed->y()+y, cx_bar, cy_bar);

    y+=y;
    ui.btnSubBkgBlue->setGeometry(x, ui.btnSubBkgRed->y()+y, szButton, szButton);
    ui.btnAddBkgBlue->setGeometry(ui.btnAddBkgRed->x(), ui.btnSubBkgBlue->y(), szButton, szButton);
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
    else if (ui.btnApplyBkgColor == pButton)
    {
        applyBkgColor();
        this->close();
    }
}

void CColorDlg::applyBkgColor()
{
    if (!m_app.getOption().bUseThemeColor)
    {
        m_app.getOption().bUseThemeColor = true;

        m_app.mainWnd().updateBkg();
    }
}

void CColorDlg::slot_barValueChanged(QWidget *pBar, uint8_t uValue)
{
    (void)uValue;

    if (ui.barBkgRed == pBar || ui.barBkgGreen == pBar || ui.barBkgBlue == pBar)
    {
        int crTheme = QRGB(ui.barBkgRed->value(), ui.barBkgGreen->value(), ui.barBkgBlue->value());
        g_crTheme.setRgb(crTheme);
        m_app.getOption().crTheme = (UINT)crTheme;

        applyBkgColor();
    }
    else if (ui.barFontRed == pBar || ui.barFontGreen == pBar || ui.barFontBlue == pBar)
    {
        int crText = QRGB(ui.barFontRed->value(), ui.barFontGreen->value(), ui.barFontBlue->value());
        g_crText.setRgb(crText);
        m_app.getOption().crText = (UINT)crText;

        CDialog::setWidgetTextColor(ui.groupBkgColor, g_crText);
        CDialog::setWidgetTextColor(ui.groupFontColor, g_crText);
    }

    this->update();
}

void CColorDlg::_onClose()
{
    m_bkgDlg.repaint();
    m_app.mainWnd().repaint();
}
