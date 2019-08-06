
#pragma once

#include <QDialog>

#include <QPalette>

#include <QEvent>

class CDialog : public QDialog
{
    Q_OBJECT
public:
    explicit CDialog(QWidget *parent = 0)
        : QDialog(parent)
    {
        QPalette pe;
        pe.setColor(QPalette::Background, QColor(180, 220, 255));
        this->setPalette(pe);

        this->setWindowFlags(Qt::FramelessWindowHint);
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

        return QDialog::event(ev);
    }

private:
    virtual void _relayout() {}
};
