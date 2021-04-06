
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "widget/widget.h"

#define __titleFontSize 1.16f

#define __dlgRound 16

class CDialog : public TWidget<QDialog>
{
public:
    CDialog(bool bFullScreen = true)
        : TWidget(NULL, Qt::FramelessWindowHint)
        , m_bFullScreen(bFullScreen)
    {
    }

    /*CDialog(QWidget& parent, bool bFullScreen = true)
        : TWidget(NULL, Qt::FramelessWindowHint)
        , m_bFullScreen(bFullScreen)
    {
    }

    CDialog(CDialog& parent, bool bFullScreen = true)
        : TWidget(NULL, Qt::FramelessWindowHint)
        , m_bFullScreen(bFullScreen)
    {
    }*/

    static void setWidgetTextColor(QWidget *widget, cqcr cr)
    {
        QPalette pe = widget->palette();
        pe.setColor(QPalette::WindowText, cr);
        widget->setPalette(pe);
    }

protected:
    bool m_bHLayout = false;

private:
    bool m_bFullScreen = true;

private:
    virtual cqcr bkgColor() const
    {
        return g_crBkg;
    }

    void _show(cfn_void cbClose);

    void _setPos();

    virtual void _relayout(int cx, int cy) {(void)cx;(void)cy;}

    virtual bool _handleReturn() {return false;}

    virtual void _onClosed(){}

protected:
    virtual bool event(QEvent *ev) override;

    virtual void _onPaint(CPainter& painter, cqrc rc) override;

public:
    static void resetPos();

    bool isHLayout() const
    {
        return m_bHLayout;
    }

#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif

    void show(cfn_void cbClose = NULL);
};
