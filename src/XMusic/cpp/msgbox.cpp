
#include "app.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox(QWidget& parent) : CDialog(parent, false)
{
    ui.setupUi(this);

    QColor crText(64, 128, 255);
    ui.labelTip->setForeColor(crText);
    ui.labelTip->setFont(1, QFont::Weight::DemiBold);

    connect(ui.btnX, &CButton::signal_clicked, this, &QDialog::close);
}

cqcr CMsgBox::bkgColor() const
{
    static QColor crBkg(200, 230, 255);
    return crBkg;
}

void CMsgBox::show(cqstr qsMsg, cfn_void cbClose)
{
    ui.labelTip->setText(qsMsg);

    CDialog::show(cbClose);
}
