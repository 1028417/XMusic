
#pragma once

#include <QLabel>

#include <QMouseEvent>

class CLabel : public QLabel
{
    Q_OBJECT

public:
    CLabel(QWidget *parent) :
        QLabel(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

signals:
    void signal_mousePressEvent(CLabel*, const QPoint& pos);

    void signal_clicked(CLabel*);

private:
    void mousePressEvent(QMouseEvent *ev) override
    {
        QLabel::mousePressEvent(ev);

        emit signal_mousePressEvent(this, ev->pos());
    }

    void mouseReleaseEvent(QMouseEvent *ev) override
    {
        QLabel::mouseReleaseEvent(ev);

        emit signal_clicked(this);
    }
};
