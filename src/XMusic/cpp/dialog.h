
#pragma once

#include "util/util.h"

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include <QRgb>

#define __defThemeColor qRgb(180, 220, 255)
extern QColor g_crTheme;

#define __BlueLabel qRgb(32, 128, 255)

class CDialog : public QDialog
{
public:
    static QColor m_crText;

    static void resetPos();

    CDialog(QWidget& parent) : m_parent(&parent)
    {
    }

protected:
    QWidget *m_parent;

private:
    bool m_bFullScreen = false;

private:
    void _setPos();

    virtual void _relayout(int cx, int cy) {(void)cx;(void)cy;}

    virtual bool _handleReturn() {return false;}

    virtual void _onClose(){}

protected:
    virtual bool event(QEvent *ev) override;

    void setBkgColor(const QColor& crBkg)
    {
        QPalette pe = this->palette();
        pe.setColor(QPalette::Background, crBkg);
        this->setPalette(pe);
    }

public:
    void show(bool bFullScreen, const fn_void& cbClose = NULL);

#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif
};
