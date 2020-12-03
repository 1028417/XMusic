
#include "xmusic.h"

#include "networkWarnDlg.h"
#include "ui_networkWarnDlg.h"

static Ui::networkWarnDlg ui;

CNetworkWarnDlg::CNetworkWarnDlg(QWidget& parent)
    : CDialog(parent, false)
{
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

cqcr CNetworkWarnDlg::bkgColor() const
{
    static QColor crBkg(195, 230, 255);
    return crBkg;
}

void CNetworkWarnDlg::show(cfn_void cb)
{
    connect(ui.labelContinue, &CLabel::signal_click, [=]{
        close();
        cb();
    });
    connect(ui.labelNeverWarn, &CLabel::signal_click, [=]{
        close();
        __app.getOption().bNetworkWarn = false;
        cb();
    });

    connect(ui.labelExit, &CLabel::signal_click, [&]{
        close();
        __app.quit();
    });

    CDialog::show();
}
