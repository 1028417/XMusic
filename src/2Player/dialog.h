
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
        QPalette pe;
        pe.setColor(QPalette::Background, QColor(180, 220, 255));
        this->setPalette(pe);
    }

    void show()
    {
        this->setWindowFlags(Qt::FramelessWindowHint);
        this->setWindowState(Qt::WindowFullScreen);

        T::show();
    }

protected:
    virtual bool event(QEvent *ev) override
    {
        switch (ev->type())
        {
        case QEvent::Move:
        case QEvent::Resize:
        case QEvent::Show:
            _relayout();

            break;
        default:
            break;
        }

        return T::event(ev);
    }

private:
    virtual void _relayout() {}
};
