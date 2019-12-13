
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

class CColorBar : public CWidget<QProgressBar>
{
public:
    CColorBar(QWidget *parent) : CWidget<QProgressBar>(parent)
    {
    }

    void setColor(E_BarColor eColor, uint8_t uValue)
    {
        m_eColor = eColor;
        m_uValue = uValue;
    }

private:
    E_BarColor m_eColor;
    uint8_t m_uValue = 0;

private:
    void _onPaint(CPainter& painter, const QRect&) override
    {
        QRect rect = this->rect();
        QLinearGradient gradient(rect.topLeft(), rect.topRight());
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

        QBrush br(gradient);
        painter.fillRect(rect, br);
    }
};
