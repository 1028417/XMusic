
#pragma once

#include "widget.h"

struct tagListViewRow
{
    tagListViewRow(UINT t_uRow, UINT t_uCol, bool t_bSelect, bool t_bFlash)
        : uRow(t_uRow)
        , uCol(t_uCol)
        , bSelect(t_bSelect)
        , bFlash(t_bFlash)
    {
    }

    UINT uRow;
    UINT uCol;

    UINT uDislpayRow = 0;

    bool bSelect;
    bool bFlash;
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
    UINT m_uColumnCount = 1;

    UINT m_uRowHeight = 0;

    UINT m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    int m_nSelectRow = -1;
    int m_nSelectCol = -1;

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

    void selectRow(UINT uRow, int nCol = -1)
    {
        showRow(uRow);

        m_nSelectRow = uRow;
        m_nSelectCol = nCol;

        CWidget<>::update();
    }

    void dselectRow()
    {
        m_nSelectRow = -1;

        CWidget<>::update();
    }

    void showRow(UINT uRow, bool bToTop=false);

    void flashRow(UINT uRow, UINT uMSDelay=300);

protected:
    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) override;

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

private:
    virtual UINT getRowCount() = 0;

    void _onPaint(CPainter& painter, const QRect& rc) override;
    virtual void _onPaintRow(CPainter&, QRect&, const tagListViewRow&) = 0;

    virtual void _handleRowClick(const tagListViewRow&, const QMouseEvent&) {}
    virtual void _handleRowDblClick(const tagListViewRow&, const QMouseEvent&) {}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, ulong dt, ulong total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}
};
