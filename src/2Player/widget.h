
#pragma once

#include <QWidget>

#include <QPainter>

#include <QGesture>

#include <QGraphicsOpacityEffect>

#include "util.h"

enum class E_FontWeight
{
    FW_Light = QFont::Weight::Light,
    //FW_Normal = QFont::Weight::Normal,
    FW_SemiBold = QFont::Weight::DemiBold
};

class CPainter : public QPainter
{
public:
    CPainter(QWidget* widget)
        : QPainter(widget)
    {
    }

    void drawPixmapEx(const QRect& rcDst, const QPixmap& pixmap)
    {
        QRect rcSrc = pixmap.rect();
        float fHWRate = 1;
        int height = rcSrc.height();
        int width = rcSrc.width();
        if ((float)height/width > fHWRate)
        {
            int dy = (height - width*fHWRate)/2;
            rcSrc.setTop(rcSrc.top()+dy);
            rcSrc.setBottom(rcSrc.bottom()-dy);
        }
        else
        {
            int dx = (width - height/fHWRate)/2;
            rcSrc.setLeft(rcSrc.left()+dx);
            rcSrc.setRight(rcSrc.right()-dx);
        }

        this->drawPixmap(rcDst, pixmap, rcSrc);
    }
};

enum class E_MouseEventType
{
    MET_Press,
    MET_Move,
    MET_Release,
    MET_Click,
    MET_DblClick
};

enum class E_TouchEventType
{
    TET_TouchBegin,
    TET_TouchMove,
    TET_TouchEnd
};

class CTouchEvent
{
public:
    CTouchEvent(){}

    CTouchEvent(const QMouseEvent& me)
        : m_ulTimestamp(me.timestamp())
        , m_x(me.pos().x())
        , m_y(me.pos().y())
    {
    }

    CTouchEvent(const QTouchEvent& te)
        : m_ulTimestamp(te.timestamp())
    {
        cauto& points = te.touchPoints();
        if (!points.empty())
        {
            cauto& pos = points.at(0).pos();
            m_x = ((int)pos.x());
            m_y = ((int)pos.y());
        }
    }

private:
    ulong m_ulTimestamp = 0;

    int m_x = 0;
    int m_y = 0;

    ulong m_dt = 0;

    int m_dx = 0;
    int m_dy = 0;

public:
    ulong timestamp() const
    {
        return m_ulTimestamp;
    }

    int x() const
    {
        return m_x;
    }
    int y() const
    {
        return m_y;
    }
    QPoint pos() const
    {
        return QPoint(m_x, m_y);
    }

    ulong dt() const
    {
        return m_dt;
    }

    int dx() const
    {
        return m_dx;
    }
    int dy() const
    {
        return m_dy;
    }

    void setDt(ulong dt)
    {
        m_dt = dt;
    }

    void setDx(int dx)
    {
        m_dx = dx;
    }
    void setDy(int dy)
    {
        m_dy = dy;
    }
};

template <class TWidget = QWidget>
class CWidget : public TWidget
{
public:
    CWidget(QWidget *parent, const list<Qt::GestureType>& lstGestureType={}) :
        TWidget(parent),
        m_lstGestureType(lstGestureType)
    {
        for (auto gestureType : m_lstGestureType)
        {
            this->grabGesture(gestureType);
        }
    }

private:
    list<Qt::GestureType> m_lstGestureType;

    CTouchEvent m_teBegin;

    QPointF m_ptTouch;

    bool m_bTouching = false;

    bool m_bMousePressed = false;

    QGraphicsOpacityEffect m_goe;

    bool m_bClicking = false;

protected:
    QColor m_crText;

public:
    void setOpacity(float fValue)
    {
       m_goe.setOpacity(fValue);
       this->setGraphicsEffect(&m_goe);
    }

    const QColor& textColor() const
    {
        return m_crText;
    }

    void setTextColor(const QColor& crText)
    {
        m_crText = crText;

        QPalette pe = TWidget::palette();
        pe.setColor(QPalette::WindowText, crText);
        TWidget::setPalette(pe);
    }

    void setTextColor(UINT r, UINT g, UINT b, UINT a=255)
    {
        setTextColor(QColor(r,g,b,a));
    }

    void setTextAlpha(UINT uAlpha)
    {
        if ((int)uAlpha != m_crText.alpha())
        {
            m_crText.setAlpha(uAlpha);
            setTextColor(m_crText);
        }
    }

    void adjustFont(float fSizeOffset, E_FontWeight eWeight, bool bItalic)
    {
        QFont font = TWidget::font();
        font.setPointSizeF(font.pointSizeF() + fSizeOffset);
        font.setWeight((int)eWeight);
        font.setItalic(bItalic);
        TWidget::setFont(font);
    }

    void adjustFontSize(float fSizeOffset)
    {
        QFont font = TWidget::font();
        font.setPointSizeF(font.pointSizeF() + fSizeOffset);
        TWidget::setFont(font);
    }

    void adjustFontWeight(E_FontWeight eWeight)
    {
        QFont font = TWidget::font();
        font.setWeight((int)eWeight);
        TWidget::setFont(font);
    }

    void adjustFontItalic(bool bItalic)
    {
        QFont font = TWidget::font();
        font.setItalic(bItalic);
        TWidget::setFont(font);
    }

protected:
    virtual bool event(QEvent *ev) override;

    virtual void _onPaint(CPainter&, const QRect& rc);

private:
    void paintEvent(QPaintEvent *pe) override;

    void _handleTouchBegin(const CTouchEvent&);
    void _handleTouchEnd(const CTouchEvent&);
    void _handleTouchMove(const CTouchEvent&);

    void _handleMouseEvent(E_MouseEventType, const QMouseEvent&);
    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) {}

    virtual void _onMouseEnter() {}
    virtual void _onMouseLeave() {}

    void _handleTouchEvent(E_TouchEventType, const QTouchEvent&);
    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) {}

    virtual bool _onGesture(QGesture&) {return false;}
};
