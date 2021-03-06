
#include "xmusic.h"

#include "listview.h"

#define __cxBar __size(5)

CListView::CListView(QWidget *parent, E_LVScrollBar eScrollBar)
    : CWidget(parent)
    , m_eScrollBar(eScrollBar)
    , m_pmForward(g_app.m_pmForward)
{
}

void CListView::showItem(UINT uItem, bool bToCenter)
{
    auto uRow = uItem/getColCount();
    if (bToCenter)
    {
        auto fPos = (float)uRow - (getRowCount()-1)/2;
        fPos = MAX(.0f, fPos);
        scroll(fPos);
    }
    else
    {
        if (uRow < m_fScrollPos)
        {
            scroll(uRow);
        }
        else
        {
            size_t uRowCount = getRowCount();
            if (uRow+1 > m_fScrollPos+uRowCount)
            {
                scroll(uRow+1-uRowCount);
            }
        }
    }
}

void CListView::_onPaint(CPainter& painter, cqrc)
{
    int cx = width();
    int cy = height();
    _onPaint(painter, cx, cy);
}

void CListView::_onPaint(CPainter& painter, int cx, int cy)
{
    size_t uRowCount = getRowCount();
    uRowCount = MAX(1, uRowCount);

    cy -= cy%uRowCount;

    m_uRowHeight = cy/uRowCount;
    m_uRowHeight = MAX(1, m_uRowHeight);

    size_t uColCount = getColCount();
    uColCount = MAX(1, uColCount);

    size_t uItemCount = getItemCount();
    m_uTotalRows = uItemCount/uColCount;
    if (uItemCount%uColCount != 0)
    {
        m_uTotalRows++;
    }

    if (m_uTotalRows > uRowCount)
    {
        m_uMaxScrollPos = m_uTotalRows - uRowCount;
    }
    else
    {
        m_uMaxScrollPos = 0;
    }

    m_fScrollPos = MIN(m_fScrollPos, m_uMaxScrollPos);

    UINT uRow = m_fScrollPos;
    m_fTopItem = m_fScrollPos-uRow+uRow*uColCount;

    int y = int(-(m_fScrollPos-uRow)*m_uRowHeight);

    UINT cx_col = cx/uColCount;
    cx_col = MAX(1, cx_col);
    int x0 = (cx%uColCount)/2;

    UINT cxMargin = 0;
    if (E_LVScrollBar::LVSB_None != m_eScrollBar)
    {
        cxMargin = __lvRowMargin;
    }

    m_lstCurrentItems.clear();
    do {
        painter.setFont(this->font());
        painter.setPen(g_crFore);

        int x = x0;
        for (UINT uCol = 0; uCol < uColCount; uCol++, x += cx_col)
        {
            auto uItem = uRow*uColCount+uCol;
            m_lstCurrentItems.push_back(uItem);

            tagLVItem lvItem(uRow, uCol, uItem);
            lvItem.rc.setRect(x + cxMargin, y, cx_col - cxMargin*2, m_uRowHeight);

            if ((int)lvItem.uItem == m_nSelectItem)
            {
                QColor cr = g_crFore;
                cr.setAlpha(CPainter::oppTextAlpha(20));
                painter.fillRect(x, y, cx_col, m_uRowHeight, cr);
            }

            _onPaintItem(painter, lvItem);
        }

        uRow++;
        y += m_uRowHeight;
    } while (y < cy);

    if (m_uMaxScrollPos != 0 && E_LVScrollBar::LVSB_None != m_eScrollBar)
    {
        int szMargin = (__lvRowMargin-__cxBar)/2;
        int xBar = E_LVScrollBar::LVSB_Left == m_eScrollBar ? szMargin : cx-szMargin-__cxBar;
        cy -= szMargin*2;
        auto cyBar = cy*uRowCount/m_uTotalRows;
        m_yBar = szMargin + m_fScrollPos*(cy-cyBar)/m_uMaxScrollPos;
        QRect rcBar(xBar, m_yBar, __cxBar, cyBar);

        auto cr = g_crFore;
        cr.setAlpha(128);
        painter.fillRectEx(rcBar, cr, __cxBar/2);
    }
}

