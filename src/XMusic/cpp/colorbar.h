
#pragma once

#include <QProgressBar>

#include "widget.h"

#include <QBrush>

class CColorBar : public CWidget<QProgressBar>
{
public:
    CColorBar(QWidget *parent) : CWidget<QProgressBar>(parent)
    {
    }

    void setColor(const QColor& color)
    {
        m_color = color;
    }

private:
    QColor m_color;

private:
    void _onPaint(CPainter& painter, const QRect&) override
    {
        QRect rect = this->rect();
        QLinearGradient gradient(rect.topLeft(), rect.topRight());
        gradient.setColorAt(0, QColor(0,0,0));
        gradient.setColorAt(1, QColor(255,0,0));
        QBrush br(gradient);
        painter.fillRect(rect, br);
    }
};
