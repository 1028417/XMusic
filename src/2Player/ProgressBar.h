
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

    void _onPaint(QPainter& painter, const QRect& pos) override
    {
        painter.fillRect(pos, QBrush(QColor(255,255,255)));

        if (this->maximum() > 0 && this->value() > 0)
        {
            QRect rcProgress(pos);
            rcProgress.setRight(pos.width()*this->value()/this->maximum());
            painter.fillRect(rcProgress, QBrush(QColor(0,255,0)));
        }
    }
};
