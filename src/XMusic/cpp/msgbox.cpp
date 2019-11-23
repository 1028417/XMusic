
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

#include <QBitmap>

void CMsgBox::_show(QWidget& parent, const QString& qsMsg, const fn_void& cbClose)
{
    m_parent = &parent;

    ui.labelTip->setText(qsMsg);

    QBitmap bmp(this->size());
    bmp.fill();
    QPainter painter(&bmp);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.drawRoundedRect(bmp.rect(),10,10);
    setMask(bmp);

    CDialog::show(false, cbClose);
}
