
#include "xmusic.h"

#if __android
#include "networkWarnDlg.h"
#include "ui_networkWarnDlg.h"

static Ui::networkWarnDlg ui;

cqcr CNetworkWarnDlg::bkgColor() const
{
    static QColor cr(195, 230, 255);
    return cr;
}

void CNetworkWarnDlg::_onPaint(CPainter& painter, cqrc rc)
{
    extern QColor g_crLogoBkg;
    painter.fillRect(rc, g_crLogoBkg);
    painter.fillRectEx(rc, bkgColor(), __dlgRound);
}

void CNetworkWarnDlg::show(cfn_void cb)
{
    static bool s_bFlag = false;
    if (!s_bFlag)
    {
        s_bFlag = true;

        ui.setupUi(this);

        ui.labelTip->setFont(1.0f, QFont::Weight::DemiBold);

        QColor crText(__crLogoText);
        ui.labelTip->setForeColor(crText);
        ui.labelContinue->setForeColor(crText);
        ui.labelNeverWarn->setForeColor(crText);
        ui.labelExit->setForeColor(crText);

#define __space "                         "
        ui.labelContinue->setText(__space + ui.labelContinue->text() + __space);
        ui.labelNeverWarn->setText(__space + ui.labelNeverWarn->text() + __space);
        ui.labelExit->setText(__space + ui.labelExit->text() + __space);
    }

    ui.labelContinue->onClicked([=]{
        close();
        cb();
    });
    ui.labelNeverWarn->onClicked([=]{
        close();
        __app.getOption().bNetworkWarn = false;
        cb();
    });

    ui.labelExit->onClicked([&]{
        close();
        __app.quit();
    });

    CDialog::show(cbClose); //CDialogEx::show();
}
#endif
