
#pragma once

#include <QMouseEvent>

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
    CLabel(QWidget *parent) : CWidget(parent)
      //, m_crShadow(128,128,128)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    bool m_bCutPixmap = false;

    E_LabelTextOption m_eTextOption = E_LabelTextOption::LTO_AutoFit;

    bool m_bUseCustomColor = false;
    QColor m_crText;

    UINT m_uShadowAlpha = 255;
    UINT m_uShadowWidth = 0;

    QColor m_crShadow;

    QRect m_rc;

signals:
    void signal_click(CLabel*, const QPoint& pos);

private:
    void _onPaint(CPainter& painter, const QRect& rc) override;

    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;

    virtual const QColor& textColor() const
    {
        if (m_bUseCustomColor)
        {
            return m_crText;
        }

        return g_crText;
    }

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

    void setTextColor(const QColor& crText)
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

    void setShadow(UINT uAlpha, UINT uWidth = 1, int r=128, int g=128, int b=128)
    {
        m_uShadowAlpha = uAlpha;
        m_uShadowWidth = uWidth;

        m_crShadow.setRgb(QRGB(r,g,b));

        update();
    }
};
