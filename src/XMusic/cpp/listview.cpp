
#include "listview.h"

#include "app.h"

#define __cxBar __size(5)

void CListView::showItem(UINT uItem, bool bToCenter)
{
    auto uRow = uItem/getColCount();
    if (bToCenter)
    {
        auto fPos = MAX(.0f, (float)uRow - (getRowCount()-1)/2);
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
    size_t uColCount = getColCount();

    size_t uRowCount = getRowCount();
    m_uRowHeight = cy/uRowCount;
    m_uRowHeight = MAX(1, m_uRowHeight);
    cy -= cy%uRowCount;

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

    for (UINT uIdx = 0; uRow < m_uTotalRows; uRow++, uIdx++)
    {
        painter.setFont(this->font());
        painter.setPen(g_crFore);

        int x = x0;
        for (UINT uCol = 0; uCol < uColCount; uCol++, x += cx_col)
        {
            tagLVItem lvItem(uRow, uCol, uRow*uColCount+uCol);
            lvItem.rc.setRect(x + cxMargin, y, cx_col - cxMargin*2, m_uRowHeight);

            if ((int)lvItem.uItem == m_nSelectItem)
            {
                QColor cr = g_crFore;
                cr.setAlpha(CPainter::oppTextAlpha(20));
                painter.fillRect(x, y, cx_col, m_uRowHeight, cr);
            }

            _onPaintItem(painter, lvItem);
        }

        y += m_uRowHeight;
        if (y >= cy)
        {
            break;
        }
    }

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

    if (context.pmIcon && !context.pmIcon->isNull())
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
        if (context.eStyle & E_LVItemStyle::IS_CenterAlign)
        {
            xIcon = rc.center().x()-szIcon-__lvRowMargin;
        }

        int y_icon = rc.center().y()-szIcon/2;
        QRect rcPixmap(xIcon, y_icon, szIcon, szIcon);
        painter.drawPixmapEx(rcPixmap, *context.pmIcon, context.uIconRound);

        rc.setLeft(xIcon + szIcon + __lvRowMargin);
    }

    if (context.eStyle & E_LVItemStyle::IS_BottomLine)
    {
        QColor cr = g_crFore;
        cr.setAlpha(CPainter::oppTextAlpha(20));
        painter.drawRectEx(QRect(rc.left(), rc.bottom(), rc.width(), 1), cr);
        rc.setBottom(rc.bottom()-3);
    }

    if (context.eStyle & E_LVItemStyle::IS_RightTip)
    {
        int sz_righttip = cy*22/100;
        int x_righttip = rc.right()-sz_righttip;
        int y_righttip = rc.center().y()-sz_righttip/2;
        QRect rcRighttip(x_righttip, y_righttip, sz_righttip, sz_righttip);
        painter.drawPixmap(rcRighttip, m_pmRightTip);

        rc.setRight(x_righttip - __lvRowMargin);
    }

    if ((int)context->uItem == m_nSelectItem)
    {
        painter.adjustFont(QFont::Weight::DemiBold);
    }

    int flags = Qt::AlignLeft|Qt::AlignVCenter;
    if (context.eStyle & E_LVItemStyle::IS_MultiLine)
    {
        flags |= Qt::TextWrapAnywhere;
    }
    _paintText(context, painter, rc, flags, __ShadowAlpha, 255);
}

cqrc CListView::_paintText(tagLVItemContext& context, CPainter& painter, QRect& rc
                           , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    QString qsText = context.strText;
    if ((context.eStyle & E_LVItemStyle::IS_MultiLine) == 0)
    {
        qsText = painter.fontMetrics().elidedText(qsText, Qt::ElideRight, rc.width()
                                                  , Qt::TextSingleLine | Qt::TextShowMnemonic);
    }

    return painter.drawTextEx(rc, flags, qsText, 1, uShadowAlpha, uTextAlpha);
}

bool CListView::_checkBarArea(int x)
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
#if __windows || __mac
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
#define __tsDblClick 222
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

void CListView::_onTouchEvent(E_TouchEventType type, const CTouchEvent& te)
{
    if (m_uMaxScrollPos != 0)
    {
        static bool bTouchBarArea = false;
        static int nTouchBarPos = 0;
        if (E_TouchEventType::TET_TouchBegin == type)
        {
            bTouchBarArea = _checkBarArea(te.x());
            if (bTouchBarArea)
            {
                nTouchBarPos = te.y()-m_yBar;
            }
        }

        if (bTouchBarArea)
        {
            mtutil::yield();

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

    if (E_TouchEventType::TET_TouchMove == type)
    {
        _scroll(te.dy());
    }
    else if (E_TouchEventType::TET_TouchEnd == type)
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
    CApp::async(dt, [=]() {
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
