
#pragma once

#include <QProgressBar>

#include "widget.h"

class CProgressBar : public CWidget<QProgressBar>
{
public:
    CProgressBar(QWidget *parent) : CWidget<QProgressBar>(parent)
    {
        setColor(QColor(255,255,255), QColor(195,240,255), QColor(150,205,255));
    }

    void setColor(const QColor& crBackground, const QColor& crBuffer, const QColor& crForeground)
    {
        m_brBackground.setColor(crBackground);
        m_brBuffer.setColor(crBuffer);
        m_brForeground.setColor(crForeground);
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
    QBrush m_brBackground;
    QBrush m_brBuffer;
    QBrush m_brForeground;

    int m_maxBuffer = -1;
    int m_bufferValue = -1;

private:
    void _onPaint(CPainter& painter, const QRect&) override
    {
        QRect rect = this->rect();
        painter.fillRect(rect, m_brBackground);

        if (m_maxBuffer > 0 && m_bufferValue > 0)
        {
            rect.setRight(this->rect().right() * m_bufferValue/m_maxBuffer);
            painter.fillRect(rect, m_brBuffer);
        }

        if (this->maximum() > 0 && this->value() > 0)
        {
            rect.setRight(this->rect().right() * this->value()/this->maximum());
            painter.fillRect(rect, m_brForeground);
        }
    }
};
