
#pragma once

#include "util.h"

#include <QWidget>

#include <QGesture>

#include <QPainter>

#include <QGraphicsOpacityEffect>

#include <QFont>

enum class E_FontWeight
{
    FW_Light = QFont::Weight::Light,
    //FW_Normal = QFont::Weight::Normal,
    FW_SemiBold = QFont::Weight::DemiBold
};

#define __defFontWeight E_FontWeight::FW_Light

extern map<E_FontWeight, QFont> g_mapFont;

class CFont : public QFont
{
public:
    CFont(const QWidget& widget)
        : QFont(widget.font())
    {
    }

    CFont(const QPainter& painter)
        : QFont(painter.font())
    {
    }

    CFont(float fSizeOffset, E_FontWeight eWeight = __defFontWeight, bool bItalic=false)
        : QFont(g_mapFont[eWeight])
    {
        setPointSizeF(pointSizeF() + fSizeOffset);
        QFont::setWeight((int)eWeight);
        setItalic(bItalic);
    }

    void setWeight(E_FontWeight eWeight)
    {
        setFamily(g_mapFont[eWeight].family());
        QFont::setWeight((int)eWeight);
    }

    E_FontWeight weight() const
    {
        return (E_FontWeight)QFont::weight();
    }
};

class CPainter : public QPainter
{
public:
    CPainter(QWidget* widget)
        : QPainter(widget)
    {
    }

public:
    QColor mixColor(const QColor& crSrc, const QColor& crDst, UINT uAlpha)
    {
        int r = crSrc.red();
        int g = crSrc.green();
        int b = crSrc.blue();
        int a = crSrc.blue();

        r += (-r + crDst.red())*uAlpha / 255;
        g += (-g + crDst.green())*uAlpha / 255;
        b += (-b + crDst.blue())*uAlpha / 255;
        a += (-a + crDst.alpha())*uAlpha / 255;

        return QColor(r,g,b,a);
    }

    void setPenColor(int r, int g, int b, int a=255)
    {
        setPen(QColor(r,g,b,a));
    }

    void setFont(const QFont& font)
    {
        QPainter::setFont(font);
    }

    void setFont(float fSizeOffset, E_FontWeight eWeight = __defFontWeight, bool bItalic=false)
    {
        QPainter::setFont(CFont(fSizeOffset, eWeight, bItalic));
    }

    void adjustFont(float fSizeOffset, E_FontWeight eWeight, bool bItalic)
    {
        CFont font(*this);
        font.setPointSizeF(font.pointSizeF() + fSizeOffset);
        font.setWeight(eWeight);
        font.setItalic(bItalic);
        QPainter::setFont(font);
    }

    void adjustFontSize(float fSizeOffset)
    {
        CFont font(*this);
        font.setPointSizeF(font.pointSizeF() + fSizeOffset);
        QPainter::setFont(font);
    }

    void adjustFontWeight(E_FontWeight eWeight)
    {
        CFont font(*this);
        font.setWeight(eWeight);
        QPainter::setFont(font);
    }

    void adjustFontItalic(bool bItalic)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        QPainter::setFont(font);
    }

    void drawPixmapEx(const QRect& rcDst, const WString& strImgFile)
    {
        QPixmap pm;
        if (pm.load(strImgFile))
        {
            drawPixmapEx(rcDst, pm);
        }
    }

    template <class T>
    void _drawFrame(UINT uWidth, const QRect& rc, const T& t)
    {
        fillRect(rc.left(), rc.top(), rc.width(), uWidth, t);
        fillRect(rc.left(), rc.top()+uWidth, uWidth, rc.height()-uWidth*2, t);
        fillRect(rc.left(), rc.bottom()-uWidth, rc.width(), uWidth, t);
        fillRect(rc.right()-uWidth, rc.top()+uWidth, uWidth, rc.height()-uWidth*2, t);
    }

    void drawFrame(UINT uWidth, const QRect& rc, const QColor& cr, Qt::BrushStyle bs=Qt::SolidPattern)
    {
        if (Qt::SolidPattern == bs)
        {
            _drawFrame(uWidth, rc, cr);
        }
        else
        {
            _drawFrame(uWidth, rc, QBrush(cr, bs));
        }
    }

    void drawFrame(UINT uWidth, const QRect& rc, int r, int g, int b, int a=255, Qt::BrushStyle bs=Qt::SolidPattern)
    {
        drawFrame(uWidth, rc, QColor(r,g,b,a), bs);
    }

    void drawPixmapEx(const QRect& rcDst, const QPixmap& pixmap)
    {
        QRect rcSrc = pixmap.rect();
        int height = rcSrc.height();
        int width = rcSrc.width();

        float fHWRate = (float)rcDst.height()/rcDst.width();
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

protected:
    QColor m_crText;

    bool m_bClicking = false;

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

    void setTextColor(int r, int g, int b, int a=255)
    {
        setTextColor(QColor(r,g,b,a));
    }

    void setTextAlpha(int nAlpha)
    {
        if (nAlpha != m_crText.alpha())
        {
            m_crText.setAlpha(nAlpha);
            setTextColor(m_crText);
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
        font.setPointSizeF(font.pointSizeF() + fSizeOffset);
        font.setWeight(eWeight);
        font.setItalic(bItalic);
        TWidget::setFont(font);
    }

    void adjustFontSize(float fSizeOffset)
    {
        CFont font(*this);
        font.setPointSizeF(font.pointSizeF() + fSizeOffset);
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

protected:
    virtual bool event(QEvent *ev) override;

    virtual void _onPaint(CPainter&, const QRect& rc);

    bool isHLayout() const
    {
        return TWidget::width()>TWidget::height();
    }

private:
    void paintEvent(QPaintEvent *pe) override;

    void _handleTouchBegin(const CTouchEvent&);
    void _handleTouchEnd(const CTouchEvent&);
    void _handleTouchMove(const CTouchEvent&);

    void _handleTouchEvent(E_TouchEventType, const QTouchEvent&);

    void _handleMouseEvent(E_MouseEventType, const QMouseEvent&);

    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) {}

    virtual void _onMouseEnter() {}
    virtual void _onMouseLeave() {}

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) {}

    virtual bool _onGesture(QGesture&) {return false;}
};
