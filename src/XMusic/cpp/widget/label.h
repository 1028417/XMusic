
#pragma once

#include <QLabel>

#include "widget.h"

#include <QTextOption>

enum class E_LabelTextOption
{
    LTO_AutoFit,
    LtO_Elided
};

class CLabel : public TWidget<QLabel>
{
    Q_OBJECT
public:
    CLabel(QWidget *parent) : TWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    QBrush m_br;
    int m_cxPm = 0;
    int m_cyPm = 0;

    QColor m_crText;
    int m_flag = -1;

    bool m_bUseCustomColor = false;

    UINT m_uShadowWidth = 1;
    UINT m_uShadowAlpha = __ShadowAlpha;

    UINT m_szRound = 0;

    QRect m_rcText;

signals:
    void signal_click(CLabel*, const QPoint& pos);

private:
    void _onPaint(CPainter& painter, cqrc rc) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;

public:
    bool pixmap() const
    {
        return m_cxPm > 0;
    }

    int pixmapWidth() const
    {
        return m_cxPm;
    }
    int pixmapHeight() const
    {
        return m_cyPm;
    }

    void setPixmap(cqpm pm)
    {
        m_br.setTexture(pm);
        m_cxPm = pm.width();
        m_cyPm = pm.height();
    }

    void setPixmapRound(UINT szRound)
    {
        m_szRound = szRound;
    }

    void setText(const QString &qsText, int flag = -1)
    {
        m_flag = flag;
        QLabel::setText(qsText);
    }

    void setShadow(UINT uWidth, UINT uAlpha=__ShadowAlpha)
    {
        m_uShadowWidth = uWidth;
        m_uShadowAlpha = uAlpha;
        update();
    }

    void clear()
    {
        QLabel::clear();

        m_br = QBrush();
        m_cxPm = m_cyPm = 0;
    }
};
