
#include "painter.h"

void CPainter::zoomoutPixmap(QPixmap& pm, UINT size)
{
    if (pm.width() < pm.height())
    {
        if (pm.width() > (int)size)
        {
            auto&& temp = pm.scaledToWidth(size, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
    else
    {
        if (pm.width() > (int)size)
        {
            auto&& temp = pm.scaledToHeight(size, Qt::SmoothTransformation);
            pm.swap(temp);
        }
    }
}

QColor CPainter::mixColor(const QColor& crSrc, const QColor& crDst, UINT uAlpha)
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

void CPainter::drawPixmap(const QRect& rcDst, const QPixmap& pixmap
                            , const QRect& rcSrc, UINT xround, UINT yround)
{
    if (xround > 0)
    {
        if (0 == yround)
        {
            yround = xround;
        }

        QBrush brush(pixmap.copy(rcSrc));
        QTransform transform;
        transform.translate(rcDst.left(), rcDst.top());
        auto scaleRate = (double)rcDst.width()/rcSrc.width();
        transform.scale(scaleRate, scaleRate);
        brush.setTransform(transform);

        //auto prevBrush = this->brush();
        //auto prevPen = this->pen();
        this->save();

        this->setBrush(brush);
        this->setPen(Qt::transparent);

        this->drawRoundedRect(rcDst,xround,yround);

        this->restore();
        //this->setPen(prevPen);
        //this->setBrush(prevBrush);
    }
    else
    {
        QPainter::drawPixmap(rcDst, pixmap, rcSrc);
    }
}

void CPainter::drawPixmapEx(const QRect& rcDst, const QPixmap& pixmap, UINT xround, UINT yround)
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

    this->drawPixmap(rcDst, pixmap, rcSrc, xround, yround);
}

void CPainter::drawRectEx(const QRect& rc, UINT xround, UINT yround)
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

void CPainter::drawRectEx(const QRect& rc, UINT uWidth, const QColor& cr,
               Qt::PenStyle style, UINT xround, UINT yround)
{
    //auto prevPen = this->pen();
    this->save();

    QPen pen;
    pen.setWidth(uWidth);
    pen.setColor(cr);
    pen.setStyle(style);
    this->setPen(pen);

    this->drawRectEx(rc, xround, yround);

    this->restore();
    //this->setPen(prevPen);
}

void CPainter::fillRectEx(const QRect& rc, const QBrush& br, UINT xround, UINT yround)
{
    if (xround > 0)
    {
        if (0 == yround)
        {
            yround = xround;
        }

        //auto prevPen = this->pen();
        //auto prevBrush = this->brush();
        this->save();

        this->setPen(Qt::transparent);
        this->setBrush(br);

        this->drawRoundedRect(rc, xround, yround);

        this->restore();
        //this->setBrush(prevBrush);
        //this->setPen(prevPen);
    }
    else
    {
        this->fillRect(rc, br);
    }
}

void CPainter::fillRectEx(const QRect& rc, const QColor& crBegin
                , const QColor& crEnd, UINT xround, UINT yround)
{
    QLinearGradient gradient(rc.topLeft(), rc.topRight());
    gradient.setColorAt(0, crBegin);
    gradient.setColorAt(1, crEnd);
    QBrush brush(gradient);

    fillRectEx(rc, brush, xround, yround);
}

void CPainter::drawTextEx(const QColor& crText, const QRect& rc, int flags, const QString& qsText, QRect *prcRet
                        , UINT uShadowWidth, UINT uShadowAlpha, UINT uTextAlpha)
{
    this->save();

    if (uShadowWidth > 0 && uShadowAlpha > 0)
    {
        int r = crText.red();
        int g = crText.green();
        int b = crText.blue();
        QColor crShadow(r<128?r+128:r-128, g<128?g+128:g-128, b<128?b+128:b-128, uShadowAlpha);

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
    QPainter::drawText(rc, flags, qsText, prcRet);

    this->restore();
}

void CPainter::drawTextEx(const QRect& rc, int flags, const QString& qsText, QRect *prcRet
                        , UINT uShadowWidth, UINT uShadowAlpha, UINT uTextAlpha)
{
    drawTextEx(g_crText, rc, flags, qsText, prcRet, uShadowWidth, uShadowAlpha, uTextAlpha);
}

void CPainter::drawTextEx(const QRect& rc, int flags, const QString& qsText
              , UINT uShadowWidth, UINT uShadowAlpha, UINT uTextAlpha)
{
    drawTextEx(g_crText, rc, flags, qsText, NULL, uShadowWidth, uShadowAlpha, uTextAlpha);
}
