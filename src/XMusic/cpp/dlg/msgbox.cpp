
#include "xmusic.h"

#include "msgbox.h"
#include "ui_msgbox.h"

static Ui::MsgBox ui;

CMsgBox::CMsgBox()
{
    ui.setupUi(this);

    this->connect_dlgClose(ui.btnX);

    ui.labelTip->setForeColor(__crLogoText);
}

void CMsgBox::show(cqstr qsMsg, cfn_void cbClose)
{
    ui.labelTip->setFont(1.15f, TD_FontWeight::DemiBold);

    ui.labelTip->setText(qsMsg);

    CDialog::show(cbClose);
}

void CMsgBox::_relayout(int cx, int cy)
{
    (void)cy;

    auto x = cx-ui.btnX->y()-ui.btnX->width();
    ui.btnX->setX(x);

    ui.labelTip->setX(0);
    ui.labelTip->setWidth(x);
}

void CMsgBox::_setPos()
{
#if __android
    static auto s_cxPrev = width();

    if (!__app.mainWnd().isHLayout())
    {
        this->move(0, __app.mainWnd().height()-height());
        this->setWidth(__app.mainWnd().width());
        return;
    }

    this->setWidth(s_cxPrev);
#endif

    cauto ptCenter = __app.mainWnd().geometry().center();
    move(ptCenter.x()-width()/2, ptCenter.y()-height()/2);
}

void CMsgBox::_onPaint(CPainter& painter, cqrc rc)
{
    extern QColor g_crLogoBkg;
    painter.fillRect(rc, g_crLogoBkg);
    painter.fillRectEx(rc,  QColor(195, 230, 255), __dlgRound);
}
