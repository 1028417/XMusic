
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
    UINT m_uShadowWidth = 0;
    QColor m_crShadow;

signals:
    void signal_mousePressEvent(CLabel*);
    void signal_mousePressEvent(CLabel*, const QPoint& pos);

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
    void mousePressEvent(QMouseEvent *ev) override
    {
        QLabel::mousePressEvent(ev);

        emit signal_mousePressEvent(this);
        emit signal_mousePressEvent(this, ev->pos());
    }

    void _onPaint(QPainter& painter, const QRect& pos) override
    {
        if (0 != m_uShadowWidth)
        {
            const QString& text = this->text();
            if (!text.isEmpty())
            {
                QPen pen = painter.pen();

                QTextOption to;
                to.setAlignment(this->alignment());

                for (UINT uIdx=0; uIdx<m_uShadowWidth; uIdx++)
                {
                    m_crShadow.setAlpha(255*(m_uShadowWidth-uIdx)/m_uShadowWidth);
                    painter.setPen(m_crShadow);

                    QRectF rcShadow(pos.left()+uIdx, pos.top()+uIdx, pos.width(), pos.height());
                    painter.drawText(rcShadow, text, to);
                }

                painter.setPen(pen);
                painter.drawText(pos, text, to);

                return;
            }
        }

        CWidget<QLabel>::_onPaint(painter, pos);
    }
};
