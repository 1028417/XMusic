
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

    void setMaximum(int maxValue, int maxBuffer=-1)
    {
        m_maxBuffer = maxBuffer;

        QProgressBar::setMaximum(maxValue);
    }

    void setValue(int value, int bufferValue=-1)
    {
        m_bufferValue = bufferValue;

        QProgressBar::setValue(value);
    }

private:
    int m_maxBuffer = -1;
    int m_bufferValue = -1;

private:
    void _onPaint(CPainter& painter, const QRect&) override
    {
        QRect rect = this->rect();
        painter.fillRect(rect, QBrush(QColor(255,255,255)));

        if (m_maxBuffer > 0 && m_bufferValue >= 0)
        {
            rect.setRight(this->rect().right() * m_bufferValue/m_maxBuffer);
            painter.fillRect(rect, QBrush(QColor(150,200,255)));
        }

        if (this->maximum() > 0 && this->value() > 0)
        {
            rect.setRight(this->rect().right() * this->value()/this->maximum());
            painter.fillRect(rect, QBrush(QColor(160,210,255)));
        }
    }
};
