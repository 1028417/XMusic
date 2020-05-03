
#include "app.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox(QWidget& parent) : CDialog(parent, false)
{
    ui.setupUi(this);

    ui.labelTip->setFont(1, QFont::Weight::DemiBold);
    QColor crText(64, 128, 255);
    ui.labelTip->setForeColor(crText);

    ui.btnClose->setFont(0.95);
    ui.btnClose->setForeColor(crText);

    connect(ui.btnClose, &CButton::signal_clicked, this, &QDialog::close);
}

void CMsgBox::show(cqstr qsMsg, cfn_void cbClose)
{
    ui.labelTip->setText(qsMsg);

    CDialog::show(cbClose);
}
