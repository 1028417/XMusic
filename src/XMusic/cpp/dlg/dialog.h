
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "widget/widget.h"

#define __titleFontSize 1.16f

#define __dlgRound 16

class CDialog : public TWidget<QDialog>
{
    friend class CDialogEx;
    friend class CMaskDlg;
public:
    static void resetPos();

    static void setWidgetTextColor(QWidget *widget, cqcr cr)
    {
        QPalette pe = widget->palette();
        pe.setColor(QPalette::WindowText, cr);
        widget->setPalette(pe);
    }

    CDialog()
        : TWidget(NULL, Qt::FramelessWindowHint)
    {
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_NoSystemBackground);
    }

protected:
    bool m_bHLayout = false;

private:
    virtual void _relayout(int cx, int cy) {(void)cx;(void)cy;}

    virtual bool _handleReturn() {return false;}

    virtual void _onClosed(){}

protected:
    virtual void _setPos();

    virtual bool event(QEvent *ev) override;

    virtual void _onPaint(CPainter&, cqrc) override;

    virtual cqcr bkgColor() const
    {
        return g_crBkg;
    }

public:
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

    void show(cfn_void cbClose = NULL);
};

class CMaskDlg : public CDialog
{
    friend class CDialogEx;
private:
    CMaskDlg(CDialog& child)
        : m_child(child)
    {
    }

private:
    CDialog& m_child;

    QColor m_crMask;

private:
    void showMask(cqcr crMask, cfn_void cbClose);

    /*void _setPos() override
    {
        CDialog::_setPos();
        m_child._setPos();
    }*/

    void _relayout(int, int) override
    {
        m_child._setPos();
    }

    void _onPaint(CPainter&, cqrc) override;

    bool _handleReturn() override
    {
        return m_child._handleReturn();
    }

    void _onClosed() override
    {
        m_child._onClosed();
    }
};

class CDialogEx : public CDialog
{
public:
    CDialogEx() = default;

    virtual ~CDialogEx()
    {
        if (m_pDlgMask)
        {
            delete m_pDlgMask;
        }
    }

private:
    CMaskDlg *m_pDlgMask = NULL;

protected:
    virtual void _setPos() override;

    virtual void _onPaint(CPainter&, cqrc) override;

public:
    void show(cqcr crMask, cfn_void cbClose = NULL);

    void show(cfn_void cbClose = NULL);
};
