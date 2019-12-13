
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

        setValue(uValue);
        update();
    }

    void setValue(uint8_t uValue)
    {
        m_uValue = uValue;
        update();
    }

    uint8_t value() const
    {
        return m_uValue;
    }

private:
    E_BarColor m_eColor;
    uint8_t m_uValue = 0;

signals:
    void signal_click(CColorBar*, const QPoint& pos);

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

        QBrush br(gradient);
        painter.fillRect(rc, br);

        auto cy = rc.height();
        auto len = cy*2;
        auto margin = 2;
        int x = margin + m_uValue*(rc.width()-margin*2-len)/255;
        painter.fillRect(x, rc.top()+margin, len, cy-margin*2);
    }

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me)
    {
        if (E_MouseEventType::MET_Click == type)
        {
            emit signal_click(this, me.pos());
        }
    }
};
