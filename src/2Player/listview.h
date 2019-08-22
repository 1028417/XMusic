
#pragma once

#include "widget.h"

struct tagLVRow
{
    tagLVRow(UINT t_uRow, UINT t_uCol, bool t_bSelect, bool t_bFlash)
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

class CListView : public CWidget<QWidget>
{
public:
    CListView(QWidget *parent=NULL, UINT uColumnCount = 1, UINT uPageRowCount=0)
        : CWidget(parent)
        , m_uPageRowCount(uPageRowCount)
        , m_uColumnCount(uColumnCount)
    {
        setAttribute(Qt::WA_TranslucentBackground);

        (void)m_pmRightTip.load(":/img/righttip.png");
    }

private:
    QPixmap m_pmRightTip;

    UINT m_uPageRowCount = 0;
    UINT m_uColumnCount = 1;

    UINT m_uRowHeight = 0;

    UINT m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    int m_nSelectRow = -1;
    int m_nSelectCol = -1;

    int m_nFlashRow = -1;

    ulong m_uAutoScrollSeq = 0;

protected:    
    virtual UINT getPageRowCount()
    {
        return m_uPageRowCount;
    }

    virtual UINT getColumnCount()
    {
        return m_uColumnCount;
    }

    bool isAutoScrolling() const
    {
        return m_uAutoScrollSeq > 0;
    }

    enum class E_RowStyle
    {
        IS_None
        , IS_Underline
        , IS_RightTip
    };
    struct tagRowContext
    {
        tagRowContext(){}

        tagRowContext(E_RowStyle t_eStyle, const wstring& t_strText)
            : eStyle(t_eStyle)
            , strText(t_strText)
        {
        }

        E_RowStyle eStyle = E_RowStyle::IS_None;

        QPixmap *pixmap = NULL;
        UINT uIconSize = 0;

        wstring strText;
        wstring strRemark;
    };
    void _paintRow(CPainter&, QRect&, const tagLVRow&, const tagRowContext&);

    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) override;

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

public:
    void setPageRowCount(UINT uPageRowCount)
    {
        m_uPageRowCount = uPageRowCount;
    }

    float scrollPos() const
    {
        return m_fScrollPos;
    }

    void update()
    {
        CWidget<>::update();
    }

    void update(float fScrollPos)
    {
        m_fScrollPos = fScrollPos;

        CWidget<>::update();
    }

    void reset()
    {
        m_fScrollPos = 0;

        m_nSelectRow = -1;
        m_nFlashRow = -1;

        m_uAutoScrollSeq = 0;
    }

    void selectRow(UINT uRow, int nCol = -1)
    {
        m_nSelectRow = uRow;
        m_nSelectCol = nCol;

        CWidget<>::update();
    }

    void dselectRow()
    {
        m_nSelectRow = -1;

        CWidget<>::update();
    }

    void showRow(UINT uRow, bool bToCenter=false);

    void flashRow(UINT uRow, UINT uMSDelay=300);

private:
    virtual UINT getRowCount() = 0;

    void _onPaint(CPainter& painter, const QRect& rc) override;
    virtual void _onPaintRow(CPainter&, QRect&, const tagLVRow&) = 0;

    virtual void _onRowClick(const tagLVRow&, const QMouseEvent&) {}
    virtual void _onRowDblClick(const tagLVRow&, const QMouseEvent&) {}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, ulong dt, ulong total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}
};
