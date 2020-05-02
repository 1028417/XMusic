
#include "painter.h"

#define __defThemeColor QRGB(128, 192, 255)
#define __defTextColor QRGB(245, 255, 225)

QColor g_crBkg(__defThemeColor);
QColor g_crFore(__defTextColor);

map<int, QString> g_mapFontFamily;

int g_nDefFontWeight = QFont::Weight::Light;

UINT g_uDefFontSize = 0;

void CPainter::alphaPixmap(cqpm pmSrc, int alpha, QPixmap& pmDst)
{
    pmDst.fill(Qt::transparent);
    QPainter painter(&pmDst);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawPixmap(0, 0, pmSrc);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    painter.fillRect(pmDst.rect(), QColor(0, 0, 0, alpha));
    painter.end();
}

void CPainter::zoomoutPixmap(QPixmap& pm, UINT size)
{
    if (pm.width() > pm.height())
    {
        if (pm.width() > (int)size)
        {
            auto&& temp = pm.scaledToWidth(size, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
    else
    {
        if (pm.height() > (int)size)
        {
            auto&& temp = pm.scaledToHeight(size, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
}

QColor CPainter::mixColor(cqcr crSrc, cqcr crDst, UINT uAlpha)
{
    uAlpha = uAlpha * crSrc.alpha()/255 * crDst.alpha()/255;

    int r = MIN(crSrc.red(), crDst.red());
    int g = MIN(crSrc.green(), crDst.green());
    int b = MIN(crSrc.blue(), crDst.blue());

    r += (-r + MAX(crSrc.red(), crDst.red()))*uAlpha / 255;
    g += (-g + MAX(crSrc.green(), crDst.green()))*uAlpha / 255;
    b += (-b + MAX(crSrc.blue(), crDst.blue()))*uAlpha / 255;

    return QColor(r,g,b,uAlpha);
}

inline static void _genSrcRect(cqrc rcDst, QRect& rcSrc)
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

void CPainter::drawPixmap(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround, UINT yround)
{
    if (xround > 0)
    {
        QTransform transform;
        auto fScaleRate = (1.0f+rc.width())/rcSrc.width();
        transform.translate(rc.left()-rcSrc.left()*fScaleRate
                            , rc.top()-rcSrc.top()*fScaleRate);
        transform.scale(fScaleRate, fScaleRate);

        br.setTransform(transform);
    }

    this->save();

    this->setBrush(br);
    this->setPen(Qt::transparent);

    this->drawRectEx(rc,xround,yround);

    this->restore();
}

void CPainter::drawPixmapEx(cqrc rc, QBrush& br, cqrc rcSrc, UINT xround, UINT yround)
{
    QRect t_rcSrc = rcSrc;
    _genSrcRect(rc, t_rcSrc);
    this->drawPixmap(rc, br, t_rcSrc, xround, yround);
}

void CPainter::drawPixmap(cqrc rc, cqpm pm, UINT xround, UINT yround)
{
    drawPixmap(rc, pm, pm.rect(), xround, yround);
}

void CPainter::drawPixmap(cqrc rc, cqpm pm, cqrc rcSrc, UINT xround, UINT yround)
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

void CPainter::drawPixmapEx(cqrc rc, cqpm pm, UINT xround, UINT yround)
{
    QRect rcSrc = pm.rect();
    _genSrcRect(rc, rcSrc);
    this->drawPixmap(rc, pm, rcSrc, xround, yround);
}

void CPainter::drawPixmapEx(cqrc rc, cqpm pm, int& dx, int& dy)
{
    QRect rcSrc = pm.rect();
    _genSrcRect(rc, rcSrc);

    if (dx != 0 || dy != 0)
    {
        auto fScaleRate = (1.0f+rcSrc.width())/rc.width();
        if (dx != 0)
        {
            auto offset = dx*fScaleRate;
            auto lpos = rcSrc.left()+offset;
            if (lpos < 0)
            {
                dx = -rcSrc.left()/fScaleRate;

                rcSrc.setRight(rcSrc.right()-rcSrc.left());
                rcSrc.setLeft(0);
            }
            else
            {
                auto rpos = rcSrc.right()+offset;
                auto cx = pm.width();
                if (rpos >= cx)
                {
                    dx = (cx-1-rcSrc.right())/fScaleRate;

                    rcSrc.setLeft(cx-rcSrc.width());
                    rcSrc.setRight(cx-1);
                }
                else
                {
                    rcSrc.setLeft(lpos);
                    rcSrc.setRight(rpos);
                }
            }
        }

        if (dy != 0)
        {
            auto offset = dy*fScaleRate;
            auto tpos = rcSrc.top()+offset;
            if (tpos < 0)
            {
                dy = -rcSrc.top()/fScaleRate;

                rcSrc.setBottom(rcSrc.bottom()-rcSrc.top());
                rcSrc.setTop(0);
            }
            else
            {
                auto bpos = rcSrc.bottom()+offset;
                auto cy = pm.height();
                if (bpos >= cy)
                {
                    dy = (cy-1-rcSrc.bottom())/fScaleRate;

                    rcSrc.setTop(cy-rcSrc.height());
                    rcSrc.setBottom(cy-1);
                }
                else
                {
                    rcSrc.setTop(tpos);
                    rcSrc.setBottom(bpos);
                }
            }
        }
    }

    this->drawPixmap(rc, pm, rcSrc);
}

void CPainter::drawRectEx(cqrc rc, UINT xround, UINT yround)
{
    if (xround > 0)
    {
        if (0 == yround)
        {
            yround = xround;
        }
        this->drawRoundedRect(rc, xround, yround);
    }
    else
    {
        this->drawRect(rc);
    }
}

void CPainter::drawRectEx(cqrc rc, cqcr cr, UINT uWidth, Qt::PenStyle style, UINT xround, UINT yround)
{
    this->save();

    QPen pen;
    pen.setWidth(uWidth);
    pen.setColor(cr);
    pen.setStyle(style);
    this->setPen(pen);

    this->drawRectEx(rc, xround, yround);

    this->restore();
}

void CPainter::fillRectEx(cqrc rc, const QBrush& br, UINT xround, UINT yround)
{
    if (xround > 0)
    {
        if (0 == yround)
        {
            yround = xround;
        }

        this->save();

        this->setPen(Qt::transparent);
        this->setBrush(br);

        this->drawRoundedRect(rc, xround, yround);

        this->restore();
    }
    else
    {
        this->fillRect(rc, br);
    }
}

void CPainter::fillRectEx(cqrc rc, cqcr crBegin
                , cqcr crEnd, UINT xround, UINT yround)
{
    QLinearGradient gradient(rc.topLeft(), rc.topRight());
    gradient.setColorAt(0, crBegin);
    gradient.setColorAt(1, crEnd);
    QBrush br(gradient);

    fillRectEx(rc, br, xround, yround);
}

static QRect g_rcDrawTextRet;

cqrc CPainter::drawTextEx(cqrc rc, int flags, const QString& qsText, cqcr crText
                          , UINT uShadowWidth, UINT uShadowAlpha, UINT uTextAlpha)
{
    this->save();

    if (uShadowWidth > 0 && uShadowAlpha > 0)
    {
        int r = crText.red();
        int g = crText.green();
        int b = crText.blue();
        QColor crShadow(r<128?r+128:r-128, g<128?g+128:g-128, b<128?b+128:b-128, uShadowAlpha/(uShadowWidth+1));

        for (UINT uIdx=0; uIdx<=uShadowWidth; uIdx++)
        {
            if (uIdx > 0)
            {
                crShadow.setAlpha(uShadowAlpha*(uShadowWidth-uIdx+1)/(uShadowWidth+1));
            }

            this->setPen(crShadow);

            QRect rcShadow(rc.left()+uIdx, rc.top()+uIdx, rc.width(), rc.height());
            QPainter::drawText(rcShadow, flags, qsText);
        }
    }

    QColor t_crText = crText;
    t_crText.setAlpha(uTextAlpha);
    this->setPen(t_crText);
    QPainter::drawText(rc, flags, qsText, &g_rcDrawTextRet);

    this->restore();

    return g_rcDrawTextRet;
}
