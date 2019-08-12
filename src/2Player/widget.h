
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

class CPainter : public QPainter
{
public:
    CPainter(QWidget* widget)
        : QPainter(widget)
    {
    }

    void drawPixmapEx(const QRect& rcDst, const QPixmap& pixmap)
    {
        QRect rcSrc = pixmap.rect();
        float fHWRate = 1;
        int height = rcSrc.height();
        int width = rcSrc.width();
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
    QColor m_crText;

protected:
    virtual bool event(QEvent *ev) override;

    virtual void _onPaint(CPainter&, const QRect& rc);

public:
    void setOpacity(float fValue)
    {
       m_goe.setOpacity(fValue);
       this->setGraphicsEffect(&m_goe);
    }

    void setTextColor(const QColor& crText)
    {
        m_crText = crText;

        QPalette pe = TParent::palette();
        pe.setColor(QPalette::WindowText, crText);
        TParent::setPalette(pe);
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

    virtual bool _onGesture(QGesture&) {return false;}
};
