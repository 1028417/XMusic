
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
        setBkgColor(QColor(180, 220, 255));

        this->setWindowFlags(Qt::FramelessWindowHint);
        this->setWindowState(Qt::WindowFullScreen);

        T::show();
    }

    void setBkgColor(const QColor& crBkg)
    {
        m_crBkg = crBkg;

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
#ifdef __ANDROID__
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
