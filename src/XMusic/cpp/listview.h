
#pragma once

#include "widget.h"

class CListView : public CWidget<QWidget>
{
protected:
    struct tagLVRow
    {
        tagLVRow(){}

        tagLVRow(UINT t_uIdx, UINT t_uRow, UINT t_uCol, bool t_bSelect, bool t_bFlash)
            : uIdx(t_uIdx)
            , uRow(t_uRow)
            , uCol(t_uCol)
            , bSelect(t_bSelect)
            , bFlash(t_bFlash)
        {
        }

        UINT uIdx = 0;
        UINT uRow = 0;
        UINT uCol = 0;

        bool bSelect = false;
        bool bFlash = false;

        QRect rc;
    };

    enum E_RowStyle
    {
        IS_None = 0

        , IS_CenterAlign    = 0x1
        , IS_MultiLine      = 0x2
        , IS_RightTip       = 0x4
        , IS_BottomLine     = 0x8
    };
    struct tagRowContext
    {
        tagRowContext(UINT t_eStyle = E_RowStyle::IS_None)
            : eStyle(t_eStyle)
        {
        }

        void setPixmap(const QPixmap *t_pixmap, float t_fIconMargin)
        {
            pixmap = t_pixmap;
            fIconMargin = t_fIconMargin;
        }

        UINT eStyle = E_RowStyle::IS_None;

        const QPixmap *pixmap = NULL;
        float fIconMargin = 0.18f;

        wstring strText;

        wstring strRemark;
    };

public:
    CListView(QWidget *parent=NULL, UINT uColumnCount = 1, UINT uPageRowCount=0
            , const list<Qt::GestureType>& lstGestureType={})
        : CWidget(parent, lstGestureType)
        , m_uPageRowCount(uPageRowCount)
        , m_uColumnCount(uColumnCount)
        , m_crSelectedBkg(0,0,0,5)
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

    QColor m_crFlashText;

    QColor m_crSelectedBkg;

    map<void*, float> m_mapScrollRecord;

protected:
    UINT rowHeight() const
    {
        return m_uRowHeight;
    }

private:
    virtual size_t getColumnCount()
    {
        return m_uColumnCount;
    }

    virtual size_t getRowCount() = 0;

    void _onPaint(CPainter& painter, const QRect& rc) override;

    virtual void _onPaintRow(CPainter&, const tagLVRow&) = 0;

    virtual void _onRowClick(const tagLVRow&, const QMouseEvent&) {}
    virtual void _onRowDblClick(const tagLVRow&, const QMouseEvent&) {}

    bool _scroll(int dy);
    void _autoScroll(ulong uSeq, int dy, ulong dt, ulong total);

    virtual void _onAutoScrollBegin() {}
    virtual void _onAutoScrollEnd() {}

protected:
    virtual size_t getPageRowCount()
    {
        return m_uPageRowCount;
    }

    bool isAutoScrolling() const
    {
        return m_uAutoScrollSeq > 0;
    }

    bool _hittest(int x, int y, tagLVRow& lvRow);

    void _paintRow(CPainter&, const tagLVRow&, const tagRowContext&);

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
    void setPageRowCount(UINT uPageRowCount)
    {
        m_uPageRowCount = uPageRowCount;
    }

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

    void setTextColor(const QColor& crText)
    {
        m_crFlashText = crText;
        m_crFlashText.setAlpha(crText.alpha()/2);

        CWidget<>::setTextColor(crText);
    }

    void setTextColor(int r, int g, int b, int a=255)
    {
        setTextColor(QColor(r,g,b,a));
    }

    void setTextColor(const QColor& crText, const QColor& crFlashText)
    {
        m_crFlashText = crFlashText;

        CWidget<>::setTextColor(crText);
    }

    void setSelectedBkgColor(const QColor& crSelectedBkg)
    {
        m_crSelectedBkg = crSelectedBkg;
    }
};
