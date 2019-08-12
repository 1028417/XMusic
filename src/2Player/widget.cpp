
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
void CWidget<TParent>::_handleMouseEvent(E_MouseEventType type, QMouseEvent& ev)
{
#if !__android
    if (E_MouseEventType::MET_Press == type)
    {
        if (!m_bTouching)
        {
            m_bMousePressed = true;
            m_ptTouch = ev.pos();
            _onTouchBegin(m_ptTouch);
        }
    }
    else if (E_MouseEventType::MET_Release == type)
    {
        if (m_bMousePressed)
        {
            m_bMousePressed = false;

            _onTouchEnd();
        }
    }
    else if (E_MouseEventType::MET_Move == type)
    {
        if (m_bMousePressed)
        {
            _handleTouchMove(ev.x(), ev.y());
        }
    }
#endif

   _onMouseEvent(type, ev);
}

template <class TParent>
void CWidget<TParent>::_handleTouchMove(int x, int y)
{
    int dx = x-m_ptTouch.x();
    int dy = y-m_ptTouch.y();
    if (dx != 0 || dy != 0)
    {
        _onTouchMove(dx, dy);
        m_ptTouch.setX(x);
        m_ptTouch.setY(y);
    }
}

template <class TParent>
bool CWidget<TParent>::event(QEvent *ev)
{
    bool bRet = TParent::event(ev);

    static bool bClicking = false;

    switch (ev->type())
    {
#if !__android
    case QEvent::Enter:
        _onMouseEnter();

        break;
    case QEvent::Leave:
        _onMouseLeave();

        break;
#endif
    case QEvent::MouseButtonPress:
    {
        auto& me = *(QMouseEvent*)ev;
        _handleMouseEvent(E_MouseEventType::MET_Press, me);

        bClicking = true;
    }

        break;
    case QEvent::MouseButtonRelease:
    {
        auto& me = *(QMouseEvent*)ev;
        _handleMouseEvent(E_MouseEventType::MET_Release, me);

        if (bClicking)
        {
            bClicking = false;

            _handleMouseEvent(E_MouseEventType::MET_Click, me);
        }
    }

        break;
    case QEvent::MouseButtonDblClick:
        _handleMouseEvent(E_MouseEventType::MET_DblClick, *(QMouseEvent*)ev);

        break;
    case QEvent::MouseMove:
        bClicking = false;

        _handleMouseEvent(E_MouseEventType::MET_Move, *(QMouseEvent*)ev);

        break;
    case QEvent::TouchEnd:
        if (m_bTouching)
        {
            m_bTouching = false;
            _onTouchEnd();
        }

        break;
    case QEvent::TouchBegin:
        if (!m_bMousePressed)
        {
            cauto& points = ((QTouchEvent*)ev)->touchPoints();
            if (!points.empty())
            {
                m_bTouching = true;
                m_ptTouch = points.at(0).pos();
                _onTouchBegin(m_ptTouch);
            }
        }

        break;
    case QEvent::TouchUpdate:
        if (m_bTouching)
        {
            cauto& points = ((QTouchEvent*)ev)->touchPoints();
            if (!points.empty())
            {
                cauto& pos = points.at(0).pos();
                _handleTouchMove(pos.x(), pos.y());
            }
        }

        break;
    case QEvent::Gesture:
    {
        //m_bTouching = false;

        QGestureEvent *ge = (QGestureEvent*)ev;
        for (auto gestureType : m_lstGestureType)
        {
            if (QGesture *gesture = ge->gesture(gestureType))
            {
                if (_onGesture(*gesture))
                {
                    bClicking = false;
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
