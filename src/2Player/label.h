
#pragma once

#include <QMouseEvent>

#include <QLabel>

#include "widget.h"

#include <QTextOption>

class CLabel : public CWidget<QLabel>
{
    Q_OBJECT

public:
    CLabel(QWidget *parent) :
        CWidget<QLabel>(parent)
        , m_crShadow(128,128,128)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    UINT m_uShadowWidth = 2;
    QColor m_crShadow;

signals:
    void signal_click(CLabel*, const QPoint& pos);

public:
    void setShadow(UINT uWidth, const QColor& crShadow)
    {
        m_uShadowWidth = uWidth;
        m_crShadow = crShadow;

        CWidget::update();
    }

    void setShadowWidth(UINT uWidth)
    {
        m_uShadowWidth = uWidth;

        CWidget::update();
    }

    void setShadowColor(const QColor& crShadow)
    {
        m_crShadow = crShadow;

        CWidget::update();
    }

private:
    void _handleMouseEvent(E_MouseEventType type, QMouseEvent& ev) override
    {
        if (E_MouseEventType::MET_Click == type)
        {
            emit signal_click(this, ev.pos());
        }
    }

    void _onPaint(QPainter& painter, const QRect& rc) override
    {
        const QString& text = this->text();
        if (!text.isEmpty())
        {
            int cx = this->rect().right();
            int cy = this->rect().bottom ();

            while (painter.fontMetrics().width(text) >= cx)
            {
                QFont font = painter.font();
                font.setPointSizeF(font.pointSizeF()-0.5f);
                painter.setFont(font);
            }

            QTextOption to;
            to.setAlignment(this->alignment());

            if (0 != m_uShadowWidth)
            {
                QPen pen = painter.pen();

                QColor crShadow = m_crShadow;
                painter.setPen(crShadow);
                for (UINT uIdx=0; uIdx<=m_uShadowWidth; uIdx++)
                {
                    if (uIdx > 1)
                    {
                        crShadow.setAlpha(m_crShadow.alpha()*(m_uShadowWidth-uIdx+1)/m_uShadowWidth);
                        painter.setPen(crShadow);
                    }

                    painter.drawText(QRectF(uIdx, uIdx, cx, cy), text, to);
               }

               painter.setPen(pen);
            }

            painter.drawText(this->rect(), text, to);

            return;
        }

        CWidget<QLabel>::_onPaint(painter, rc);
    }
};
