
#pragma once

#include <QPainter>

#include <QWidget>

#include <QFont>

#include <QPixmap>

#include "util/util.h"

using cqcr = const QColor&;
using cqrc = const QRect&;
using cqpm = const QPixmap&;
using cqbr = const QBrush&;

#define __ShadowColor(alpha) QColor(128, 128, 128, 128*alpha/255)
#define __ShadowAlpha 80

#define __ColorOffset(cr1, cr2) (abs(cr1.red()-cr2.red()) + abs(cr1.green()-cr2.green()) \
    + abs(cr1.blue()-cr2.blue()))
#define __ColorOffsetAvg(cr1, cr2) (__ColorOffset(cr1, cr2) / 3)

#define __OppAlpha(f) (255-(255*pow(__ColorOffsetAvg(g_crFore, g_crBkg)/255.0, f)))

extern QColor g_crBkg;
extern QColor g_crFore;

extern int g_nDefFontWeight;

class CFont : public QFont
{
private:
    static UINT g_uDefFontSize;

    static list<pair<int, QString>> m_lstFontFamily;
    inline static cqstr _getFamily(int nWeight)
    {
        for (cauto pr : m_lstFontFamily)
        {
            if (pr.first >= nWeight)
            {
                return pr.second;
            }
        }

        return m_lstFontFamily.back().second;
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

    CFont(float fSizeOffset=1.0f, int nWeight = g_nDefFontWeight, bool bItalic=false, bool bUnderline=false)
        : QFont(_getFamily(nWeight), g_uDefFontSize * fSizeOffset, nWeight)
    {
        //_setFamily();

        setItalic(bItalic);

        setUnderline(bUnderline);
    }

public:
    static void init(const QFont& font);

    inline void setWeight(int nWeight)
    {
        QFont::setWeight(nWeight);
        _setFamily();
    }

    inline void setBold(bool bBold)
    {
        QFont::setBold(bBold);
        _setFamily();
    }

    inline void adjust(float fSizeOffset)
    {
        setPointSizeF(pointSizeF() * fSizeOffset);
    }

    inline void adjust(float fSizeOffset, int nWeight)
    {
        adjust(fSizeOffset);

        setWeight(nWeight);
    }

    inline void adjust(float fSizeOffset, int nWeight, bool bItalic)
    {
        adjust(fSizeOffset, nWeight);

        setItalic(bItalic);
    }

    void adjust(float fSizeOffset, int nWeight, bool bItalic, bool bUnderline)
    {
        adjust(fSizeOffset, nWeight, bItalic);

        setUnderline(bUnderline);
    }
};

#define __defRenderHints (QPainter::Antialiasing | QPainter::TextAntialiasing \
    | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing)

class CPainter : public QPainter
{
public:
    CPainter(QPaintDevice* device, RenderHints eRenderHints = __defRenderHints)
        : QPainter(device)
    {
        this->setRenderHints(eRenderHints);
    }

    CPainter(QPaintDevice& device)
        : QPainter(&device)
    {
    }

public:
    static UINT oppTextAlpha(UINT uMinAlpha, float fPow = 0.1f)
    {
        UINT uOppAlpha = __OppAlpha(fPow);
        return MAX(uMinAlpha, uOppAlpha);
    }

    static void alphaPixmap(cqpm pmSrc, int alpha, QPixmap& pmDst);
    static QPixmap alphaPixmap(cqpm pmSrc, int alpha)
    {
        QPixmap pmDst(pmSrc.size());
        alphaPixmap(pmSrc, alpha, pmDst);
        return pmDst;
    }

    static QColor mixColor(cqcr crSrc, cqcr crDst, UINT uAlpha);

    void setPenColor(int r, int g, int b, int a=255)
    {
        setPen(QColor(r,g,b,a));
    }

    void adjustFont(float fSizeOffset, int nWeight, bool bItalic, bool bUnderline)
    {
        CFont font(*this);
        font.adjust(fSizeOffset, nWeight, bItalic, bUnderline);
        QPainter::setFont(font);
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

    void adjustFont(int nWeight)
    {
        CFont font(*this);
        font.setWeight(nWeight);
        QPainter::setFont(font);
    }

    void adjustFont(bool bItalic)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        QPainter::setFont(font);
    }

