
#pragma once

#include <QMouseEvent>

#include <QLabel>

#include "widget.h"

class CLabel : public CWidget<QLabel>
{
    Q_OBJECT

public:
    CLabel(QWidget *parent) :
        CWidget<QLabel>(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

signals:
    void signal_mousePressEvent(CLabel*, const QPoint& pos);

    void signal_clicked(CLabel*);

private:
    void mousePressEvent(QMouseEvent *ev) override
    {
        CWidget<QLabel>::mousePressEvent(ev);

        emit signal_mousePressEvent(this, ev->pos());
    }

    void mouseReleaseEvent(QMouseEvent *ev) override
    {
        CWidget<QLabel>::mouseReleaseEvent(ev);

        emit signal_clicked(this);
    }
};
