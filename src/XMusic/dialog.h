
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

#include "util/util.h"

extern void showFull(QWidget* wnd);

#include <set>
extern std::set<class CDialog*> g_setDlgs;

class CDialog : public QDialog
{
public:
    CDialog(QWidget *parent=NULL) :
        QDialog(parent)
        , m_crBkg(180, 220, 255)
    {
        this->setWindowFlags(Qt::Dialog);
        this->setWindowModality(Qt::ApplicationModal);
    }

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
    void show()
    {
        _setBkgColor();

        this->setWindowFlags(Qt::FramelessWindowHint);

        showFull(this);

        g_setDlgs.insert(this);
    }

    void setBkgColor(uint r, uint g, uint b)
    {
        m_crBkg.setRgb(r,g,b);

        _setBkgColor();
    }

    const QColor& bkgColor() const
    {
        return m_crBkg;
    }

protected:
    virtual bool event(QEvent *ev) override
    {
        switch (ev->type())
        {
        case QEvent::Close:
            g_setDlgs.erase(this);
            _onClose();

#if __mac
            // TODO /*for (auto pDlg : g_setDlgs) pDlg->
#endif

            break;
        case QEvent::Move:
        case QEvent::Resize:
        {
            int cx = this->width();
            int cy = this->height();
            _relayout(cx, cy);
        }

            break;
#if __android || __ios
        case QEvent::KeyRelease:
            if (!_handleReturn())
            {
                close();
            }

            return true;

            break;
#endif
        default:
            break;
        }

        return QDialog::event(ev);
    }
};
