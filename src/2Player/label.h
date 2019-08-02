
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
            QPen pen = painter.pen();

            QTextOption to;
            to.setAlignment(this->alignment());

            QRect rcText(pos);

            QColor crShadow(m_crShadow);

            for (UINT uIdx=0; uIdx<m_uShadowWidth; uIdx++)
            {
                rcText.setLeft(rcText.left()+1);
                rcText.setTop(rcText.top()+1);

                crShadow.setAlpha(m_crShadow.alpha()*(m_uShadowWidth-uIdx)/m_uShadowWidth);
                painter.setPen(crShadow);

                painter.drawText(rcText, this->text(), to);
            }

            painter.setPen(pen);
            painter.drawText(pos, this->text(), to);
        }
        else
        {
            CWidget<QLabel>::_onPaint(painter, pos);
        }
    }
};
