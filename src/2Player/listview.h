
#pragma once

#include "widget.h"

struct tagListViewItem
{
    UINT uItem = 0;
    UINT uRow = 0;

    bool bSelect = false;
    bool bFlash = false;
};

class CListView : public CWidget<QWidget>
{
public:
    CListView(QWidget *parent=NULL) :
        CWidget(parent
#if __android
                , {Qt::TapAndHoldGesture}
#endif
    )
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    UINT m_uRowCount = 0;

    UINT m_uRowHeight = 0;

    UINT m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    int m_nSelectItem = -1;
    int m_nFlashItem = -1;

public:
    void setRowCount(UINT uRowCount)
    {
        m_uRowCount = uRowCount;
    }

    void selectItem(UINT uItem);

    void dselectItem();

    void showItem(UINT uItem, bool bToTop=false);

    void flashItem(UINT uItem, UINT uMSDelay=300);

private:
    virtual UINT getRowCount()
    {
        return m_uRowCount;
    }

    virtual UINT getItemCount() = 0;

    void _onPaint(CPainter& painter, const QRect& rc) override;
    virtual void _onPaintItem(CPainter&, QRect&, const tagListViewItem&) = 0;

    void _onMouseEvent(E_MouseEventType, QMouseEvent&) override;
    virtual void _handleRowClick(UINT uRowIdx, QMouseEvent&) {(void)uRowIdx;}
    virtual void _handleRowDblClick(UINT uRowIdx, QMouseEvent&) {(void)uRowIdx;}

    void _onTouchMove(int dx, int dy) override;

    void _onTouchSwipe(ulong dt, int dx, int dy) override;

    bool _scroll(int dy);
    void _scrollEx(int dy, UINT uCount);
};
