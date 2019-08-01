
#include "Widget.h"

#include <QTouchEvent>

static QPaintEvent *g_pe = NULL;

template <class TParent>
void CWidget<TParent>::_onPaint(QPainter&, const QRect&)
{
    TParent::paintEvent(g_pe);
}

template <class TParent>
void CWidget<TParent>::paintEvent(QPaintEvent *pe)
{
    g_pe = pe;

    QPainter painter(this);
    painter.setRenderHint(QPainter::HighQualityAntialiasing);
    _onPaint(painter, pe->rect());
}

template <class TParent>
bool CWidget<TParent>::event(QEvent *ev)
{
    bool bRet = TParent::event(ev);

    switch (ev->type())
    {
#ifndef __ANDROID__
    case QEvent::Enter:
        _onMouseEnter();

        break;
    case QEvent::Leave:
        _onMouseLeave();

        break;
    case QEvent::MouseButtonRelease:
        if (m_bMousePressed)
        {
            m_bMousePressed = false;

            _onTouchEnd();
        }

        break;
    case QEvent::MouseButtonPress:
        if (!m_bTouching)
        {
            m_bMousePressed = true;
            m_ptTouch = ((QMouseEvent*)ev)->pos();
            _onTouchBegin(m_ptTouch);
        }

        break;
    case QEvent::MouseMove:
        if (m_bMousePressed)
        {
            cauto& pos = ((QMouseEvent*)ev)->pos();
            int dy = pos.y()-m_ptTouch.y();
            if (dy != 0)
            {
                _onTouchMove(dy);
                m_ptTouch = pos;
            }
        }

        break;
#endif
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
                int dy = pos.y()-m_ptTouch.y();
                if (dy != 0)
                {
                    _onTouchMove(dy);
                    m_ptTouch = pos;
                }
            }
        }

        break;
    case QEvent::Gesture:
    {
        m_bTouching = false;

        QGestureEvent *ge = (QGestureEvent*)ev;
        for (auto gestureType : m_lstGestureType)
        {
            if (QGesture *gesture = ge->gesture(gestureType))
            {
                _onGesture(*gesture);
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
