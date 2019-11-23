
#include "app.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox(QWidget& parent)
    : CDialog(parent, QColor(200, 230, 255))
{
    ui.setupUi(this);

    ui.labelTip->setFont(1, E_FontWeight::FW_SemiBold);
    ui.labelTip->setTextColor(QColor(__BlueLabel));

    ui.labelClose->setTextColor(QColor(__BlueLabel));

    connect(ui.labelClose, &CLabel::signal_click, this, &QDialog::close);
}

void CMsgBox::_show(QWidget& parent, const QString& qsMsg, const fn_void& cbClose)
{
    m_parent = &parent;

    ui.labelTip->setText(qsMsg);

    CDialog::show(false, cbClose);
}
