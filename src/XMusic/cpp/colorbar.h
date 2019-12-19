
#pragma once

#include <QProgressBar>

#include "widget.h"

#include <QBrush>

enum class E_BarColor
{
    BC_Red,
    BC_Green,
    BC_Blue,
};

class CColorBar : public CWidget<QWidget>
{
    Q_OBJECT
public:
    CColorBar(QWidget *parent) : CWidget(parent)
    {
    }

    void setColor(E_BarColor eColor, uint8_t uValue)
    {
        m_eColor = eColor;

        m_uValue = uValue;
    }

    void setValue(uint8_t uValue)
    {
        if (uValue != m_uValue)
        {
            m_uValue = uValue;
            update();

            emit signal_valueChanged(this, m_uValue);
        }
    }

    uint8_t value() const
    {
        return m_uValue;
    }

private:
    E_BarColor m_eColor;
    uint8_t m_uValue = 0;

    UINT m_uMargin = 0;

signals:
    void signal_valueChanged(CColorBar*, uint8_t uValue);

public:
    void setMargin(UINT uMargin)
    {
        m_uMargin = uMargin;
        update();
    }

private:
    void _onPaint(CPainter& painter, const QRect&) override
    {
        painter.setPen(Qt::transparent);

        QRect rc = this->rect();
        painter.fillRect(rc, g_crTheme);

        rc.setTop(rc.top() + m_uMargin);
        rc.setBottom(rc.bottom() - m_uMargin);

        auto cy = rc.height();

        QColor crBegin(0,0,0);
        UINT xround = cy/2;
        if (E_BarColor::BC_Red == m_eColor)
        {
            painter.fillRectEx(rc, crBegin, QColor(255,0,0), xround);
        }
        else if (E_BarColor::BC_Green == m_eColor)
        {
            painter.fillRectEx(rc, crBegin, QColor(0,255,0), xround);
        }
        else if (E_BarColor::BC_Blue == m_eColor)
        {
            painter.fillRectEx(rc, crBegin, QColor(0,0,255), xround);
        }

        auto margin = cy/4+1;
        auto len = cy+1;
        int x = margin + m_uValue*(rc.width()-margin*2-len)/255;
        painter.setBrush(Qt::GlobalColor::white);

        QRect rcPos(x, rc.top()+margin, len, cy-margin*2);
        painter.drawRectEx(rcPos, rcPos.height()/2);
    }

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent& te) override
    {
        auto len = height();
        auto margin = 3;

        int nValue = 255*(te.x()-len/2-margin)/(width()-margin*2-len);
        nValue = MAX(0, nValue);
        nValue = MIN(255, nValue);

        setValue((uint8_t)nValue);
    }
};
