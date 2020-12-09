
#include "xmusic.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox(QWidget& parent) : CDialog(parent, false)
{
}

cqcr CMsgBox::bkgColor() const
{
    static QColor crBkg(195, 230, 255);
    return crBkg;
}

void CMsgBox::show(QWidget& parent, cqstr qsMsg, cfn_void cbClose)
{
    ui.setupUi(this);

    QColor crText(__crLogoText);
    ui.labelTip->setForeColor(crText);
    ui.labelTip->setFont(1.15f, QFont::Weight::DemiBold);

    connect(ui.btnX, &CButton::signal_clicked, this, &QDialog::close);

    ui.labelTip->setText(qsMsg);

    CDialog::show(parent, cbClose);
}
