
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
    , IS_SingleLine     = 0x0002

    , IS_BottomLine     = 0x0004
    , IS_ForwardButton  = 0x0008
};
struct tagLVItemContext
{
    tagLVItemContext(tagLVItem& lvItem, UINT uStyle = E_LVItemStyle::IS_None)
        : lvItem(lvItem), uStyle(uStyle)
    {
    }

    inline void setIcon(cqpm pm, int nSize)
    {
        brIcon = NULL;

        pmIcon = &pm;
        nIconSize = nSize;
    }
    void setIcon(cqpm pm, int nSize, UINT uRound)
    {
        setIcon(pm, nSize);
        uIconRound = uRound;
    }

    inline void setIcon(CBrush& br, int nSize)
    {
        pmIcon = NULL;

        brIcon = &br;
        nIconSize = nSize;
    }
    void setIcon(CBrush& br, int nSize, UINT uRound)
    {
        setIcon(br, nSize);
        uIconRound = uRound;
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

    UINT uStyle = (UINT)E_LVItemStyle::IS_None;

    int nIconSize = 0;
    CBrush *brIcon = NULL;
    const QPixmap *pmIcon = NULL;
    UINT uIconRound = 0;

    WString strText;
};

enum class E_LVScrollBar
{
    LVSB_None,
    LVSB_Left,
    LVSB_Right
};

class CListView : public CWidget
{
public:
    CListView(QWidget *parent, E_LVScrollBar eScrollBar = E_LVScrollBar::LVSB_None);

protected:
    E_LVScrollBar m_eScrollBar = E_LVScrollBar::LVSB_None;

private:
    cqpm m_pmForward;

    UINT m_uRowHeight = 1;
    UINT m_uTotalRows = 0;

    int m_yBar = 0;

    size_t m_uMaxScrollPos = 0;

    float m_fScrollPos = 0;
    float m_fTopItem = 0;

    ulong m_uAutoScrollSeq = 0;

    int m_nSelectItem = -1;

    map<void*, float> m_mapScrollRecord;

    list<UINT> m_lstCurrentItems;

private:
    void _onPaint(CPainter& painter, cqrc) override;

    virtual void _onPaintItem(CPainter& painter, tagLVItem& lvItem)
    {
        tagLVItemContext context(lvItem);
        _genItemContext(context);
        _paintRow(painter, context);
    }

    virtual void _genItemContext(tagLVItemContext&) {}

    virtual void _onItemClick(tagLVItem&, const QMouseEvent&) {}
    virtual void _onItemDblClick(tagLVItem&, const QMouseEvent&) {}

    virtual void _onBlankClick(const QMouseEvent&) {}
    virtual void _onBlankDblClick(const QMouseEvent&) {}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, UINT dt, UINT total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}

    bool _checkBarArea(int x);

protected:
    UINT rowHeight() const
    {
        return m_uRowHeight;
    }

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

    bool _hittest(int& x, int& y, tagLVItem& lvItem);

    virtual void _onPaint(CPainter& painter, int cx, int cy);

    void _paintRow(CPainter&, tagLVItemContext&);

    void _paintBottonLine(CPainter& painter, QRect& rc);

    virtual void _paintIcon(tagLVItemContext&, CPainter&, cqrc);
    virtual cqrc _paintText(tagLVItemContext&, CPainter&, QRect&, int flags, UINT uTextAlpha, UINT uShadowAlpha);

    virtual void _onMouseEvent(E_MouseEventType, const QMouseEvent&) override;

    virtual void _onTouchEvent(const CTouchEvent&) override;

    float _scrollRecord(void *p)
    {
        return m_mapScrollRecord[p];
    }

    void _saveScrollRecord(void *p)
    {
        m_mapScrollRecord[p] = m_fTopItem;

        reset();
    }

    void _clearScrollRecord(void *p)
    {
        m_mapScrollRecord.erase(p);

        reset();
    }

    virtual bool event(QEvent *ev) override;

public:
    const list<UINT>& currentItems() const
    {
        return m_lstCurrentItems;
    }

    float topItem() const
    {
        return m_fTopItem;
    }

    inline float scrollPos() const
    {
        return m_fScrollPos;
    }

    void scroll(float fPos)
    {
        m_fScrollPos = fPos;
        m_uAutoScrollSeq = 0;

        update();
    }

    void scrollToItem(float fItemPos)
    {
        m_fScrollPos = fItemPos-(UINT)fItemPos+(UINT)fItemPos/getColCount();
        m_uAutoScrollSeq = 0;

        update();
    }

    void showItem(UINT uItem, bool bToCenter = false);

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
