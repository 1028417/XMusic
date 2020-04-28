
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
    E_LabelTextOption m_eTextOption = E_LabelTextOption::LTO_AutoFit;

    bool m_bUseCustomColor = false;
    QColor m_crText;

    UINT m_uShadowWidth = 1;
    UINT m_uShadowAlpha = __ShadowAlpha;

    UINT m_szRound = 0;

    QRect m_rc;

signals:
    void signal_click(CLabel*, const QPoint& pos);

private:
    void _onPaint(CPainter& painter, cqrc rc) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;

public:
    void setPixmapRound(UINT szRound)
    {
        m_szRound = szRound;
    }

    cqpm pixmap() const
    {
        auto pm = QLabel::pixmap();
        if (pm)
        {
            return *pm;
        }

        static QPixmap s_pixmap;
        return s_pixmap;
    }

    void setText(const QString &qsText, E_LabelTextOption eTextOption = E_LabelTextOption::LTO_AutoFit)
    {        
        m_eTextOption = eTextOption;
        QLabel::setText(qsText);
    }

    void setShadow(UINT uWidth, UINT uAlpha=__ShadowAlpha)
    {
        m_uShadowWidth = uWidth;
        m_uShadowAlpha = uAlpha;
        update();
    }
};
