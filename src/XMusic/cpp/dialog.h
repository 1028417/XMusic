
#pragma once

#include "util/util.h"

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "widget.h"

#define __BangsOffset __size(30)

class CDialog : public QDialog
{
public:
    CDialog(bool bFullScreen = true, QWidget *parent = NULL)
        : QDialog(parent)
        , m_bFullScreen(bFullScreen)
    {
        this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    }

private:
    bool m_bFullScreen = false;

private:
    virtual const QColor& bkgColor() const
    {
        return g_crTheme;
    }

    void _setPos();

    virtual void _relayout(int cx, int cy) {(void)cx;(void)cy;}

    virtual bool _handleReturn() {return false;}

    virtual void _onClose(){}

protected:
    virtual bool event(QEvent *ev) override;

    bool _checkBangs(int cx, int cy)
    {
        return 375 == cx && 812 == cy;
    }

public:
    static void setWidgetTextColor(QWidget *widget, const QColor& cr)
    {
        QPalette pe = widget->palette();
        pe.setColor(QPalette::WindowText, cr);
        widget->setPalette(pe);
    }

    static void resetPos();

#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif

    void show(QWidget& parent, bool bFullScreen, const fn_void& cbClose = NULL);
    void show(const fn_void& cbClose = NULL);
};
