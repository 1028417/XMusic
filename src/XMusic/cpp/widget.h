
#pragma once

#include <QWidget>

#include <QGesture>

#include <QGraphicsEffect>

#include <QMouseEvent>

#include <QTouchEvent>

#include "painter.h"

extern float g_fPixelRatio;

#if __ios
#define __size(x) decltype(x)((x)/g_fPixelRatio)
#define __rect(x) QRect(__size(x.left()), __size(x.top()), __size(x.width()), __size(x.height()))
#else
#define __size(x) (x)
#define __rect(x) (x)
#endif

#define __szRound __size(8)

enum class E_MouseEventType
{
    MET_Move,
    MET_Press,
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
        cauto points = te.touchPoints();
        if (!points.empty())
        {
            cauto pos = points.at(0).pos();
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
    CWidget(QWidget *parent, QPainter::RenderHints eRenderHints = __defRenderHints) :
        TWidget(parent),
        m_eRenderHints(eRenderHints)
    {
        setFont(1);
    }

private:
    QPainter::RenderHints m_eRenderHints;

    SSet<Qt::GestureType> m_setGestureType;

    CTouchEvent m_teBegin;

    int m_xTouch = 0;
    int m_yTouch = 0;

    bool m_bTouching = false;

    bool m_bMousePressed = false;

protected:
    bool m_bClicking = false;

public:
    void grabGesture(const list<Qt::GestureType>& lstGestureType)
    {
        m_setGestureType = lstGestureType;
        for (auto gestureType : m_setGestureType)
        {
            this->grabGesture(gestureType);
        }
    }

    void setFont(const QFont& font)
    {
        TWidget::setFont(font);
    }

    void setFont(float fSizeOffset, E_FontWeight eWeight = __defFontWeight, bool bItalic=false)
    {
        TWidget::setFont(CFont(fSizeOffset, eWeight, bItalic));
    }

    void adjustFont(float fSizeOffset, E_FontWeight eWeight, bool bItalic)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, eWeight, bItalic);
        TWidget::setFont(font);
    }

    void adjustFont(float fSizeOffset)
    {
        CFont font(*this);
        font.adjust(fSizeOffset);
        TWidget::setFont(font);
    }

    void adjustFontWeight(E_FontWeight eWeight)
    {
        CFont font(*this);
        font.setWeight(eWeight);
        TWidget::setFont(font);
    }

    void adjustFontItalic(bool bItalic)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        TWidget::setFont(font);
    }

    void setOpacityEffect(float fValue)
    {
        auto oe = dynamic_cast<QGraphicsOpacityEffect*>(this->graphicsEffect());
        if (NULL == oe)
        {
            oe = new QGraphicsOpacityEffect(this);
        }
        else
        {
            oe->setEnabled(true);
        }

        oe->setOpacity(fValue);

        this->setGraphicsEffect(oe);
    }

    void unsetOpacityEffect()
    {
        auto oe = dynamic_cast<QGraphicsOpacityEffect*>(this->graphicsEffect());
        if (oe)
        {
            oe->setEnabled(false);
        }
    }

    void setBlurEffect(float fStength)
    {
        auto be = dynamic_cast<QGraphicsBlurEffect*>(this->graphicsEffect());
        if (NULL == be)
        {
            be = new QGraphicsBlurEffect(this);
        }
        else
        {
            be->setEnabled(true);
        }

        be->setBlurRadius(fStength);

        this->setGraphicsEffect(be);
    }

    void unsetBlurEffect()
    {
        auto be = dynamic_cast<QGraphicsBlurEffect*>(this->graphicsEffect());
        if (be)
        {
            be->setEnabled(false);
        }
    }

    void setColorizeEffect(cqcr cr, float fStength)
    {
        auto ce = dynamic_cast<QGraphicsColorizeEffect*>(this->graphicsEffect());
        if (NULL == ce)
        {
            ce = new QGraphicsColorizeEffect(this);
        }
        else
        {
            ce->setEnabled(true);
        }

        ce->setColor(cr);
        ce->setStrength(fStength);

        this->setGraphicsEffect(ce);
    }

    void unsetColorizeEffect()
    {
        auto ce = dynamic_cast<QGraphicsColorizeEffect*>(this->graphicsEffect());
        if (ce)
        {
            ce->setEnabled(false);
        }
    }

    /*class CDropShadowEffect : public QGraphicsDropShadowEffect
    {
    public:
        CDropShadowEffect(QObject *parent) : QGraphicsDropShadowEffect(parent)
        {
        }

    private:
        void draw(QPainter *painter) override
        {
            //painter->fillRect()
            QGraphicsDropShadowEffect::draw(painter);
        }
    };*/

    template <class T = QGraphicsDropShadowEffect>
    void setDropShadowEffect(cqcr cr, int dx, int dy, float fBlur=0)
    {
        auto dse = dynamic_cast<T*>(this->graphicsEffect());
        if (NULL == dse)
        {
            dse = new T(this);
        }
        else
        {
            dse->setEnabled(true);
        }

        dse->setColor(cr);
        dse->setOffset(dx, dy);
        dse->setBlurRadius(fBlur);

        this->setGraphicsEffect(dse);
    }

    void unsetDropShadowEffect()
    {
        auto dse = dynamic_cast<QGraphicsDropShadowEffect*>(this->graphicsEffect());
        if (dse)
        {
            dse->setEnabled(false);
        }
    }

protected:
    virtual bool event(QEvent *ev) override;

    bool isHLayout() const
    {
        return TWidget::width()>TWidget::height();
    }

private:
    void paintEvent(QPaintEvent *pe) override;

    virtual void _onPaint(CPainter&, cqrc rc);

    void _handleTouchBegin(const CTouchEvent&);
    void _handleTouchEnd(CTouchEvent);
    void _handleTouchMove(CTouchEvent);

    void _handleTouchEvent(E_TouchEventType, const QTouchEvent&);

    void _handleMouseEvent(E_MouseEventType, const QMouseEvent&);

    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) {}

    virtual void _onMouseEnter() {}
    virtual void _onMouseLeave() {}

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) {}

    virtual bool _onGesture(QGesture&) {return false;}
};
