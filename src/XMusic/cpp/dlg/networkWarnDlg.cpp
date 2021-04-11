
#include "xmusic.h"

#if __android
#include "networkWarnDlg.h"
#include "ui_networkWarnDlg.h"

static Ui::networkWarnDlg ui;

cqcr CNetworkWarnDlg::bkgColor() const
{
    static QColor cr(195, 230, 255);
    return cr;
}

void CNetworkWarnDlg::_onPaint(CPainter& painter, cqrc rc)
{
    extern QColor g_crLogoBkg;
    painter.fillRect(rc, g_crLogoBkg);
    painter.fillRectEx(rc, bkgColor(), __dlgRound);
}

void CNetworkWarnDlg::_setupUi()
{
    if (m_bInit)
    {
        return;
    }

    ui.setupUi(this);

    ui.labelTip->adjustFont(TD_FontWeight::DemiBold);

    QColor crText(__crLogoText);
    ui.labelTip->setForeColor(crText);
    ui.labelContinue->setForeColor(crText);
    ui.labelNeverWarn->setForeColor(crText);
    ui.labelExit->setForeColor(crText);

#define __space "                         "
    ui.labelContinue->setText(__space + ui.labelContinue->text() + __space);
    ui.labelNeverWarn->setText(__space + ui.labelNeverWarn->text() + __space);
    ui.labelExit->setText(__space + ui.labelExit->text() + __space);
}

void CNetworkWarnDlg::_checkWifiConnected()
{
    __async(1000, [&]{
        if (!this->isVisible())
        {
            return;
        }

        if (checkWifiConnected())
        {
            __async(3000, [&]{
                if (checkWifiConnected())
                {
                    ui.labelContinue->performClick();
                }
            });
            return;
        }

        _checkWifiConnected();
    });
}

void CNetworkWarnDlg::show(cfn_void cb)
{
    _setupUi();

    ui.labelContinue->onClicked([=]{
        close();
        cb();
    });
    ui.labelNeverWarn->onClicked([=]{
        close();
        __app.getOption().bNetworkWarn = false;
        cb();
    });

    ui.labelExit->onClicked([&]{
        close();
        __app.quit();
    });

    _checkWifiConnected();

    CDialog::show(); //CDialogEx::show();
}

void CNetworkWarnDlg::_setPos()
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

    ui.labelTip->setWidth(cx);
    ui.labelContinue->setWidth(cx);
    ui.labelNeverWarn->setWidth(cx);
    ui.labelExit->setWidth(cx);
}
#endif
