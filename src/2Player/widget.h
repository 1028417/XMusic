
#pragma once

#include <QPainter>
#include <QGesture>

#include "util.h"

template <class TParent=QWidget>
class CWidget : public TParent
{
public:
    CWidget(QWidget *parent, const list<Qt::GestureType>& lstGestureType={
            Qt::TapAndHoldGesture, Qt::PanGesture, Qt::PinchGesture, Qt::SwipeGesture}) :
        TParent(parent),
        m_lstGestureType(lstGestureType)
    {
        for (auto gestureType : m_lstGestureType)
        {
            this->grabGesture(gestureType);
        }
    }

private:
    list<Qt::GestureType> m_lstGestureType;

    bool m_bTouching = false;
    QPointF m_ptTouch;

    bool m_bMousePressed = false;

protected:
    virtual bool event(QEvent *ev) override;

private:
    virtual void _onPaint(QPainter&, const QRect&) {}

    virtual void _onMouseEnter() {}
    virtual void _onMouseLeave() {}

    virtual void _onTouchBegin(const QPointF&) {}
    virtual void _onTouchMove(int dy) {(void)dy;}
    virtual void _onTouchEnd() {}

    virtual void _onGesture(QGesture&) {}
};
