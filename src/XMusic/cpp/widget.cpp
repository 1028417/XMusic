
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
    case QEvent::MouseMove:
        _handleMouseEvent(E_MouseEventType::MET_Move, *(QMouseEvent*)ev);

        break;
    case QEvent::MouseButtonPress:
    {
        cauto me = *(QMouseEvent*)ev;
        if (me.button() == Qt::MouseButton::LeftButton)
        {
            _handleMouseEvent(E_MouseEventType::MET_Press, me);
        }
    }

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

    m_xTouch = te.x();
    m_yTouch = te.y();

    _onTouchEvent(E_TouchEventType::TET_TouchBegin, te);
}

template <class TParent>
void CWidget<TParent>::_handleTouchEnd(CTouchEvent te)
{
    te.setDt(te.timestamp() - m_teBegin.timestamp());
    te.setDx(te.x() - m_teBegin.x());
    te.setDy(te.y() - m_teBegin.y());

    _onTouchEvent(E_TouchEventType::TET_TouchEnd, te);
}

template <class TParent>
void CWidget<TParent>::_handleTouchMove(CTouchEvent te)
{
    int dx = te.x()-m_xTouch;
    int dy = te.y()-m_yTouch;
    if (dx != 0 || dy != 0)
    {
        if (abs(dx)>2 || abs(dy)>2)
        {
            m_bClicking = false;
        }

        te.setDx(dx);
        te.setDy(dy);

        _onTouchEvent(E_TouchEventType::TET_TouchMove, te);

        m_xTouch = te.x();
        m_yTouch = te.y();
    }
}
