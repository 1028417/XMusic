
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

void CMsgBox::_setupUi()
{
    if (m_bInit)
    {
        return;
    }

    ui.setupUi(this);

    this->connect_dlgClose(ui.btnX);

    ui.labelTip->setFont(1.15f, TD_FontWeight::DemiBold);
    ui.labelTip->setForeColor(__crLogoText);
}

void CMsgBox::show(cqstr qsMsg, cfn_void cbClose)
{
    _setupUi();

    ui.labelTip->setText(qsMsg);

    CDialogEx::show(cbClose);
}

#if __android
void CMsgBox::_setPos()
{
    static auto s_cxPrev = width();

    int cx = 0;
    if (__app.mainWnd().isHLayout())
    {
        this->setWidth(s_cxPrev);
        cx = s_cxPrev;

        CDialogEx::_setPos();
    }
    else
    {
        cx = __app.mainWnd().width();
        this->setWidth(cx);

        this->move(0, __app.mainWnd().height()-height());
    }

    auto x = cx-ui.btnX->y()-ui.btnX->width();
    ui.btnX->setX(x);

    ui.labelTip->setX(0);
    ui.labelTip->setWidth(cx-x);
}
#endif
