
#pragma once

#include "widget.h"

class CListView : public CWidget<QWidget>
{
protected:
    struct tagLVRow
    {
        tagLVRow() {}

        tagLVRow(UINT t_uIdx, UINT t_uRow, UINT t_uCol, bool t_bSelected, bool t_bFlashing)
            : uIdx(t_uIdx)
            , uRow(t_uRow)
            , uCol(t_uCol)
            , bSelected(t_bSelected)
            , bFlashing(t_bFlashing)
        {
        }

        UINT uIdx = 0;
        UINT uRow = 0;
        UINT uCol = 0;

        bool bSelected = false;
        bool bFlashing = false;

        QRect rc;
    };

    enum E_RowStyle
    {
        IS_None = 0

        , IS_CenterAlign    = 0x0001
        , IS_MultiLine      = 0x0002

        , IS_BottomLine     = 0x0004
        , IS_RightTip       = 0x0008
        , IS_RightButton    = 0x0010
    };
    struct tagRowContext
    {
        tagRowContext(tagLVRow& t_lvRow, UINT t_eStyle = E_RowStyle::IS_None)
            : lvRow(t_lvRow), eStyle(t_eStyle)
        {
        }

        void setPixmap(const QPixmap *t_pixmap, float t_fIconMargin)
        {
            pixmap = t_pixmap;
            fIconMargin = t_fIconMargin;
        }

        tagLVRow& lvRow;

        UINT eStyle = E_RowStyle::IS_None;

        const QPixmap *pixmap = NULL;
        float fIconMargin = 0.18f;

        wstring strText;
    };

public:
    CListView(QWidget *parent) : CWidget(parent)
    {
        setAttribute(Qt::WA_TranslucentBackground);

        /*if (!lstGestureType.empty())
        {
            grabGesture(lstGestureType);
        }*/

        (void)m_pmRightTip.load(":/img/righttip.png");
    }

private:
    QPixmap m_pmRightTip;

    UINT m_uRowHeight = 0;

    size_t m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    int m_nSelectRow = -1;
    int m_nSelectCol = -1;

    int m_nFlashRow = -1;

    ulong m_uAutoScrollSeq = 0;

    QColor m_crFlashText;

    map<void*, float> m_mapScrollRecord;

protected:
    UINT rowHeight() const
    {
        return m_uRowHeight;
    }

private:
    virtual size_t getColumnCount()
    {
        return 1;
    }

    virtual size_t getRowCount() = 0;

    void _onPaint(CPainter& painter, const QRect&) override;

    virtual void _onPaintRow(CPainter&, tagLVRow&) = 0;

    virtual void _onRowClick(tagLVRow&, const QMouseEvent&) {}
    virtual void _onRowDblClick(tagLVRow&, const QMouseEvent&) {}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, UINT dt, UINT total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}

protected:
    virtual size_t getPageRowCount() = 0;

    bool isAutoScrolling() const
    {
        return m_uAutoScrollSeq > 0;
    }

    bool _hittest(int x, int y, tagLVRow& lvRow);

    virtual void _onPaint(CPainter& painter, int cx, int cy);

    void _paintRow(CPainter&, const tagRowContext&);

    virtual void _paintText(CPainter& painter, QRect& rc, const tagRowContext& context);

    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) override;

    virtual void _onTouchEvent(E_TouchEventType, const CTouchEvent&) override;

    float _scrollRecord(void* p)
    {
        return m_mapScrollRecord[p];
    }

    void _saveScrollRecord(void* p)
    {
        m_mapScrollRecord[p] = scrollPos();

        reset();
    }

    void _clearScrollRecord(void *p)
    {
        m_mapScrollRecord.erase(p);

        reset();
    }

public:
    float scrollPos() const
    {
        return m_fScrollPos;
    }

    void scroll(float fScrollPos)
    {
        m_fScrollPos = fScrollPos;

        update();
    }

    void showRow(UINT uRow, bool bToCenter=false);

    void selectRow(UINT uRow, int nCol = -1)
    {
        m_nSelectRow = uRow;
        m_nSelectCol = nCol;

        update();
    }

    void dselectRow()
    {
        m_nSelectRow = -1;

        update();
    }

    void flashRow(UINT uRow, UINT uMSDelay=300);

    void reset()
    {
        m_fScrollPos = 0;

        m_nSelectRow = -1;
        m_nFlashRow = -1;

        m_uAutoScrollSeq = 0;
    }

    /*void setTextColor(const QColor& crText)
    {
        CWidget<>::setTextColor(crText);
    }

    void setTextColor(int r, int g, int b, int a=255)
    {
        setTextColor(QColor(r,g,b,a));
    }

    void setTextColor(const QColor& crText, const QColor& crFlashText)
    {
        CWidget<>::setTextColor(crText);
    }*/
};