void CListView::_paintRow(CPainter& painter, tagLVItemContext& context)
{
    QRect rc = context->rc;
    int cy = rc.height();

    if ((context.brIcon && *context.brIcon) || (context.pmIcon && !context.pmIcon->isNull()))
    {
        int szIcon = 0;
        if (context.nIconSize > 0)
        {
            szIcon = MIN(cy, context.nIconSize);
        }
        else
        {
            szIcon = cy + context.nIconSize;
            if (szIcon <= 0)
            {
                szIcon = cy;
            }
        }

        auto xIcon = rc.left();
        if (context.uStyle & E_LVItemStyle::IS_CenterAlign)
        {
            xIcon = rc.center().x()-szIcon-__lvRowMargin/2;
        }

        int y_icon = rc.center().y()-szIcon/2;
        QRect rcIcon(xIcon, y_icon, szIcon, szIcon);
        _paintIcon(context, painter, rcIcon);
        rc.setLeft(xIcon + szIcon + __lvRowMargin);
    }

    if (context.uStyle & E_LVItemStyle::IS_BottomLine)
    {
        _paintBottonLine(painter, rc);
    }

    if (context.uStyle & E_LVItemStyle::IS_ForwardButton)
    {
        int sz_righttip = __size(37);
        int x_righttip = rc.right()-sz_righttip;
        int y_righttip = rc.center().y()-sz_righttip/2;
        QRect rcRighttip(x_righttip, y_righttip, sz_righttip, sz_righttip);
        painter.drawImg(rcRighttip, m_pmForward);

        rc.setRight(x_righttip - __lvRowMargin);
    }

    if ((int)context->uItem == m_nSelectItem)
    {
        painter.adjustFontWeight(TD_FontWeight::DemiBold);
    }

    int flags = Qt::AlignLeft|Qt::AlignVCenter;
    if (context.uStyle & E_LVItemStyle::IS_SingleLine)
    {
        flags |= Qt::TextSingleLine;
    }
    else
    {
        flags |= Qt::TextWrapAnywhere;
    }
    _paintText(context, painter, rc, flags, 255, __ShadowAlpha);
}

void CListView::_paintBottonLine(CPainter& painter, QRect& rc)
{
    QColor cr = g_crFore;
    cr.setAlpha(CPainter::oppTextAlpha(20));
    painter.drawRectEx(QRect(rc.left(), rc.bottom(), rc.width(), 1), cr);
    rc.setBottom(rc.bottom()-3);
}

void CListView::_paintIcon(tagLVItemContext& context, CPainter& painter, cqrc rc)
{
    if (context.brIcon)
    {
        QRect rcSrc(0,0,context.brIcon->width(), context.brIcon->height());
        painter.drawBrushEx(rc, *context.brIcon, rcSrc, context.uIconRound);
    }
    else
    {
        painter.drawImgEx(rc, *context.pmIcon, context.uIconRound);
    }
}

cqrc CListView::_paintText(tagLVItemContext& context, CPainter& painter, QRect& rc
                           , int flags, UINT uTextAlpha, UINT uShadowAlpha)
{
    /*QString qsText = context.strText;
    if (context.uStyle & E_LVItemStyle::IS_SingleLine)
    {
        qsText = painter.fontMetrics().elidedText(qsText, Qt::ElideRight, rc.width()
                                                  , Qt::TextSingleLine | Qt::TextShowMnemonic);
    }*/

    return painter.drawTextEx(rc, flags, context.strText, 1, uShadowAlpha, uTextAlpha);
}

bool CListView::_hittest(int& x, int& y, tagLVItem& lvItem)
{
    UINT uRow = UINT((float)y/m_uRowHeight + m_fScrollPos);
    if (uRow >= m_uTotalRows)
    {
        return false;
    }

    UINT uColCount = getColCount();
    UINT cx_col = width()/uColCount;
    cx_col = MAX(1, cx_col);
    UINT uCol = UINT(x/cx_col);
    lvItem = tagLVItem(uRow, uCol, uRow*uColCount+uCol);

    int left = uCol*cx_col;
    int top = int(-(m_fScrollPos-uRow)*m_uRowHeight);
    lvItem.rc.setRect(left, top, cx_col, m_uRowHeight);

    x -= left;
    y -= top;

    return true;
}

inline bool CListView::_checkBarArea(int x)
{
    return ((E_LVScrollBar::LVSB_Left == m_eScrollBar && x <= __lvRowMargin)
        || (E_LVScrollBar::LVSB_Right == m_eScrollBar && x >= width()-__lvRowMargin));
}

void CListView::_onMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
    if (m_uMaxScrollPos != 0 && _checkBarArea(me.pos().x()))
    {
        return;
    }

    if (E_MouseEventType::MET_Press == type)
    {
        if (m_uAutoScrollSeq > 0)
        {
            m_uAutoScrollSeq = 0;

            _onAutoScrollEnd();

            disableClick();
        }
    }
