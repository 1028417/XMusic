
#include "xmusic.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

cqcr CMsgBox::bkgColor() const
{
    static QColor cr(195, 230, 255);
    return cr;
}

void CMsgBox::_onPaint(CPainter& painter, cqrc rc)
{
#if __android
    extern QColor g_crLogoBkg;
    painter.fillRect(rc, g_crLogoBkg);
    painter.fillRectEx(rc, bkgColor(), __dlgRound);
#else
    CDialog::_onPaint(painter, rc);
#endif
}

void CMsgBox::show(cqstr qsMsg, cfn_void cbClose)
{
    static bool s_bFlag = false;
    if (!s_bFlag)
    {
        s_bFlag = true;

        ui.setupUi(this);

        ui.labelTip->setFont(1.15f, QFont::Weight::DemiBold);
        ui.labelTip->setForeColor(__crLogoText);
    }

    this->connect_dlgClose(ui.btnX);

    ui.labelTip->setText(qsMsg);

#if __android
    CDialog::show(cbClose);
#else
    CDialogEx::show(cbClose);
#endif
}
