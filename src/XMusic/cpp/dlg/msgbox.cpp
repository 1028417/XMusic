
#include "xmusic.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox() : CDialog(false)
{
    ui.setupUi(this);

    ui.labelTip->setFont(1.15f, QFont::Weight::DemiBold);
    ui.labelTip->setForeColor(__crLogoText);

    connect(ui.btnX, &CButton::signal_clicked, this, &QDialog::close);
}

cqcr CMsgBox::bkgColor() const
{
    static QColor cr(195, 230, 255);
    return cr;
}

void CMsgBox::show(cqstr qsMsg, cfn_void cbClose)
{
    ui.labelTip->setText(qsMsg);

    CDialog::show(cbClose);
}

void CMsgBox::_onPaint(CPainter& painter, cqrc rc)
{
#if __android
    extern QColor g_crLogoBkg;
    painter.fillRect(rc, g_crLogoBkg);
    painter.fillRectEx(rc, bkgColor(), __dlgRound);
    return;
#endif
    CDialog::_onPaint(painter, rc);
}