#if __windows || __mac // 移动端双击偏移大，在单击事件中判断
    else if (E_MouseEventType::MET_DblClick == type)
    {
        tagLVItem lvItem;
        int x = me.pos().x();
        int y = me.pos().y();
        if (_hittest(x, y, lvItem))
        {
            auto t_me = me;
            t_me.setLocalPos(QPoint(x,y));
            _onItemDblClick(lvItem, t_me);
        }
    }
#endif
    else if (E_MouseEventType::MET_Click == type)
    {
        int nItem = -1;

        tagLVItem lvItem;
        int x = me.pos().x();
        int y = me.pos().y();
        if (_hittest(x, y, lvItem))
        {
            nItem = lvItem.uItem;

            auto t_me = me;
            t_me.setLocalPos(QPoint(x,y));
            _onItemClick(lvItem, t_me);
        }
        else
        {
            _onBlankClick(me);
        }
        (void)nItem;

#if __android || __ios // 允许移动端双击偏移大
#define __tsDblClick 333//222
        static int s_nPrevItem = -1;
        static ulong s_tsPrevClick = 0;
        do {
            if (nItem >=0 && nItem == s_nPrevItem
                    && me.timestamp() - s_tsPrevClick < __tsDblClick)
            {
                _onItemDblClick(lvItem, me);
                break;
            }

            s_nPrevItem = nItem;
            s_tsPrevClick = me.timestamp();
        } while(0);
#endif
    }
}

void CListView::_onTouchEvent(const CTouchEvent& te)
{
    auto teType = te.type();

    if (m_uMaxScrollPos != 0)
    {
        static bool bTouchBarArea = false;
        static int nTouchBarPos = 0;
        if (E_TouchEventType::TET_TouchBegin == teType)
        {
            bTouchBarArea = _checkBarArea(te.x());
            if (bTouchBarArea)
            {
                nTouchBarPos = te.y()-m_yBar;
            }
        }

        if (bTouchBarArea)
        {
            __yield();

            UINT uRowCount = getRowCount();

            int cyBar = height()*uRowCount/m_uTotalRows;
            int y = te.y()-(__lvRowMargin-__cxBar)/2;
            if (nTouchBarPos > 0 && nTouchBarPos < cyBar)
            {
                y -= nTouchBarPos;
            }
            else
            {
                y -= cyBar/2;
            }

            y = MAX(y, 0);
            auto fPos = (float)m_uMaxScrollPos*y/(height()-(__lvRowMargin-__cxBar)-cyBar);
            scroll(fPos);

            return;
        }
    }

    if (E_TouchEventType::TET_TouchMove == teType)
    {
        _scroll(te.dy());
    }
    else if (E_TouchEventType::TET_TouchEnd == teType)
    {
        auto dy = te.dy();
        if (dy < 3 && dy > -3)
        {
            return;
        }

        auto dt = te.dt();
        if (dt < 300)
        {
            if (dy > 30 || dy < -30)
            {
                dy /= 3;

                dt /= 3;
                dt = MAX(30, dt);
            }
        }
        else
        {
            if (dt > 1000)
            {
                return;
            }

            dt = 30;

            if (dy > 0)
            {
                dy = 3+rowHeight()/100;
            }
            else
            {
                dy = -3-(int)rowHeight()/100;
            }
        }

        _onAutoScrollBegin();

        m_uAutoScrollSeq = te.timestamp();
        _autoScroll(m_uAutoScrollSeq, dy, dt, 2000);
    }
}

bool CListView::_scroll(int dy)
{
    m_fScrollPos -= (float)dy/m_uRowHeight;

    bool bFlag = false;
    if (dy < 0)
    {
        if (m_fScrollPos > m_uMaxScrollPos)
        {
            m_fScrollPos = m_uMaxScrollPos;
            bFlag = true;
        }
    }
    else
    {
        if (m_fScrollPos  < 0)
        {
            m_fScrollPos = 0;
            bFlag = true;
        }
    }

    update();

    return bFlag;
}

void CListView::_autoScroll(ulong uSeq, int dy, UINT dt, UINT total)
{
    async(dt, [=]{
        if (uSeq != m_uAutoScrollSeq)
        {
            return;
        }

        if (_scroll(dy) || total < dt)
        {
            m_uAutoScrollSeq = 0;

            _onAutoScrollEnd();

            return;
        }

        _autoScroll(uSeq, dy, dt, total-dt);
    });
}

bool CListView::event(QEvent *ev)
{
    if (ev->type() == QEvent::Resize)
    {
        if (m_fTopItem > 0)
        {
            scrollToItem(m_fTopItem);
        }
        else
        {
            m_uAutoScrollSeq = 0;
        }
    }

    return TWidget::event(ev);
}
