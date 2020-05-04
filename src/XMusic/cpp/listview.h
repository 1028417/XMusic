
#pragma once

#include "widget.h"

#define __lvRowMargin __size(40)

struct tagLVRow
{
    tagLVRow() = default;

    tagLVRow(UINT t_uIdx, UINT t_uRow, UINT t_uCol, bool t_bSelected)
        : uIdx(t_uIdx)
        , uRow(t_uRow)
        , uCol(t_uCol)
        , bSelected(t_bSelected)
    {
    }

    UINT uIdx = 0;
    UINT uRow = 0;
    UINT uCol = 0;

    bool bSelected = false;

    QRect rc;
};

enum E_RowStyle
{
    IS_None = 0

    , IS_CenterAlign    = 0x0001
    , IS_MultiLine      = 0x0002

    , IS_BottomLine     = 0x0004
    , IS_RightTip       = 0x0008
};
struct tagRowContext
{
    tagRowContext(tagLVRow& t_lvRow, UINT t_eStyle = E_RowStyle::IS_None)
        : lvRow(t_lvRow), eStyle(t_eStyle)
    {
    }

    void setPixmap(const QPixmap *t_pm, float t_fIconMargin)
    {
        pmImg = t_pm;
        fIconMargin = t_fIconMargin;
    }

    tagLVRow* operator->()
    {
        return &lvRow;
    }
    const tagLVRow* operator->() const
    {
        return &lvRow;
    }

    tagLVRow& lvRow;

    UINT eStyle = E_RowStyle::IS_None;

    const QPixmap *pmImg = NULL;
    float fIconMargin = 0.21f;
    UINT uIconRound = __size(6);

    WString strText;
};

enum class E_LVScrollBar
{
    LVSB_None,
    LVSB_Left,
    LVSB_Right
};

class CListView : public TWidget<QWidget>
{
public:
    CListView(QWidget& parent, E_LVScrollBar eScrollBar = E_LVScrollBar::LVSB_None)
        : TWidget(&parent)
        , m_eScrollBar(eScrollBar)
    {
        setAttribute(Qt::WA_TranslucentBackground);

        /*if (!lstGestureType.empty())
        {
            grabGesture(lstGestureType);
        }*/

        (void)m_pmRightTip.load(":/img/righttip.png");
    }

protected:
    E_LVScrollBar m_eScrollBar = E_LVScrollBar::LVSB_None;

private:
    QPixmap m_pmRightTip;

    UINT m_uRowHeight = 0;

    size_t m_uMaxScrollPos = 0;
    float m_fScrollPos = 0;

    int m_nSelectRow = -1;
    int m_nSelectCol = -1;

    ulong m_uAutoScrollSeq = 0;

    QColor m_crFlashText;

    map<void*, float> m_mapScrollRecord;

protected:
    UINT rowHeight() const
    {
        return m_uRowHeight;
    }

private:
    virtual void _reset() {}

    virtual size_t getColumnCount() const
    {
        return 1;
    }

    virtual size_t getRowCount() const = 0;

    void _onPaint(CPainter& painter, cqrc) override;

    virtual void _onPaintRow(CPainter& painter, tagLVRow& lvRow)
    {
        tagRowContext context(lvRow);
        if (_genRowContext(context))
        {
            _paintRow(painter, context);
        }
    }

    virtual bool _genRowContext(tagRowContext&) { return true; }

    virtual void _onRowClick(tagLVRow&, const QMouseEvent&) {}
    virtual void _onRowDblClick(tagLVRow&, const QMouseEvent&) {}

    virtual void _onBlankClick(const QMouseEvent&) {}
    virtual void _onBlankDblClick(const QMouseEvent&) {}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, UINT dt, UINT total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}

    bool _checkBarArea(int x);

protected:
    virtual size_t getPageRowCount() const = 0;

    bool isAutoScrolling() const
    {
        return m_uAutoScrollSeq > 0;
    }

    bool _hittest(int x, int y, tagLVRow& lvRow);

    virtual void _onPaint(CPainter& painter, int cx, int cy);

    void _paintRow(CPainter&, tagRowContext&);

    virtual cqrc _paintText(tagRowContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha);

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

    void scroll(float fPos)
    {
        m_fScrollPos = fPos;
        m_uAutoScrollSeq = 0;

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

    virtual void reset()
    {
        _reset();

        m_fScrollPos = 0;

        m_nSelectRow = -1;

        m_uAutoScrollSeq = 0;

        update();
    }
};
