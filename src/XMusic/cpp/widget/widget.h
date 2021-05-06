
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

template <typename T>
using TD_XObj = const typename QtPrivate::FunctionPointer<T>::Object*;

class CXObj
{
protected:
    CXObj() = default;

public:
    template <typename _sgn, typename _fn>
    inline void onSignal(Qt::ConnectionType connType, _sgn sgn, _fn fn)
    {
        auto ths = (TD_XObj<_sgn>)this;
        QObject::connect(ths, sgn, ths, std::move(fn), connType);
    }

    template <typename _sgn, typename _fn>
    inline void onUISignal(_sgn sgn, _fn fn)
    {
        Qt::ConnectionType connType = Qt::DirectConnection;
        connType = Qt::ConnectionType(connType | Qt::UniqueConnection);
        onSignal(connType, sgn, fn);
    }

    template <typename _sgn, typename _slot>
    inline void onSignal(Qt::ConnectionType connType, _sgn sgn, TD_XObj<_slot> recv, _slot slot)
    {
        auto ths = (TD_XObj<_sgn>)this;
        QObject::connect(ths, sgn, recv, slot, connType);
    }

    template <typename _sgn, typename _slot>
    inline void onUISignal(_sgn sgn, TD_XObj<_slot> recv, _slot slot)
    {
        Qt::ConnectionType connType = Qt::DirectConnection;
        connType = Qt::ConnectionType(connType | Qt::UniqueConnection);
        onSignal(connType, sgn, recv, slot);
    }

public:
    template <typename _slot, typename _sgn>
    inline void regSlot(Qt::ConnectionType connType,_slot slot, _sgn sgn, TD_XObj<_sgn> sender)
    {
        QObject::connect(sender, sgn, (TD_XObj<_slot>)this, slot, connType);
    }
    template <typename _slot, typename _sgn>
    inline void regSlot(Qt::ConnectionType connType, _slot slot, _sgn sgn
                        , const initializer_list<TD_XObj<_sgn>>& senders)
    {
        for (auto sender : senders)
        {
            regSlot(connType, slot, sgn, sender);
        }
    }
    template <typename _slot, typename _sgn>
    inline void regSlot(Qt::ConnectionType connType, _slot slot, _sgn sgn
                        , const SList<TD_XObj<_sgn>>& senders)
    {
        for (auto sender : senders)
        {
            regSlot(connType, slot, sgn, sender);
        }
    }

    template <typename _slot, typename _sgn>
    inline void regUISlot(_slot slot, _sgn sgn, TD_XObj<_sgn> sender)
    {
        Qt::ConnectionType connType = Qt::DirectConnection;
        connType = Qt::ConnectionType(connType | Qt::UniqueConnection);
        regSlot(connType, slot, sgn, sender);
    }
    template <typename _slot, typename _sgn>
    inline void regUISlot(_slot slot, _sgn sgn, const initializer_list<TD_XObj<_sgn>>& senders)
    {
        for (auto sender : senders)
        {
            regUISlot(slot, sgn, sender);
        }
    }
    template <typename _slot, typename _sgn, class T>
    inline void regUISlot(_slot slot, _sgn sgn, const SList<T>& senders)
    {
        for (auto sender : senders)
        {
            regUISlot(slot, sgn, sender);
        }
    }
};

template <class T>
class TWidget : public T, public CXObj
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

    //SList<Qt::GestureType> m_lstGestureType;

    bool m_bSetForeColor = false;
    QColor m_crFore;

    CTouchEvent m_teBegin;

    int m_xTouch = 0;
    int m_yTouch = 0;

    bool m_bTouch = false;

    bool m_bMousePress = false;

    bool m_bClicking = false;

public:
    /*void grabGesture(const list<Qt::GestureType>& lstGestureType)
    {
        m_lstGestureType.add(lstGestureType);
        for (auto gestureType : lstGestureType)
        {
            T::grabGesture(gestureType);
        }
    }*/

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

    void setFont()
    {
        T::setFont(CFont());
    }

    void setFont(float fSizeOffset, TD_FontWeight eWeight = CFont::g_eDefFontWeight, bool bItalic=false, bool bUnderline=false)
    {
        T::setFont(CFont(fSizeOffset, eWeight, bItalic, bUnderline));
    }

    void adjustFont(float fSizeOffset, TD_FontWeight eWeight, bool bItalic, bool bUnderline)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, eWeight, bItalic, bUnderline);
        T::setFont(font);
    }

    void adjustFont(float fSizeOffset, TD_FontWeight eWeight, bool bItalic)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, eWeight, bItalic);
        T::setFont(font);
    }

    void adjustFont(float fSizeOffset, TD_FontWeight eWeight)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, eWeight);
        T::setFont(font);
    }

    void adjustFont(bool bItalic, bool bUnderline)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        font.setUnderline(bUnderline);
        T::setFont(font);
    }

    void adjustFontSize(float fSizeOffset)
    {
        CFont font(*this);
        font.adjust(fSizeOffset);
        T::setFont(font);
    }

    void adjustFontWeight(TD_FontWeight eWeight)
    {
        CFont font(*this);
        font.setWeight(eWeight);
        T::setFont(font);
    }

    void adjustFontItalic(bool bItalic)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        T::setFont(font);
    }

    void flash()
    {
        setOpacityEffect(0.5f);
        UINT uDelayTime = 100;
#if __windows || __mac
        uDelayTime = 200;
#endif
        __async(uDelayTime, [&]{
            unsetOpacityEffect();
        });
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

    void setX(int x)
    {
        this->move(x, this->y());
    }
    void setY(int y)
    {
        this->move(this->x(), y);
    }

    void setWidth(UINT cx)
    {
        this->resize(cx, this->height());
    }
    void setHeight(UINT cy)
    {
        this->resize(this->width(), cy);
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
