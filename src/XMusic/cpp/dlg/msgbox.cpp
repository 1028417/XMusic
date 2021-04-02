
#include "xmusic.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox(QWidget& parent) : CDialog(parent, false)
{
    ui.setupUi(this);

    ui.labelTip->setFont(1.15f, QFont::Weight::DemiBold);
    ui.labelTip->setForeColor(__crLogoText);

    connect(ui.btnX, &CButton::signal_clicked, this, &QDialog::close);
}

cqcr CMsgBox::bkgColor() const
{
    return QColor(195, 230, 255);
}

void CMsgBox::show(QWidget& parent, cqstr qsMsg, cfn_void cbClose)
{
    ui.labelTip->setText(qsMsg);

    CDialog::show(parent, cbClose);
}
