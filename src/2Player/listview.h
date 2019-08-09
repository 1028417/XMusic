
#pragma once

#include "widget.h"

class CListView : public CWidget<QWidget>
{
public:
    CListView(QWidget *parent=NULL) :
        CWidget(parent, {Qt::TapAndHoldGesture})
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

protected:
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

    void scroll(UINT uRow);

private:
    virtual UINT getRowCount()
    {
        return m_uRowCount;
    }

    virtual UINT getItemCount() = 0;

    void _onPaint(QPainter& painter, const QRect& rc) override;
    virtual void _onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem) = 0;

    void _handleMouseEvent(E_MouseEventType, QMouseEvent&) override;
    virtual void _handleRowClick(UINT uRowIdx) {(void)uRowIdx;}
    virtual void _handleRowDblClick(UINT uRowIdx) {(void)uRowIdx;}

    void _onTouchMove(int dy) override;
};
