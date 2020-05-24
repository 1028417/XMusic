
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

    connect(ui.labelExit, &CLabel::signal_click, [&](){
        this->setVisible(false);
        m_app.quit();
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

    CDialog::show();
}
