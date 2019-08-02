
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

private:
    QColor m_crShadow;

signals:
    void signal_mousePressEvent(CLabel*);
    void signal_mousePressEvent(CLabel*, const QPoint& pos);

public:
    void setShadowColor(const QColor& crShadow)
    {
        m_crShadow = crShadow;
    }

private:
    void mousePressEvent(QMouseEvent *ev) override
    {
        QLabel::mousePressEvent(ev);

        emit signal_mousePressEvent(this);
        emit signal_mousePressEvent(this, ev->pos());
    }

    void _onPaint(QPainter& painter, const QRect& pos) override
    {
        CWidget<QLabel>::_onPaint(painter, pos);
    }
};
