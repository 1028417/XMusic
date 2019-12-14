
#pragma once

#include <QProgressBar>

#include "widget.h"

class CProgressBar : public CWidget<QProgressBar>
{
public:
    CProgressBar(QWidget *parent) : CWidget(parent)
        , m_brBackground(QColor(255,255,255))
        , m_brBuffer(QColor(195,240,255))
        , m_brForeground(QColor(150,205,255))
    {
    }

private:
    QBrush m_brBackground;
    QBrush m_brBuffer;
    QBrush m_brForeground;

    UINT m_maxBuffer = 0;
    UINT m_bufferValue = 0;

public:
    UINT maxBuffer() const
    {
        return m_maxBuffer;
    }
    UINT bufferValue() const
    {
        return m_bufferValue;
    }

    void setColor(const QColor& crBackground, const QColor& crBuffer, const QColor& crForeground)
    {
        m_brBackground.setColor(crBackground);
        m_brBuffer.setColor(crBuffer);
        m_brForeground.setColor(crForeground);
    }

    void setMaximum(int maxValue, UINT maxBuffer=0)
    {
        m_maxBuffer = maxBuffer;
        QProgressBar::setMaximum(maxValue);
    }

    void setValue(int value, UINT bufferValue=0)
    {
        QProgressBar::setValue(value);

        m_bufferValue = bufferValue;
        update();
    }

private:
    void _onPaint(CPainter& painter, const QRect&) override
    {
        painter.setPen(Qt::transparent);

        QRect rc = this->rect();
        painter.setBrush(m_brBackground);
        painter.drawRoundedRect(rc, 3, 3);

        if (m_maxBuffer > 0 && m_bufferValue > 0)
        {
            rc.setRight(this->rect().right() * m_bufferValue/m_maxBuffer);
            painter.setBrush(m_brBuffer);
            painter.drawRoundedRect(rc, 3, 3);
        }

        if (this->maximum() > 0 && this->value() > 0)
        {
            rc.setRight(this->rect().right() * this->value()/this->maximum());
            painter.setBrush(m_brForeground);
            painter.drawRoundedRect(rc, 3, 3);
        }
    }
};
