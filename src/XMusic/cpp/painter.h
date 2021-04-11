
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

class CBrush
{
public:
    CBrush() = default;

    CBrush(cqpm pm)
    {
        setTexture(pm);
    }

    CBrush(cqstr qsFile)
    {
        setTexture(qsFile);
    }

private:
    QBrush m_br;
    int m_cx = 0;
    int m_cy = 0;

public:
    operator bool() const
    {
        return m_cx > 0;
    }
    bool operator !() const
    {
        return 0 == m_cx;
    }

    cqbr brush() const
    {
        return m_br;
    }
    QBrush& brush()
    {
        return m_br;
    }

    operator cqbr() const
    {
        return m_br;
    }
    operator QBrush&()
    {
        return m_br;
    }

    QBrush* operator->()
    {
        return &m_br;
    }
    const QBrush* operator->() const
    {
        return &m_br;
    }

    int width() const
    {
        return m_cx;
    }
    int height() const
    {
        return m_cy;
    }

    void setTexture(cqpm pm)
    {
        m_cx = pm.width();
        if (0 == m_cx)
        {
            clear();
            return;
        }
        m_cy = pm.height();

        m_br.setTexture(pm);
    }

    void setTexture(cqstr qsFile)
    {
        setTexture(QPixmap(qsFile));
    }

    void clear()
    {
        QBrush br;
        m_br.swap(br);
        m_cx = m_cy = 0;
    }
};

class CFont : public QFont
{
public:
    static int g_nDefFontWeight;

private:
    static UINT m_uDefFontSize;

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
        : QFont(_getFamily(nWeight), m_uDefFontSize * fSizeOffset, nWeight)
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

class CPainterRestoreGuard
{
protected:
    QPainter& m_painter;

    bool m_bRestored = false;

public:
    CPainterRestoreGuard(QPainter& painter)
        : m_painter(painter)
    {
        painter.save();
    }

    ~CPainterRestoreGuard()
    {
        restore();
    }

    void restore()
    {
        if (!m_bRestored)
        {
            m_bRestored = true;
            m_painter.restore();
        }
    }
};

class CPainterClipGuard : public CPainterRestoreGuard //据说裁剪慢
{
public:
    CPainterClipGuard(QPainter& painter, cqrc rc, UINT xround=0, UINT yround=0)
        : CPainterRestoreGuard(painter)
    {
        if (xround)
        {
            if (0 == yround)
            {
                yround = xround;
            }
        }

        QPainterPath path;
        path.addRoundedRect(rc, xround, yround);
        painter.setClipPath(path);
        painter.setClipping(true);
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

private:
    void _drawRectEx(cqrc rc, UINT xround=0, UINT yround=0);

    void _genSrcRect(cqrc rcDst, QRect& rcSrc)
    {
        auto cxSrc = rcSrc.width();
        auto cySrc = rcSrc.height();
        if (cxSrc > 0 && cySrc > 0)
        {
            float fHWRate = (float)rcDst.height()/rcDst.width();
            if ((float)cySrc/cxSrc > fHWRate)
            {
                int dy = (cySrc - cxSrc*fHWRate)/2;
                rcSrc.setTop(rcSrc.top()+dy);
                rcSrc.setBottom(rcSrc.bottom()-dy);
            }
            else
            {
                int dx = (cxSrc - cySrc/fHWRate)/2;
                rcSrc.setLeft(rcSrc.left()+dx);
                rcSrc.setRight(rcSrc.right()-dx);
            }
        }
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

    void drawImg(cqrc rc, cqpm pm, cqrc rcSrc, UINT xround=0, UINT yround=0);

    void drawImg(cqrc rc, const QImage& img, cqrc rcSrc, UINT xround=0, UINT yround=0);

    template <class T>
    inline void drawImg(cqrc rc, const T& img, UINT xround=0, UINT yround=0)
    {
        drawImg(rc, img, img.rect(), xround, yround);
    }

    void drawImg(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround=0, UINT yround=0);

    void drawImgEx(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround=0, UINT yround=0);

    template <class T>
    void drawImgEx(cqrc rc, const T& t, UINT xround=0, UINT yround=0)
    {
        QRect rcSrc = t.rect();
        _genSrcRect(rc, rcSrc);
        this->drawImg(rc, t, rcSrc, xround, yround);
    }

    void drawImgEx(cqrc rc, cqpm pm, int& dx, int& dy, UINT szAdjust=1);

    void drawRectEx(cqrc rc, cqcr cr, UINT xround=0, UINT yround=0)
    {
        drawRectEx(rc, cr, Qt::SolidLine, 1, xround, yround);
    }

    void drawRectEx(cqrc rc, cqcr cr, Qt::PenStyle style, UINT uWidth = 1, UINT xround=0, UINT yround=0);

    void fillRectEx(cqrc rc, cqbr br, UINT xround=0, UINT yround=0);

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
