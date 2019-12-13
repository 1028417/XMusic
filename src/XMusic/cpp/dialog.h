
#pragma once

#include "util/util.h"

#include <QDialog>

#include <QPalette>

#include <QEvent>

#define __BlueLabel QRGB(32, 128, 255)

extern QColor g_crTheme;

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

public:
    void setBkgColor(const QColor& crBkg)
    {
        QPalette pe = this->palette();
        pe.setColor(QPalette::Background, crBkg);
        this->setPalette(pe);
    }

    void show(bool bFullScreen, const fn_void& cbClose = NULL);

#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif
};
