
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
    bool m_bAutoFit = false;

    UINT m_uShadowWidth = 2;
    QColor m_crShadow;

    QRect m_rcText;

signals:
    void signal_click(CLabel*, const QPoint& pos);

public:
    void setText(const QString &qsText, bool bAutoFit=false)
    {
        QLabel::setText(qsText);
        m_bAutoFit = bAutoFit;
    }

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
            if (m_rcText.contains(ev.pos()))
            {
                emit signal_click(this, ev.pos());
            }
        }
    }

    void _onPaint(QPainter& painter, const QRect& rc) override
    {
        m_rcText = this->rect();
        QString text = this->text();
        if (!text.isEmpty())
        {
            int cx = m_rcText.right();

            if (m_bAutoFit)
            {
                while (painter.fontMetrics().width(text) >= cx)
                {
                    QFont font = painter.font();
                    font.setPointSizeF(font.pointSizeF()-0.5f);
                    painter.setFont(font);
                }
            }
            else
            {
                text = painter.fontMetrics().elidedText(text, Qt::ElideRight, cx, Qt::TextShowMnemonic);
            }

            auto alignment = this->alignment();

            if (0 != m_uShadowWidth)
            {
                QPen pen = painter.pen();

                UINT uShadowAlpha = m_crText.alpha();
                if (uShadowAlpha < 255)
                {
                    uShadowAlpha /= 2;
                }

                QColor crShadow = m_crShadow;
                crShadow.setAlpha(uShadowAlpha);
                painter.setPen(crShadow);

                for (UINT uIdx=0; uIdx<=m_uShadowWidth; uIdx++)
                {
                    if (uIdx > 1)
                    {
                        crShadow.setAlpha(uShadowAlpha*(m_uShadowWidth-uIdx+1)/m_uShadowWidth);
                        painter.setPen(crShadow);
                    }

                    painter.drawText(QRectF(uIdx, uIdx, cx, m_rcText.bottom()), alignment, text);
               }

               painter.setPen(pen);
            }

            painter.drawText(m_rcText, alignment, text, &m_rcText);
        }
        else
        {
            CWidget<QLabel>::_onPaint(painter, rc);
        }
    }
};
