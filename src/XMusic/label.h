
#pragma once

#include <QMouseEvent>

#include <QLabel>

#include "widget.h"

#include <QTextOption>

enum class E_LabelTextOption
{
    LTO_None = 0,
    LTO_AutoFit,
    LtO_Elided
};

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

    E_LabelTextOption m_eTextOption = E_LabelTextOption::LTO_AutoFit;

    UINT m_uShadowWidth = 0;
    QColor m_crShadow;

    QRect m_rc;

signals:
    void signal_click(CLabel*, const QPoint& pos);

public:
    void setPixmap(const QPixmap & pm, bool bCut=true)
    {
        m_bCutPixmap = bCut;
        QLabel::setPixmap(pm);
    }

    void setText(const QString &qsText, E_LabelTextOption eTextOption = E_LabelTextOption::LTO_AutoFit)
    {        
        m_eTextOption = eTextOption;

        QLabel::setText(qsText);
    }

    void setShadow(UINT uWidth, UINT r, UINT g, UINT b)
    {
        m_uShadowWidth = uWidth;
        m_crShadow.setRgb(r,g,b);

        update();
    }

    void setShadow(UINT uWidth)
    {
        m_uShadowWidth = uWidth;

        update();
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

    void _onPaint(CPainter& painter, const QRect& rc) override
    {
        m_rc = this->rect();

        auto pm = pixmap();
        if (pm && !pm->isNull())
        {
            painter.drawPixmapEx(m_rc, *pm);
            return;
        }

        QString text = this->text();
        if (!text.isEmpty() && E_LabelTextOption::LTO_None != m_eTextOption)
        {
            int cx = m_rc.right();

            if (E_LabelTextOption::LTO_AutoFit == m_eTextOption)
            {
                QFont font = painter.font();
                while (painter.fontMetrics().width(text) >= cx)
                {
                    auto fPointSize = font.pointSizeF()-0.1;
                    if (fPointSize < 0)
                    {
                        break;
                    }

                    font.setPointSizeF(fPointSize);
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

            return;
        }

        CWidget<QLabel>::_onPaint(painter, rc);
    }
};
