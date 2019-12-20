
#pragma once

#include "util/util.h"

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "widget.h"

class CDialog : public QDialog
{
public:
    CDialog() {}

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

public:
    static void setWidgetTextColor(QWidget *widget, const QColor& cr)
    {
        QPalette pe = widget->palette();
        pe.setColor(QPalette::WindowText, cr);
        widget->setPalette(pe);
    }

#if __windows
    static void resetPos();

    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif

    void show(QWidget& parent, bool bFullScreen, const fn_void& cbClose = NULL);
};
