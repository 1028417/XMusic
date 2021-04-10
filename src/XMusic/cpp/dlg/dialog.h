
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "widget/widget.h"

#define __titleFontSize 1.16f

#define __dlgRound 16

class CDialog : public TWidget<QDialog>
{
    friend class CMaskDlg;
public:
    static void setWidgetTextColor(QWidget *widget, cqcr cr)
    {
        QPalette pe = widget->palette();
        pe.setColor(QPalette::WindowText, cr);
        widget->setPalette(pe);
    }

    CDialog(bool bFullScreen = true)
        : TWidget(NULL, Qt::FramelessWindowHint)
        , m_bFullScreen(bFullScreen)
    {
    }

    virtual ~CDialog()
    {
        if (m_pDlgMask)
        {
            delete m_pDlgMask;
        }
    }

protected:
    bool m_bHLayout = false;

private:
    bool m_bFullScreen = true;

    CDialog *m_pDlgMask = NULL;

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

    virtual void _onPaint(CPainter&, cqrc) override;

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

    void connect_dlgClose(class CButton *btn);

    void show(cfn_void cbClose = NULL)
    {
//#if __android
        if (!m_bFullScreen)
        {
            showMask(cbClose);
            return;
        }
//#endif
        _show(cbClose);
    }

    void showMask(cqcr crMask, cfn_void cbClose = NULL);

    void showMask(cfn_void cbClose = NULL)
    {
        showMask(QColor(0,0,0,0), cbClose);
    }
};

class CMaskDlg : public CDialog
{
    friend class CDialog;
private:
    CMaskDlg(CDialog& child)
        : CDialog(true)
        , m_child(child)
    {
    }

private:
    CDialog& m_child;

    QColor m_crMask;

private:
    void show(cqcr crMask, cfn_void cbClose);

    void _relayout(int cx, int cy) override
    {
        m_child.move((cx-m_child.width())/2, (cy-m_child.height())/2);
    }

    void _onPaint(CPainter&, cqrc) override;

    bool _handleReturn() override
    {
        return m_child._handleReturn();
    }
};
