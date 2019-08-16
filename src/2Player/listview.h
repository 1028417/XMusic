
#pragma once

#include "widget.h"

struct tagListViewRow
{
    UINT uRow = 0;

    bool bSelect = false;
    bool bFlash = false;
};

class CListRowCount
{
public:
    CListRowCount(UINT uPageRowCount=0)
        : m_uPageRowCount(uPageRowCount)
    {
    }

private:
    UINT m_uPageRowCount = 0;


public:
    void setPageRowCount(UINT uPageRowCount)
    {
        m_uPageRowCount = uPageRowCount;
    }

    virtual UINT getPageRowCount()
    {
        return m_uPageRowCount;
    }
};

class CListView : public CWidget<QWidget>, public CListRowCount
{
public:
    CListView(QWidget *parent=NULL, UINT uColumnCount = 1, UINT uPageRowCount=0)
        : CWidget(parent)
        , CListRowCount(uPageRowCount)
        , m_uColumnCount(uColumnCount)
    {
        setAttribute(Qt::WA_TranslucentBackground);
    }

private:
    UINT m_uColumnCount = 0;

    UINT m_uRowHeight = 0;

    UINT m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    int m_nSelectRow = -1;
    int m_nFlashRow = -1;

    ulong m_uTouchSeq = 0;

    ulong m_uAutoScrollSeq = 0;

protected:
    bool isAutoScrolling() const
    {
        return m_uAutoScrollSeq > 0;
    }

public:
    float scrollPos() const
    {
        return m_fScrollPos;
    }

    void update()
    {
        CWidget<>::update();
    }

    void update(float fScrollPos, bool bReset=false)
    {
        m_fScrollPos = fScrollPos;

        if (bReset)
        {
            m_nSelectRow = -1;
            m_nFlashRow = -1;

            m_uTouchSeq = 0;
        }

        CWidget<>::update();
    }

    void selectItem(UINT uItem)
    {
        showItem(uItem);

        m_nSelectRow = uItem;

        CWidget<>::update();
    }

    void dselectItem()
    {
        m_nSelectRow = -1;

        CWidget<>::update();
    }

    void showItem(UINT uItem, bool bToTop=false);

    void flashItem(UINT uItem, UINT uMSDelay=300);

protected:
    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) override;

    void _handleMouseEvent(E_MouseEventType type, const QMouseEvent& me) override;

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

private:
    virtual UINT getRowCount() = 0;

    void _onPaint(CPainter& painter, const QRect& rc) override;
    virtual void _onPaintItem(CPainter&, QRect&, const tagListViewRow&) = 0;

    virtual void _handleRowClick(UINT uRowIdx, const QMouseEvent&) {(void)uRowIdx;}
    virtual void _handleRowDblClick(UINT uRowIdx, const QMouseEvent&) {(void)uRowIdx;}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, ulong dt, ulong total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}
};
