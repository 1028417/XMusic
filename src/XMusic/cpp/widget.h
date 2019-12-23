
#pragma once

#include "util/util.h"

#include <QWidget>

#include <QGesture>

#include <QPainter>

#include <QGraphicsOpacityEffect>

#include <QFont>

#include <QPixmap>

#if __ios
#define __size(x) decltype(x)((x)/g_fPixelRatio)
#define __rect(x) QRect(__size(x.left()), __size(x.top()), __size(x.width()), __size(x.height()))
#else
#define __size(x) (x)
#define __rect(x) (x)
#endif

#define __ShadowColor QRGB(128,128,128)

enum class E_FontWeight
{
    FW_Light = QFont::Weight::Light,
    //FW_Normal = QFont::Weight::Normal,
    FW_SemiBold = QFont::Weight::DemiBold
};
#define __defFontWeight E_FontWeight::FW_Light

extern map<E_FontWeight, QFont> g_mapFont;

extern QColor g_crTheme;
extern QColor g_crText;

extern float g_fPixelRatio;

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
        setPointSizeF(pointSizeF() * fSizeOffset);
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

    void adjust(float fSizeOffset)
    {
        setPointSizeF(pointSizeF() * fSizeOffset);
    }

    void adjust(float fSizeOffset, E_FontWeight eWeight, bool bItalic)
    {
        adjust(fSizeOffset, eWeight);

        setItalic(bItalic);
    }

    void adjust(float fSizeOffset, E_FontWeight eWeight)
    {
        adjust(fSizeOffset);

        setWeight(eWeight);
    }
};

#define __defRenderHints (QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform)

class CPainter : public QPainter
{
public:
    CPainter(QPaintDevice* device, RenderHints eRenderHints = __defRenderHints)
        : QPainter(device)
    {
        this->setRenderHints(eRenderHints);
    }

public:
    static void zoomoutPixmap(QPixmap& pm, UINT size);

    static QColor mixColor(const QColor& crSrc, const QColor& crDst, UINT uAlpha);

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
        font.adjust(fSizeOffset, eWeight, bItalic);
        QPainter::setFont(font);
    }

    void adjustFont(float fSizeOffset, E_FontWeight eWeight)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, eWeight);
        QPainter::setFont(font);
    }

    void adjustFont(float fSizeOffset)
    {
        CFont font(*this);
        font.adjust(fSizeOffset);
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

    void drawPixmap(const QRect& rcDst, const QPixmap& pixmap, const QRect& rcSrc=QRect(), UINT xround=0, UINT yround=0);

    void drawPixmapEx(const QRect& rcDst, const QPixmap& pixmap, UINT xround=0, UINT yround=0);

    void drawRectEx(const QRect& rc, UINT xround=0, UINT yround=0);

    void drawRectEx(const QRect& rc, UINT uWidth, const QColor& cr,
                   Qt::PenStyle style=Qt::SolidLine, UINT xround=0, UINT yround=0);

    void fillRectEx(const QRect& rc, const QBrush& br, UINT xround=0, UINT yround=0);

    void fillRectEx(const QRect& rc, const QColor& cr, UINT xround=0, UINT yround=0)
    {
        QBrush brush(cr);
        fillRectEx(rc, brush, xround, yround);
    }

    void fillRectEx(const QRect& rc, const QColor& crBegin
                    , const QColor& crEnd, UINT xround=0, UINT yround=0);
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

    QPointF m_ptTouch;

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

        oe->setOpacity(fValue);

        this->setGraphicsEffect(oe);
    }

    void unsetOpacityEffect()
    {
        auto dse = dynamic_cast<QGraphicsOpacityEffect*>(this->graphicsEffect());
        if (dse)
        {
            dse->setEnabled(false);
        }
    }

    void setBlurEffect(float fStength)
    {
        auto be = dynamic_cast<QGraphicsBlurEffect*>(this->graphicsEffect());
        if (NULL == be)
        {
            be = new QGraphicsBlurEffect(this);
        }

        be->setBlurRadius(fStength);

        this->setGraphicsEffect(be);
    }

    void unsetBlurEffect()
    {
        auto dse = dynamic_cast<QGraphicsBlurEffect*>(this->graphicsEffect());
        if (dse)
        {
            dse->setEnabled(false);
        }
    }

    void setColorizeEffect(const QColor& cr, float fStength)
    {
        auto ce = dynamic_cast<QGraphicsColorizeEffect*>(this->graphicsEffect());
        if (NULL == ce)
        {
            ce = new QGraphicsColorizeEffect(this);
        }

        ce->setColor(cr);
        ce->setStrength(fStength);

        this->setGraphicsEffect(ce);
    }

    void unsetColorizeEffect()
    {
        auto dse = dynamic_cast<QGraphicsColorizeEffect*>(this->graphicsEffect());
        if (dse)
        {
            dse->setEnabled(false);
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
    void setDropShadowEffect(const QColor& cr, int dx, int dy, float fBlur=0)
    {
        auto dse = dynamic_cast<T*>(this->graphicsEffect());
        if (dse)
        {
            dse->setEnabled(true);
        }
        else
        {
            dse = new T(this);
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

    void setDropShadowEx()
    {
        int avg = (g_crTheme.red()+g_crTheme.green()+g_crTheme.blue())/3;
        int alpha = 100*pow(avg/255.0, 5);
        if (alpha > 0)
        {
            QColor crShadow(__ShadowColor);
            crShadow.setAlpha(alpha);
            this->setDropShadowEffect(crShadow, 1, 1);
        }
        else
        {
            this->unsetDropShadowEffect();
        }
    }

    virtual void update()
    {
        TWidget::update();
    }

protected:
    virtual bool event(QEvent *ev) override;

    bool isHLayout() const
    {
        return TWidget::width()>TWidget::height();
    }

private:
    void paintEvent(QPaintEvent *pe) override;

    virtual void _onPaint(CPainter&, const QRect& rc);

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