    void adjustFont(bool bItalic, bool bUnderline)
    {
        CFont font(*this);
        font.setItalic(bItalic);
        font.setUnderline(bUnderline);
        QPainter::setFont(font);
    }

    void drawPixmap(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround=0, UINT yround=0);
    void drawPixmapEx(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround=0, UINT yround=0);

    void drawPixmap(cqrc rc, cqpm pm, cqrc rcSrc, UINT xround=0, UINT yround=0)
    {
        if (xround > 0)
        {
            QBrush br(pm);
            drawPixmap(rc, br, rcSrc, xround, yround);
        }
        else
        {
            QPainter::drawPixmap(rc, pm, rcSrc);
        }
    }
    void drawPixmap(cqrc rc, cqpm pm, UINT xround=0, UINT yround=0)
    {
        drawPixmap(rc, pm, pm.rect(), xround, yround);
    }

    void drawPixmapEx(cqrc rc, cqpm pm, UINT xround=0, UINT yround=0);

    void drawPixmapEx(cqrc rc, cqpm pm, int& dx, int& dy, UINT szAdjust=1);

    void drawRectEx(cqrc rc, UINT xround=0, UINT yround=0);

    void drawRectEx(cqrc rc, cqcr cr, UINT uWidth=1,
                   Qt::PenStyle style=Qt::SolidLine, UINT xround=0, UINT yround=0);

    void fillRectEx(cqrc rc, const QBrush& br, UINT xround=0, UINT yround=0);

    void fillRectEx(cqrc rc, cqcr cr, UINT xround=0, UINT yround=0)
    {
        QBrush br(cr);
        fillRectEx(rc, br, xround, yround);
    }

    void fillRectEx(cqrc rc, cqcr crBegin, cqcr crEnd, UINT xround=0, UINT yround=0);

    cqrc drawTextEx(cqrc rc, int flags, cqstr qsText, cqcr crText
                    , UINT uShadowWidth=1, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255);
    cqrc drawTextEx(cqrc rc, int flags, cqstr qsText
                    , UINT uShadowWidth=1, UINT uShadowAlpha=__ShadowAlpha, UINT uTextAlpha=255)
    {
        return drawTextEx(rc, flags, qsText, g_crFore, uShadowWidth, uShadowAlpha, uTextAlpha);
    }
};

class CPainterFontGuard
{
private:
    QPainter& m_painter;

public:
    ~CPainterFontGuard()
    {
        m_painter.restore();
    }

    CPainterFontGuard(CPainter& painter, float fSizeOffset, int nWeight, bool bItalic, bool bUnderline)
        : m_painter(painter)
    {
        painter.save();

        painter.adjustFont(fSizeOffset, nWeight, bItalic, bUnderline);
    }

    CPainterFontGuard(CPainter& painter, float fSizeOffset, int nWeight, bool bItalic)
        : m_painter(painter)
    {
        painter.save();

        painter.adjustFont(fSizeOffset, nWeight, bItalic);
    }

    CPainterFontGuard(CPainter& painter, float fSizeOffset, int nWeight)
        : m_painter(painter)
    {
        painter.save();

        painter.adjustFont(fSizeOffset, nWeight);
    }

    CPainterFontGuard(CPainter& painter, float fSizeOffset)
        : m_painter(painter)
    {
        painter.save();

        painter.adjustFont(fSizeOffset);
    }

    CPainterFontGuard(CPainter& painter, int nWeight)
        : m_painter(painter)
    {
        painter.save();

        painter.adjustFont(nWeight);
    }

    CPainterFontGuard(CPainter& painter, bool bItalic)
        : m_painter(painter)
    {
        painter.save();

        painter.adjustFont(bItalic);
    }

    CPainterFontGuard(CPainter& painter, bool bItalic, bool bUnderline)
        : m_painter(painter)
    {
        painter.save();

        painter.adjustFont(bItalic, bUnderline);
    }
};
