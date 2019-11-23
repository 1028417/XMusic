
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "util/util.h"

class CDialog : public QDialog
{
public:
    CDialog(QWidget& parent) : m_parent(&parent)
        , m_crBkg(180, 220, 255)
    {
    }

    CDialog(QWidget& parent, const QColor& crBkg) : m_parent(&parent)
        , m_crBkg(crBkg)
    {
    }

protected:
    QWidget *m_parent;

private:
    QColor m_crBkg;

private:
    void _setBkgColor()
    {
        QPalette pe = this->palette();
        pe.setColor(QPalette::Background, m_crBkg);
        this->setPalette(pe);
    }

    virtual void _relayout(int cx, int cy) {(void)cx;(void)cy;}

    virtual bool _handleReturn() {return false;}

    virtual void _onClose(){}

public:
    void show(bool bFullScreen, const fn_void& cbClose = NULL);

    void setBkgColor(UINT r, UINT g, UINT b)
    {
        m_crBkg.setRgb(r,g,b);

        _setBkgColor();
    }

    const QColor& bkgColor() const
    {
        return m_crBkg;
    }

#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif

protected:
    virtual bool event(QEvent *ev) override;
};
