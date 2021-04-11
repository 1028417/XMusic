
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

    ui.labelTip->setFont(1.0f, QFont::Weight::DemiBold);

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

void CNetworkWarnDlg::_checkUnMobileConnected()
{
    __async(1000, [&]{
        if (!this->isVisible())
        {
            return;
        }

        if (checkUnMobileConnected())
        {
            __async(3000, [&]{
                this->close();
            });
            return;
        }

        _checkUnMobileConnected();
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

    _checkUnMobileConnected();

    CDialog::show(); //CDialogEx::show();
}
#endif
