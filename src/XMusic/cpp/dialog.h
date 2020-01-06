
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "widget.h"

#define __dlgFlags Qt::Dialog | Qt::FramelessWindowHint

class CDialog : public QDialog
{
public:
    CDialog(QWidget *parent = NULL, bool bFullScreen = true)
        : QDialog(parent, __dlgFlags)
        , m_bFullScreen(bFullScreen)
    {
    }

    CDialog(QWidget& parent, bool bFullScreen)
        : QDialog(&parent, __dlgFlags)
        , m_bFullScreen(bFullScreen)
    {
    }

    CDialog(CDialog& dlg, bool bFullScreen = true)
        : QDialog(&dlg, __dlgFlags)
        , m_bFullScreen(bFullScreen)
    {
    }

private:
    bool m_bFullScreen = true;

private:
    virtual cqcr bkgColor() const
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
    static void setWidgetTextColor(QWidget *widget, cqcr cr)
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

    void show(cfn_void cbClose = NULL);
    void show(QWidget& parent, cfn_void cbClose = NULL);
};
