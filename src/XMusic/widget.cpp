
#include "Widget.h"

#include <QTouchEvent>

static QPaintEvent *g_pe = NULL;

template <class TParent>
void CWidget<TParent>::paintEvent(QPaintEvent *pe)
{
    g_pe = pe;

    CPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
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
#if _winqt
    case QEvent::Enter:
        _onMouseEnter();

        break;
    case QEvent::Leave:
        _onMouseLeave();

        break;
#endif
    case QEvent::MouseButtonPress:
        _handleMouseEvent(E_MouseEventType::MET_Press, *(QMouseEvent*)ev);

        break;
    case QEvent::MouseMove:
        _handleMouseEvent(E_MouseEventType::MET_Move, *(QMouseEvent*)ev);

        break;
    case QEvent::MouseButtonRelease:
        _handleMouseEvent(E_MouseEventType::MET_Release, *(QMouseEvent*)ev);

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
        for (auto gestureType : m_lstGestureType)
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
            m_bClicking = false;

            _onMouseEvent(E_MouseEventType::MET_Click, me);
        }
    }
    else if (E_MouseEventType::MET_Move == type)
    {
        m_bClicking = false;

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
        tme.setDx(dx);
        tme.setDy(dy);

        _onTouchEvent(E_TouchEventType::TET_TouchMove, tme);

        m_ptTouch = tme.pos();
    }
}


void CPainter::zoomoutPixmap(QPixmap& pm, size_t size)
{
    if (pm.width() < pm.height())
    {
        if (pm.width() > (int)size)
        {
            auto&& temp = pm.scaledToWidth(size);
            pm.swap(temp);
        }
    }
    else
    {
        if (pm.width() > (int)size)
        {
            auto&& temp = pm.scaledToHeight(size);
            pm.swap(temp);
        }
    }
}

QColor CPainter::mixColor(const QColor& crSrc, const QColor& crDst, UINT uAlpha)
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

void CPainter::drawPixmapEx(const QRect& rcDst, const QPixmap& pixmap)
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
