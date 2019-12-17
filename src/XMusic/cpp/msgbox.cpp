
#include "app.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox()
{
    ui.setupUi(this);

    ui.labelTip->setFont(1, E_FontWeight::FW_SemiBold);
    QColor crText(64, 128, 255);
    ui.labelTip->setTextColor(crText);

    ui.labelClose->setTextColor(crText);

    connect(ui.labelClose, &CLabel::signal_click, this, &QDialog::close);
}

void CMsgBox::show(QWidget& parent, const QString& qsMsg, const fn_void& cbClose)
{
    static CMsgBox inst;
    ui.labelTip->setText(qsMsg);

    ((CDialog&)inst).show(parent, false, cbClose);
}
