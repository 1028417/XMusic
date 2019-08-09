
#pragma once

#include <QWidget>

#include <QPainter>

#include <QGesture>

#include <QGraphicsOpacityEffect>

#include "util.h"

enum class E_MouseEventType
{
    MET_Move,
    MET_Press,
    MET_Release,
    MET_Click,
    MET_DblClick
};

template <class TParent=QWidget>
class CWidget : public TParent
{
public:
    CWidget(QWidget *parent, const list<Qt::GestureType>& lstGestureType={}) :
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

    QPointF m_ptTouch;
    bool m_bTouching = false;

    bool m_bMousePressed = false;

    QGraphicsOpacityEffect m_goe;

protected:
    virtual bool event(QEvent *ev) override;

    virtual void _onPaint(QPainter&, const QRect& rc);

    void setOpacity(float fValue)
    {
       m_goe.setOpacity(fValue);
       this->setGraphicsEffect(&m_goe);
    }

private:
    void paintEvent(QPaintEvent *pe);

    void handleMouseEvent(E_MouseEventType, QMouseEvent&);
    virtual void _handleMouseEvent(E_MouseEventType, QMouseEvent&) {}

private:
    virtual void _onMouseEnter() {}
    virtual void _onMouseLeave() {}

    virtual void _onTouchBegin(const QPointF&) {}
    virtual void _onTouchMove(int dy) {(void)dy;}
    virtual void _onTouchEnd() {}

    virtual void _onGesture(QGesture&) {}
};
