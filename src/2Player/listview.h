
#pragma once

#include <QWidget>

#include "widget.cpp"

#include "view.h"

class CListView : public CWidget<QWidget>
{
public:
    CListView(QWidget *parent=NULL, const list<Qt::GestureType>& lstGestureType={}) :
        CWidget(parent, lstGestureType)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
    QFont m_font;
    QColor m_crText;

    UINT m_uRowCount = 0;

private:
    UINT m_uRowHeight = 0;

    UINT m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

public:
    void setRowCount(UINT uRowCount)
    {
        m_uRowCount = uRowCount;
    }

    void setFont(const QFont& font, const QColor& crText)
    {
        m_font = font;
        m_crText = crText;
    }

    void scroll(UINT uRow)
    {
        if (uRow < m_fScrollPos)
        {
            m_fScrollPos = uRow;
        }
        else if (uRow+1 > m_fScrollPos+m_uRowCount)
        {
            m_fScrollPos = uRow+1-m_uRowCount;
        }
    }

private:
    virtual UINT getItemCount() = 0;

    void _onPaint(QPainter& painter, const QRect& rc) override;
    virtual void _onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem) = 0;

    void mouseDoubleClickEvent(QMouseEvent *ev) override;
    virtual void _handleMouseDoubleClick(UINT uRowIdx) {(void)uRowIdx;}

    void _onTouchMove(int dy) override;
};
