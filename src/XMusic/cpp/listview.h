
#pragma once

#include "widget.h"

#define __lvRowMargin __size(40)

struct tagLVItem
{
    tagLVItem() = default;

    tagLVItem(UINT uRow, UINT uCol, UINT uItem)
        : uRow(uRow)
        , uCol(uCol)
        , uItem(uItem)
    {
    }

    UINT uRow = 0;
    UINT uCol = 0;
    UINT uItem = 0;

    QRect rc;
};

enum E_LVItemStyle
{
    IS_None = 0

    , IS_CenterAlign    = 0x0001
    , IS_MultiLine      = 0x0002

    , IS_BottomLine     = 0x0004
    , IS_RightTip       = 0x0008
};
struct tagLVItemContext
{
    tagLVItemContext(tagLVItem& lvItem, UINT eStyle = E_LVItemStyle::IS_None)
        : lvItem(lvItem), eStyle(eStyle)
    {
    }

    void setIcon(const QPixmap *pm, float fMargin)
    {
        pmIcon = pm;
        fIconMargin = fMargin;
    }

    tagLVItem* operator->()
    {
        return &lvItem;
    }
    const tagLVItem* operator->() const
    {
        return &lvItem;
    }

    tagLVItem& lvItem;

    UINT eStyle = E_LVItemStyle::IS_None;

    const QPixmap *pmIcon = NULL;
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
    CListView(QWidget *parent, E_LVScrollBar eScrollBar = E_LVScrollBar::LVSB_None)
        : TWidget(parent)
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

    UINT m_uRowHeight = 1;
    UINT m_uTotalRows = 0;

    int m_yBar = 0;

    size_t m_uMaxScrollPos = 0;

    float m_fScrollPos = 0;

    ulong m_uAutoScrollSeq = 0;

    int m_nSelectItem = -1;

    map<void*, float> m_mapScrollRecord;

protected:
    UINT rowHeight() const
    {
        return m_uRowHeight;
    }

private:
    void _onPaint(CPainter& painter, cqrc) override;

    virtual void _onPaintItem(CPainter& painter, tagLVItem& lvItem)
    {
        tagLVItemContext context(lvItem);
        _genItemContext(context);
        _paintRow(painter, context);
    }

    virtual void _genItemContext(tagLVItemContext&) {}

    virtual void _onRowClick(tagLVItem&, const QMouseEvent&) {}
    virtual void _onRowDblClick(tagLVItem&, const QMouseEvent&) {}

    virtual void _onBlankClick(const QMouseEvent&) {}
    virtual void _onBlankDblClick(const QMouseEvent&) {}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, UINT dt, UINT total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}

    bool _checkBarArea(int x);

protected:
    virtual size_t getColCount() const
    {
        return 1;
    }

    virtual size_t getRowCount() const = 0;

    virtual size_t getItemCount() const = 0;

    bool isAutoScrolling() const
    {
        return m_uAutoScrollSeq > 0;
    }

    bool _hittest(int x, int y, tagLVItem& lvItem);

    virtual void _onPaint(CPainter& painter, int cx, int cy);

    void _paintRow(CPainter&, tagLVItemContext&);

    virtual cqrc _paintText(tagLVItemContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha);

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

    void showItem(UINT uItem);
    void showItemTop(UINT uItem);
    void showItemCenter(UINT uItem);

    void selectItem(UINT uItem)
    {
        m_nSelectItem = uItem;

        update();
    }

    void dselectItem()
    {
        m_nSelectItem = -1;

        update();
    }

    virtual void reset()
    {
        m_fScrollPos = 0;

        m_uAutoScrollSeq = 0;

        m_nSelectItem = -1;

        update();
    }
};
