
#pragma once

#include "util/util.h"

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "widget.h"

class CDialog : public QDialog
{
public:
    static void resetPos();

    CDialog(QWidget& parent) : m_parent(&parent)
    {
    }

protected:
    QWidget *m_parent;

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
    void show(bool bFullScreen, const fn_void& cbClose = NULL);

#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif

    static void setWidgetColor(QWidget *widget, const QColor& cr)
    {
        QPalette pe = widget->palette();
        pe.setColor(QPalette::WindowText, cr);
        widget->setPalette(pe);
    }
};
