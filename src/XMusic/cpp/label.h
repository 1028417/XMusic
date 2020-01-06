
#pragma once

#include <QLabel>

#include "widget.h"

#include <QTextOption>

enum class E_LabelTextOption
{
    LTO_AutoFit,
    LtO_Elided
};

class CLabel : public CWidget<QLabel>
{
    Q_OBJECT
public:
    CLabel(QWidget *parent) : CWidget(parent, QPainter::TextAntialiasing)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    bool m_bCutPixmap = false;

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

    virtual cqcr textColor() const
    {
        if (m_bUseCustomColor)
        {
            return m_crText;
        }

        return g_crText;
    }

public:
    void setPixmapRound(UINT szRound)
    {
        m_szRound = szRound;
    }

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

    void setTextColor(cqcr crText)
    {
        m_bUseCustomColor = true;
        m_crText = crText;

        QPalette pe = QLabel::palette();
        pe.setColor(QPalette::WindowText, crText);
        QLabel::setPalette(pe);
    }

    void setTextColor(int r, int g, int b, int a=255)
    {
        setTextColor(QColor(r,g,b,a));
    }

    void setShadow(UINT uWidth, UINT uAlpha=__ShadowAlpha)
    {
        m_uShadowWidth = uWidth;
        m_uShadowAlpha = uAlpha;

        update();
    }
};
