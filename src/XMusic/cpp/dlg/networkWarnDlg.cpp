
#include "xmusic.h"

#if __android
#include "networkWarnDlg.h"
#include "ui_networkWarnDlg.h"

static Ui::networkWarnDlg ui;

void CNetworkWarnDlg::_setupUi()
{
    static bool bInited = false;
    if (bInited)
    {
        return;
    }
    bInited = true;

    ui.setupUi(this);

#define __fontSize 1.03f
    ui.labelTip->adjustFont(__fontSize, TD_FontWeight::DemiBold);
    ui.labelContinue->adjustFont(__fontSize);
    ui.labelNeverWarn->adjustFont(__fontSize);
    ui.labelExit->adjustFont(__fontSize);

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
    async(1000, [&]{
        if (!this->isVisible())
        {
            return;
        }

        if (checkWifiConnected())
        {
            async(3000, [&]{
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
        g_app.getOption().bNetworkWarn = false;
        cb();
    });

    ui.labelExit->onClicked([&]{
        close();

        async([]{
            g_app.quit();
        });
    });

    _checkWifiConnected();

    CDialog::show(); //CDialogEx::show();
}

void CNetworkWarnDlg::_setPos()
{
    static auto s_cxPrev = width();

    int cx = 0;
    if (g_bHLayout)
    {
        this->setWidth(s_cxPrev);
        cx = s_cxPrev;

        CDialogEx::_setPos();
    }
    else
    {
        cx = g_app.mainWnd().width();
        this->setWidth(cx);

        this->move(0, g_app.mainWnd().height()-height());
    }

    ui.labelTip->setWidth(cx);
    ui.labelContinue->setWidth(cx);
    ui.labelNeverWarn->setWidth(cx);
    ui.labelExit->setWidth(cx);
}

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
#endif
