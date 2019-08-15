
#pragma once

#include "widget.h"

struct tagListViewItem
{
    UINT uItem = 0;
    UINT uRow = 0;

    bool bSelect = false;
    bool bFlash = false;
};

class CListRowCount
{
public:
    CListRowCount(UINT uRowCount=0)
        : m_uRowCount(uRowCount)
    {
    }

private:
    UINT m_uRowCount;

public:
    void setRowCount(UINT uRowCount)
    {
        m_uRowCount = uRowCount;
    }

    virtual UINT getRowCount()
    {
        return m_uRowCount;
    }
};

class CListView : public CWidget<QWidget>, public CListRowCount
{
public:
    CListView(QWidget *parent=NULL, UINT uRowCount=0)
        : CWidget(parent)
        , CListRowCount(uRowCount)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    UINT m_uRowHeight = 0;

    UINT m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    int m_nSelectItem = -1;
    int m_nFlashItem = -1;

public:
    float scrollPos() const
    {
        return m_fScrollPos;
    }

    void update()
    {
        CWidget<QWidget>::update();
    }

    void update(float fScrollPos, bool bReset=false)
    {
        m_fScrollPos = fScrollPos;

        if (bReset)
        {
            m_nSelectItem = -1;
            m_nFlashItem = -1;
        }

        CWidget<QWidget>::update();
    }

    void selectItem(UINT uItem)
    {
        showItem(uItem);

        m_nSelectItem = uItem;

        CWidget<QWidget>::update();
    }

    void dselectItem()
    {
        m_nSelectItem = -1;

        CWidget<QWidget>::update();
    }

    void showItem(UINT uItem, bool bToTop=false);

    void flashItem(UINT uItem, UINT uMSDelay=300);

protected:
    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) override;
    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

private:
    virtual UINT getItemCount() = 0;

    void _onPaint(CPainter& painter, const QRect& rc) override;
    virtual void _onPaintItem(CPainter&, QRect&, const tagListViewItem&) = 0;

    virtual void _handleRowClick(UINT uRowIdx, const QMouseEvent&) {(void)uRowIdx;}
    virtual void _handleRowDblClick(UINT uRowIdx, const QMouseEvent&) {(void)uRowIdx;}

    bool _scroll(int dy);
    void _scrollEx(int dy, ulong dt, ulong total);
};
