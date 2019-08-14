
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
    bool m_bCutPixmap = false;

    bool m_bAutoFit = false;

    UINT m_uShadowWidth = 0;
    QColor m_crShadow;

    QRect m_rc;

signals:
    void signal_click(CLabel*, const QPoint& pos);

public:
    void setPixmap(const QPixmap &pixmap, bool bCut=true)
    {
        m_bCutPixmap = bCut;
        QLabel::setPixmap(pixmap);
    }

    void setText(const QString &qsText, bool bAutoFit=true)
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
    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override
    {
        if (E_MouseEventType::MET_Click == type)
        {
            if (m_rc.contains(me.pos()))
            {
                emit signal_click(this, me.pos());
            }
        }
    }

    void _onPaint(CPainter& painter, const QRect&) override
    {
        m_rc = this->rect();

        const QPixmap *pPixmap = pixmap();
        if (pPixmap && !pPixmap->isNull())
        {
            painter.drawPixmapEx(m_rc, *pPixmap);
        }

        QString text = this->text();
        if (!text.isEmpty())
        {
            int cx = m_rc.right();

            if (m_bAutoFit)
            {
                while (painter.fontMetrics().width(text) >= cx)
                {
                    QFont font = painter.font();
                    font.setPointSizeF(font.pointSizeF()-0.1f);
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

                    painter.drawText(QRectF(uIdx, uIdx, cx, m_rc.bottom()), alignment, text);
               }

               painter.setPen(pen);
            }

            painter.drawText(m_rc, alignment, text, &m_rc);

            //return;
        }

        //CWidget<QLabel>::_onPaint(painter, rc);
    }
};
