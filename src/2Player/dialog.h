
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

template <class T=QDialog>
class CDialog : public T
{
public:
    explicit CDialog(QWidget *parent = 0)
        : T(parent)
    {
    }

private:
    QColor m_crBkg;

public:
    void show()
    {
        setBkgColor(180, 220, 255);

        this->setWindowFlags(Qt::FramelessWindowHint);
        T::showFullScreen();
        this->setWindowState(Qt::WindowFullScreen);
    }

    void setBkgColor(uint r, uint g, uint b)
    {
        m_crBkg.setRgb(r,g,b);

        QPalette pe = this->palette();
        pe.setColor(QPalette::Background, m_crBkg);
        this->setPalette(pe);
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
        case QEvent::Move:
        case QEvent::Resize:
        case QEvent::Show:
        {
            int cx = this->width();
            int cy = this->height();
            _relayout(cx, cy);
        }

            break;
#if __android
        case QEvent::KeyRelease:
            if (_handleReturn())
            {
                return true;
            }

            break;
#endif
        default:
            break;
        }

        return T::event(ev);
    }

private:
    virtual void _relayout(int cx, int cy) {(void)cx;(void)cy;}

    virtual bool _handleReturn() {return false;}
};
