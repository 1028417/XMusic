
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

signals:
    void signal_valueChanged(CColorBar*, uint8_t uValue);

private:
    void _onPaint(CPainter& painter, const QRect&) override
    {
        QRect rc = this->rect();
        QLinearGradient gradient(rc.topLeft(), rc.topRight());
        gradient.setColorAt(0, QColor(0,0,0));

        if (E_BarColor::BC_Red == m_eColor)
        {
            gradient.setColorAt(1, QColor(255,0,0));
        }
        else if (E_BarColor::BC_Green == m_eColor)
        {
            gradient.setColorAt(1, QColor(0,255,0));
        }
        else if (E_BarColor::BC_Blue == m_eColor)
        {
            gradient.setColorAt(1, QColor(0,0,255));
        }

        painter.setPen(Qt::transparent);

        QBrush br(gradient);
        painter.setBrush(br);
        painter.drawRoundedRect(rc, 3, 3);

        auto cy = rc.height();
        auto len = cy;
        auto margin = 3;
        int x = margin + m_uValue*(rc.width()-margin*2-len)/255;
        painter.setBrush(Qt::GlobalColor::white);
        painter.drawRoundedRect(x, rc.top()+margin, len, cy-margin*2, 3, 3);
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
