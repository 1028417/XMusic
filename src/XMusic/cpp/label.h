
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
    CLabel(QWidget *parent) : CWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    bool m_bCutPixmap = false;

    E_LabelTextOption m_eTextOption = E_LabelTextOption::LTO_AutoFit;

    UINT m_uShadowAlpha = 255;
    UINT m_uShadowWidth = 0;

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

    void setShadow(UINT uAlpha, UINT uWidth = 1)
    {
        m_uShadowAlpha = uAlpha;
        m_uShadowWidth = uWidth;

        update();
    }

private:
    void _onPaint(CPainter& painter, const QRect& rc) override;
	
    void _onMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;
};
