
#include "widget.h"

#include <QTouchEvent>

static QPaintEvent *g_pe = NULL;

template <class TParent>
void CWidget<TParent>::paintEvent(QPaintEvent *pe)
{
    g_pe = pe;

    CPainter painter(this, m_eRenderHints);
    _onPaint(painter, pe->rect());
}

template <class TParent>
void CWidget<TParent>::_onPaint(CPainter&, const QRect&)
{
    TParent::paintEvent(g_pe);
}

template <class TParent>
bool CWidget<TParent>::event(QEvent *ev)
{
    bool bRet = TParent::event(ev);

    switch (ev->type())
    {
#if __windows || __mac
    case QEvent::Enter:
        _onMouseEnter();

        break;
    case QEvent::Leave:
        _onMouseLeave();

        break;
#endif
    case QEvent::MouseButtonPress:
    {
        cauto me = *(QMouseEvent*)ev;
        if (me.button() == Qt::MouseButton::LeftButton)
        {
            _handleMouseEvent(E_MouseEventType::MET_Press, me);
        }
    }

    break;
    case QEvent::MouseMove:
        _handleMouseEvent(E_MouseEventType::MET_Move, *(QMouseEvent*)ev);

        break;
    case QEvent::MouseButtonRelease:
    {
        cauto me = *(QMouseEvent*)ev;
        if (me.button() == Qt::MouseButton::LeftButton)
        {
            _handleMouseEvent(E_MouseEventType::MET_Release, me);
        }
    }

    break;
    case QEvent::MouseButtonDblClick:
        _handleMouseEvent(E_MouseEventType::MET_DblClick, *(QMouseEvent*)ev);

        break;
    case QEvent::TouchBegin:
        _handleTouchEvent(E_TouchEventType::TET_TouchBegin, *(QTouchEvent*)ev);

        break;
    case QEvent::TouchUpdate:
        _handleTouchEvent(E_TouchEventType::TET_TouchMove, *(QTouchEvent*)ev);

        break;
    case QEvent::TouchEnd:
        _handleTouchEvent(E_TouchEventType::TET_TouchEnd, *(QTouchEvent*)ev);

        break;
    case QEvent::Gesture:
    {
        QGestureEvent *ge = (QGestureEvent*)ev;
        for (auto gestureType : m_setGestureType)
        {
            if (QGesture *gesture = ge->gesture(gestureType))
            {
                if (_onGesture(*gesture))
                {
                    if (Qt::TapAndHoldGesture == gestureType)
                    {
                        m_bClicking = false;
                    }
                }
                break;
            }
        }
    }

    break;
    default:
        break;
    }

    return bRet;
}

template <class TParent>
void CWidget<TParent>::_handleMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
    if (E_MouseEventType::MET_Press == type)
    {
        m_bClicking = true;
    }

    _onMouseEvent(type, me);

    if (E_MouseEventType::MET_Press == type)
    {
        if (!m_bTouching)
        {
            m_bMousePressed = true;

            _handleTouchBegin(me);
        }
    }
    else if (E_MouseEventType::MET_Release == type)
    {
        if (m_bMousePressed)
        {
            m_bMousePressed = false;

            _handleTouchEnd(me);
        }

        if (m_bClicking)
        {
            //CApp::async([me](){
                //if (!m_bClicking)
                //{
                //    return;
                //}

                m_bClicking = false;

                _onMouseEvent(E_MouseEventType::MET_Click, me);
            //});
        }
    }
    else if (E_MouseEventType::MET_Move == type)
    {
        if (m_bMousePressed)
        {
            _handleTouchMove(me);
        }
    }
}

template <class TParent>
void CWidget<TParent>::_handleTouchEvent(E_TouchEventType type, const QTouchEvent& te)
{
    if (E_TouchEventType::TET_TouchBegin == type)
    {
        if (!m_bMousePressed)
        {
            m_bTouching = true;

            _handleTouchBegin(te);
        }
    }
    else if (E_TouchEventType::TET_TouchEnd == type)
    {
        if (m_bTouching)
        {
            m_bTouching = false;

            _handleTouchEnd(te);
        }
    }
    else if (E_TouchEventType::TET_TouchMove == type)
    {
        if (m_bTouching)
        {
            _handleTouchMove(te);
        }
    }
}

template <class TParent>
void CWidget<TParent>::_handleTouchBegin(const CTouchEvent& te)
{
    m_teBegin = te;
    m_ptTouch = te.pos();

    _onTouchEvent(E_TouchEventType::TET_TouchBegin, te);
}

template <class TParent>
void CWidget<TParent>::_handleTouchEnd(const CTouchEvent& te)
{
    CTouchEvent tee(te);
    tee.setDt(te.timestamp() - m_teBegin.timestamp());
    tee.setDx(te.x() - m_teBegin.x());
    tee.setDy(te.y() - m_teBegin.y());

    _onTouchEvent(E_TouchEventType::TET_TouchEnd, tee);
}

template <class TParent>
void CWidget<TParent>::_handleTouchMove(const CTouchEvent& te)
{
    CTouchEvent tme(te);
    int dx = tme.x()-m_ptTouch.x();
    int dy = tme.y()-m_ptTouch.y();
    if (dx != 0 || dy != 0)
    {
        if (abs(dx)>2 || abs(dy)>2)
        {
            m_bClicking = false;
        }

        tme.setDx(dx);
        tme.setDy(dy);

        _onTouchEvent(E_TouchEventType::TET_TouchMove, tme);

        m_ptTouch = tme.pos();
    }
}


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
