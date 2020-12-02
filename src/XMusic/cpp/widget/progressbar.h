
#pragma once

#include <QProgressBar>

#include "widget.h"

class CProgressBar : public TWidget<QWidget>
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

    UINT m_value = 0;
    UINT m_maxValue = 0;

    UINT m_maxBuffer = 0;
    UINT m_buffer = 0;

public:
    UINT value() const
    {
        return m_value;
    }

    UINT maximum() const
    {
        return m_maxValue;
    }

    UINT maxBuffer() const
    {
        return m_maxBuffer;
    }
    UINT buffer() const
    {
        return m_buffer;
    }

    void setColor(cqcr crBackground, cqcr crBuffer, cqcr crForeground)
    {
        m_crBackground = crBackground;
        m_crBuffer = crBuffer;
        m_crForeground = crForeground;
        update();
    }

    void setMaximum(UINT maxValue)
    {
        m_maxValue = maxValue;
        update();
    }

    void setValue(UINT value)
    {
        m_value = value;
        update();
    }

    void setValue(UINT value, UINT maxValue)
    {
        m_value = value;
        m_maxValue = maxValue;
        update();
    }

    void setMaxBuffer(UINT maxBuffer)
    {
        m_maxBuffer = maxBuffer;
        update();
    }

    void setBuffer(UINT buffer)
    {
        m_buffer = buffer;
        update();
    }

    void setBuffer(UINT buffer, UINT maxBuffer)
    {
        m_buffer = buffer;
        m_maxBuffer = maxBuffer;
        update();
    }

    void set(UINT value, UINT maxValue, UINT buffer, UINT maxBuffer)
    {
        m_value = value;
        m_maxValue = maxValue;

        m_buffer = buffer;
        m_maxBuffer = maxBuffer;

        update();
    }

private:
    void _onPaint(CPainter& painter, cqrc) override
    {
        this->setDropShadowEffect(__ShadowColor(__ShadowAlpha), 1, 1);

        auto rc = this->rect();

        UINT xround = __size(3);
        painter.fillRectEx(rc, m_crBackground, xround);

        if (m_maxValue > 0)
        {
            if (m_maxBuffer > 0 && m_buffer > 0)
            {
                auto cx = width() * m_buffer/m_maxBuffer;
                QRect rc(0, 0, cx, height());

                auto crBegin = m_crBuffer;
                crBegin.setAlpha(crBegin.alpha()*m_buffer/m_maxBuffer);
                painter.fillRectEx(rc, crBegin, m_crBuffer, xround);
            }

            if (m_value > 0)
            {
                auto cx = width() * m_value/m_maxValue;
                QRect rc(0, 0, cx, height());

                auto crBegin = m_crForeground;
                crBegin.setAlpha(crBegin.alpha()*m_value/m_maxValue);
                painter.fillRectEx(rc, crBegin, m_crForeground, xround);
            }
        }

        painter.drawRectEx(rc, QColor(255,255,255,50), 1, Qt::SolidLine, xround);
    }
};
