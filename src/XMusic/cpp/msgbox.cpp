
#include "app.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox(QWidget& parent) : CDialog(parent, false)
{
    ui.setupUi(this);

    ui.labelTip->setFont(1, QFont::Weight::DemiBold);
    QColor crText(64, 128, 255);
    ui.labelTip->setTextColor(crText);

    ui.labelClose->setTextColor(crText);

    connect(ui.labelClose, &CLabel::signal_click, this, &QDialog::close);
}

void CMsgBox::show(const QString& qsMsg, cfn_void cbClose)
{
    ui.labelTip->setText(qsMsg);

    CDialog::show(cbClose);
}
