
#pragma once

#include <QWidget>

#include <QGesture>

#include <QGraphicsEffect>

#include <QMouseEvent>

#include <QTouchEvent>

#include "painter.h"

#if __windows || __mac
#if __isdebug
#define __fastTouchDt 300
#else
#define __fastTouchDt 200 // 鼠标慢，触屏快
#endif
#else
#define __fastTouchDt 130
#endif

#if __ios
#define __size(x) ((x)/g_screen.pixelRatio) //decltype(x)((x)/g_screen.fPixelRatio)
#define __rect(x) QRect(__size(x.left()), __size(x.top()), __size(x.width()), __size(x.height()))
#else
#define __size(x) (x)
#define __rect(x) (x)
#endif

#define __size10 __size(10)
#define __size100 __size(100)

#define __szRound __size10

#define __png(name)  ":/img/" #name ".png"
#define __mdlPng(name) ":/img/medialib/" #name ".png"

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
    CTouchEvent() = default;

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

template <class T>
class TWidget : public T
{
public:
    TWidget(QWidget *parent, QPainter::RenderHints eRenderHints = __defRenderHints)
        : T(parent)
        , m_eRenderHints(eRenderHints)
    {
    }

    TWidget(QWidget *parent, Qt::WindowFlags f, QPainter::RenderHints eRenderHints = __defRenderHints)
        : T(parent, f)
        , m_eRenderHints(eRenderHints)
    {
    }

private:
    QPainter::RenderHints m_eRenderHints;

    SSet<Qt::GestureType> m_setGestureType;

    bool m_bSetForeColor = false;
    QColor m_crFore;

    CTouchEvent m_teBegin;

    int m_xTouch = 0;
    int m_yTouch = 0;

    bool m_bTouch = false;

    bool m_bMousePress = false;

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

    void setForeColor(cqcr crFore)
    {
        m_bSetForeColor = true;
        m_crFore = crFore;
        T::update();
    }
    void setForeColor(int r, int g, int b, int a=255)
    {
        setForeColor(QColor(r,g,b,a));
    }

    void setFont(const CFont& font)
    {
        T::setFont(font);
    }

    void setFont(float fSizeOffset, int nWeight = CFont::g_nDefFontWeight, bool bItalic=false, bool bUnderline=false)
    {
        T::setFont(CFont(fSizeOffset, nWeight, bItalic, bUnderline));
    }

    void adjustFont(float fSizeOffset, int nWeight, bool bItalic, bool bUnderline)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, nWeight, bItalic, bUnderline);
        T::setFont(font);
    }

    void adjustFont(float fSizeOffset, int nWeight, bool bItalic)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, nWeight, bItalic);
        T::setFont(font);
    }

    void adjustFont(float fSizeOffset, int nWeight)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, nWeight);
        T::setFont(font);
    }

    void adjustFont(float fSizeOffset)
    {
        CFont font(*this);
        font.adjust(fSizeOffset);
        T::setFont(font);
    }

    void adjustFont(int nWeight)
    {
        CFont font(*this);
        font.setWeight(nWeight);
        T::setFont(font);
    }

    void adjustFont(bool bItalic)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        T::setFont(font);
    }

    void adjustFont(bool bItalic, bool bUnderline)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        font.setUnderline(bUnderline);
        T::setFont(font);
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

    template <class E = QGraphicsDropShadowEffect>
    void setDropShadowEffect(cqcr cr, int dx, int dy, float fBlur=0)
    {
        auto dse = dynamic_cast<E*>(this->graphicsEffect());
        if (NULL == dse)
        {
            dse = new E(this);
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

    virtual cqcr foreColor() const
    {
        if (m_bSetForeColor)
        {
            return m_crFore;
        }

        return g_crFore;
    }

    void disableClick()
    {
        m_bClicking = false;
    }

    virtual void _onPaint(CPainter&, cqrc rc);

private:
    void paintEvent(QPaintEvent *pe) override;

    void _handleTouchBegin(const CTouchEvent&);
    void _handleTouchEnd(CTouchEvent);
    void _handleTouchMove(CTouchEvent&);

    void _handleTouchEvent(E_TouchEventType, const QTouchEvent&);

    void _handleMouseEvent(E_MouseEventType, const QMouseEvent&);

    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) {}

    virtual void _onMouseEnter() {}
    virtual void _onMouseLeave() {}

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) {}

    virtual bool _onGesture(QGesture&) {return false;}
};
