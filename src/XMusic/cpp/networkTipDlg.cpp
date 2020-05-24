
#include "app.h"

#include "NetworkTipDlg.h"
#include "ui_networkTipDlg.h"

static Ui::networkTipDlg ui;

CNetworkTipDlg::CNetworkTipDlg(QWidget& parent, class CApp& app)
    : CDialog(parent, false)
    , m_app(app)
{
    ui.setupUi(this);

    ui.labelTip->setFont(1, QFont::Weight::DemiBold);

    QColor crText(64, 128, 255);
    ui.labelTip->setForeColor(crText);
    ui.labelContinue->setForeColor(crText);
    ui.labelNeverWarn->setForeColor(crText);
    ui.labelExit->setForeColor(crText);

#define __space "                    "
    ui.labelContinue->setText(__space + ui.labelContinue->text() + __space);
    ui.labelNeverWarn->setText(__space + ui.labelNeverWarn->text() + __space);
    ui.labelExit->setText(__space + ui.labelExit->text() + __space);

    connect(ui.labelExit, &CLabel::signal_click, [&](){
        close();
    });
}

void CNetworkTipDlg::show(cfn_void cb)
{
    connect(ui.labelContinue, &CLabel::signal_click, [=](){
        this->setVisible(false);
        cb();
    });
    connect(ui.labelNeverWarn, &CLabel::signal_click, [=](){
        this->setVisible(false);
        cb();
    });

    CDialog::show([&]{
        m_app.quit();
    });
}
