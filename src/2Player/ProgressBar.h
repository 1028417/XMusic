
#pragma once

#include <QProgressBar>

#include "widget.h"

class CProgressBar : public CWidget<QProgressBar>
{
public:
    CProgressBar(QWidget *parent) :
        CWidget<QProgressBar>(parent)
    {
    }

    void _onPaint(CPainter& painter, const QRect&) override
    {
        QRect rect = this->rect();
        painter.fillRect(rect, QBrush(QColor(255,255,255)));

        if (this->maximum() > 0 && this->value() > 0)
        {
            rect.setRight(rect.right()*this->value()/this->maximum());
            painter.fillRect(rect, QBrush(QColor(180,220,255)));
        }
    }
};
