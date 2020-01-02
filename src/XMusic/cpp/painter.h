
#pragma once

#include <QPainter>

#include <QWidget>

#include <QFont>

#include <QPixmap>

#include "util/util.h"

#if __ios
#define __size(x) decltype(x)((x)/g_fPixelRatio)
#define __rect(x) QRect(__size(x.left()), __size(x.top()), __size(x.width()), __size(x.height()))
#else
#define __size(x) (x)
#define __rect(x) (x)
#endif

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

#define __ColorOffset(cr1, cr2) (abs(cr1.red()-cr2.red()) + abs(cr1.green()-cr2.green()) \
    + abs(cr1.blue()-cr2.blue()))
#define __ColorOffsetAvg(cr1, cr2) (__ColorOffset(cr1, cr2) / 3)

#define __ShadowColor(alpha) QColor(128, 128, 128, 128*alpha/255)

#define __ShadowAlpha 80

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

    void drawTextEx(const QColor& crText, const QRect& rc, int flags, const QString& qsText, QRect *prcRet
                  , UINT uShadowWidth, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255);
    void drawTextEx(const QRect& rc, int flags, const QString& qsText, QRect *prcRet
                  , UINT uShadowWidth, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255);

    void drawTextEx(const QColor& crText, const QRect& rc, int flags, const QString& qsText
                  , UINT uShadowWidth, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255)
    {
        drawTextEx(crText, rc, flags, qsText, NULL, uShadowWidth, uShadowAlpha, uTextAlpha);
    }
    void drawTextEx(const QRect& rc, int flags, const QString& qsText
                  , UINT uShadowWidth, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255);
};
