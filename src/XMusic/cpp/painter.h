
#pragma once

#include <QPainter>

#include <QWidget>

#include <QFont>

#include <QPixmap>

#include "util/util.h"

extern QColor g_crTheme;
extern QColor g_crText;

#define __ColorOffset(cr1, cr2) (abs(cr1.red()-cr2.red()) + abs(cr1.green()-cr2.green()) \
    + abs(cr1.blue()-cr2.blue()))
#define __ColorOffsetAvg(cr1, cr2) (__ColorOffset(cr1, cr2) / 3)

#define __OppAlpha(f) (255-(255*pow(__ColorOffsetAvg(g_crText, g_crTheme)/255.0, f)))

#define __ShadowColor(alpha) QColor(128, 128, 128, 128*alpha/255)

#define __ShadowAlpha 80

using cqcr = const QColor&;
using cqrc = const QRect&;

extern map<int, QString> g_mapFontFamily;

extern int g_nDefFontWeight;

extern UINT g_uDefFontSize;

class CFont : public QFont
{
private:
    inline QString _getFamily(int nWeight)
    {
        for (auto itr = g_mapFontFamily.rbegin(); itr != g_mapFontFamily.rend(); ++itr)
        {
            if (itr->first <= nWeight)
            {
                return itr->second;
            }
        }

        return "";
    }

    inline void _setFamily()
    {
        QFont::setFamily(_getFamily(QFont::weight()));
    }

public:
    CFont(const QFont& font)
        : QFont(font)
    {
        _setFamily();
    }

    CFont(const QWidget& widget)
        : QFont(widget.font())
    {
        _setFamily();
    }

    CFont(const QPainter& painter)
        : QFont(painter.font())
    {
        _setFamily();
    }

    CFont(float fSizeOffset=1.0f, int nWeight = g_nDefFontWeight, bool bItalic=false)
        : QFont(_getFamily(nWeight), g_uDefFontSize * fSizeOffset, nWeight)
    {
        setItalic(bItalic);

        _setFamily();
    }

    void setWeight(int nWeight)
    {
        QFont::setWeight(nWeight);
        _setFamily();
    }

    void setBold(bool bBold)
    {
        QFont::setBold(bBold);
        _setFamily();
    }

    void adjust(float fSizeOffset)
    {
        setPointSizeF(pointSizeF() * fSizeOffset);
    }

    void adjust(float fSizeOffset, int nWeight, bool bItalic)
    {
        adjust(fSizeOffset, nWeight);

        setItalic(bItalic);
    }

    void adjust(float fSizeOffset, int nWeight)
    {
        adjust(fSizeOffset);

        setWeight(nWeight);
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
    static UINT oppTextAlpha(UINT uMinAlpha, float fPow = 0.1f)
    {
        UINT uOppAlpha = __OppAlpha(fPow);
        return MAX(uMinAlpha, uOppAlpha);
    }

    static void alphaPixmap(const QPixmap& pmSrc, int alpha, QPixmap& pmDst);
    static QPixmap alphaPixmap(const QPixmap& pmSrc, int alpha)
    {
        QPixmap pmDst(pmSrc.size());
        alphaPixmap(pmSrc, alpha, pmDst);
        return pmDst;
    }

    static void zoomoutPixmap(QPixmap& pm, UINT size);

    static QColor mixColor(cqcr crSrc, cqcr crDst, UINT uAlpha);

    void setPenColor(int r, int g, int b, int a=255)
    {
        setPen(QColor(r,g,b,a));
    }

    void adjustFont(float fSizeOffset, int nWeight, bool bItalic)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, nWeight, bItalic);
        QPainter::setFont(font);
    }

    void adjustFont(float fSizeOffset, int nWeight)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, nWeight);
        QPainter::setFont(font);
    }

    void adjustFont(float fSizeOffset)
    {
        CFont font(*this);
        font.adjust(fSizeOffset);
        QPainter::setFont(font);
    }

    void adjustFontWeight(int nWeight)
    {
        CFont font(*this);
        font.setWeight(nWeight);
        QPainter::setFont(font);
    }

    void adjustFontItalic(bool bItalic)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        QPainter::setFont(font);
    }

    void drawPixmap(cqrc rcDst, const QPixmap& pixmap, UINT xround=0, UINT yround=0);
    void drawPixmap(cqrc rcDst, const QPixmap& pixmap, cqrc rcSrc, UINT xround=0, UINT yround=0);

    void drawPixmapEx(cqrc rcDst, const QPixmap& pixmap, UINT xround=0, UINT yround=0);

    void drawRectEx(cqrc rc, UINT xround=0, UINT yround=0);

    void drawRectEx(cqrc rc, cqcr cr, UINT uWidth=1,
                   Qt::PenStyle style=Qt::SolidLine, UINT xround=0, UINT yround=0);

    void fillRectEx(cqrc rc, const QBrush& br, UINT xround=0, UINT yround=0);

    void fillRectEx(cqrc rc, cqcr cr, UINT xround=0, UINT yround=0)
    {
        QBrush brush(cr);
        fillRectEx(rc, brush, xround, yround);
    }

    void fillRectEx(cqrc rc, cqcr crBegin
                    , cqcr crEnd, UINT xround=0, UINT yround=0);

    void drawTextEx(cqrc rc, int flags, const QString& qsText, QRect *prcRet, cqcr crText
                    , UINT uShadowWidth=1, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255);
    void drawTextEx(cqrc rc, int flags, const QString& qsText, QRect *prcRet
                            , UINT uShadowWidth=1, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255)
    {
        drawTextEx(rc, flags, qsText, prcRet, g_crText, uShadowWidth, uShadowAlpha, uTextAlpha);
    }

    void drawTextEx(cqrc rc, int flags, const QString& qsText, cqcr crText
                    , UINT uShadowWidth=1, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255)
    {
        drawTextEx(rc, flags, qsText, NULL, crText, uShadowWidth, uShadowAlpha, uTextAlpha);
    }
    void drawTextEx(cqrc rc, int flags, const QString& qsText
                  , UINT uShadowWidth=1, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255)
    {
        drawTextEx(rc, flags, qsText, NULL, g_crText, uShadowWidth, uShadowAlpha, uTextAlpha);
    }
};
