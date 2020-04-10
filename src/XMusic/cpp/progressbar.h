
#pragma once

#include <QProgressBar>

#include "widget.h"

class CProgressBar : public TWidget<QProgressBar>
{
public:
    CProgressBar(QWidget *parent) : TWidget(parent)
        , m_crBackground(255,255,255)
        , m_crBuffer(210,255,230)
        , m_crForeground(155,210,255)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    QColor m_crBackground;
    QColor m_crBuffer;
    QColor m_crForeground;

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

    void setColor(cqcr crBackground, cqcr crBuffer, cqcr crForeground)
    {
        m_crBackground = crBackground;
        m_crBuffer = crBuffer;
        m_crForeground = crForeground;
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
    void _onPaint(CPainter& painter, cqrc) override
    {
        this->setDropShadowEffect(__ShadowColor(__ShadowAlpha), 1, 1);

        auto rc = this->rect();

        UINT xround = __size(3);
        painter.fillRectEx(rc, m_crBackground, xround);

        if (m_maxBuffer > 0 && m_bufferValue > 0)
        {
            rc.setRight(this->rect().right() * m_bufferValue/m_maxBuffer);

            auto crBegin = m_crBuffer;
            crBegin.setAlpha(crBegin.alpha()*m_bufferValue/m_maxBuffer);
            painter.fillRectEx(rc, crBegin, m_crBuffer, xround);
        }

        if (this->maximum() > 0 && this->value() > 0)
        {
            rc.setRight(this->rect().right() * this->value()/this->maximum());

            auto crBegin = m_crForeground;
            crBegin.setAlpha(crBegin.alpha()*this->value()/this->maximum());
            painter.fillRectEx(rc, crBegin, m_crForeground, xround);
        }

        painter.drawRectEx(this->rect(), QColor(255,255,255,50), 1, Qt::SolidLine, xround);
    }
};
