
#include "listview.h"

#include "app.h"

#define __cxBar __size(5)

void CListView::showRow(UINT uRow, bool bToCenter)
{
    m_uAutoScrollSeq = 0;

    size_t uPageRowCount = getPageRowCount();

    if (bToCenter)
    {
        m_fScrollPos = MAX(.0f, (float)uRow - (uPageRowCount-1)/2);
    }
    else
    {
        if (uRow < m_fScrollPos)
        {
            m_fScrollPos = uRow;
        }
        else if (uRow+1 > m_fScrollPos+uPageRowCount)
        {
            m_fScrollPos = uRow+1-uPageRowCount;
        }
        else
        {
            return;
        }
    }

    update();
}

void CListView::_onPaint(CPainter& painter, cqrc)
{
    int cx = width();
    int cy = height();
    _onPaint(painter, cx, cy);
}

void CListView::_onPaint(CPainter& painter, int cx, int cy)
{
    size_t uPageRowCount = getPageRowCount();
    if (0 == uPageRowCount)
    {
        return;
        //uPageRowCount = uRowCount;
    }

    m_uRowHeight = cy/uPageRowCount;

    size_t uRowCount = getRowCount();
    if (uRowCount > uPageRowCount)
    {
        m_uMaxScrollPos = uRowCount - uPageRowCount;
    }
    else
    {
        m_uMaxScrollPos = 0;
    }

    m_fScrollPos = MIN(m_fScrollPos, m_uMaxScrollPos);

    UINT uRow = m_fScrollPos;
    int y = int(-(m_fScrollPos-uRow)*m_uRowHeight);

    size_t uColumnCount = getColumnCount();
    if (0 == uColumnCount)
    {
        uColumnCount = 1;
    }

    int x = 0;
    if (E_LVScrollBar::LVSB_None != m_eScrollBar)
    {
        x = __lvRowMargin;
        cx -= __lvRowMargin*2;
    }

    UINT cx_col = cx / uColumnCount;

    for (UINT uIdx = 0; uRow < uRowCount; uRow++, uIdx++)
    {
        painter.setFont(this->font());
        painter.setPen(g_crFore);

        bool bSelected = (int)uRow == m_nSelectRow;
        tagLVRow lvRow(uIdx, uRow, 0, bSelected);
        QRect& rc = lvRow.rc;
        for (auto& uCol = lvRow.uCol; uCol < uColumnCount; uCol++)
        {
            rc.setRect(x + uCol*cx_col, y, cx_col, m_uRowHeight);

            if (bSelected)
            {
                QColor cr = g_crFore;
                cr.setAlpha(CPainter::oppTextAlpha(20));
                painter.fillRect(0, rc.top(), cx, m_uRowHeight, cr);
            }

            _onPaintRow(painter, lvRow);
        }

        y += m_uRowHeight;
        if (y >= cy)
        {
            break;
        }
    }

    if (E_LVScrollBar::LVSB_None != m_eScrollBar && uRowCount > uPageRowCount)
    {
        int szMargin = (__lvRowMargin-__cxBar)/2;
        int xBar = E_LVScrollBar::LVSB_Left == m_eScrollBar ? 0 : cx;
        xBar += szMargin;
        cy -= szMargin*2;
        auto cyBar = cy*uPageRowCount/uRowCount;
        m_yBar = szMargin + m_fScrollPos*(cy-cyBar)/(uRowCount-uPageRowCount);
        QRect rcBar(xBar, m_yBar, __cxBar, cyBar);

        auto cr = g_crFore;
        cr.setAlpha(128);
        painter.fillRectEx(rcBar, cr, __cxBar/2);
    }
}

void CListView::_paintRow(CPainter& painter, tagRowContext& context)
{
    cauto lvRow = context.lvRow;
    QRect rc = lvRow.rc;
    int cy = rc.height();

    if (context.pmImg && !context.pmImg->isNull())
    {
        UINT sz_icon = cy;
        if (context.fIconMargin > 0)
        {
            sz_icon -= UINT(sz_icon * context.fIconMargin * 2);
        }

        auto x_icon = rc.left();
        if (context.eStyle & E_RowStyle::IS_CenterAlign)
        {
            x_icon = rc.center().x()-sz_icon-__lvRowMargin;
        }

        int y_icon = rc.center().y()-sz_icon/2;
        QRect rcPixmap(x_icon, y_icon, sz_icon, sz_icon);
        painter.drawPixmapEx(rcPixmap, *context.pmImg, context.uIconRound);

        rc.setLeft(x_icon + sz_icon + __lvRowMargin);
    }

    if (context.eStyle & E_RowStyle::IS_BottomLine)
    {
        QColor cr = g_crFore;
        cr.setAlpha(CPainter::oppTextAlpha(20));
        painter.drawRectEx(QRect(rc.left(), rc.bottom(), rc.width(), 1), cr);
        rc.setBottom(rc.bottom()-3);
    }

    if (context.eStyle & E_RowStyle::IS_RightTip)
    {
        int sz_righttip = cy*22/100;
        int x_righttip = rc.right()-sz_righttip;
        int y_righttip = rc.center().y()-sz_righttip/2;
        QRect rcRighttip(x_righttip, y_righttip, sz_righttip, sz_righttip);
        painter.drawPixmap(rcRighttip, m_pmRightTip);

        rc.setRight(x_righttip - __lvRowMargin);
    }

    if (context->bSelected)
    {
        painter.adjustFont(QFont::Weight::DemiBold);
    }

    int flags = Qt::AlignLeft|Qt::AlignVCenter;
    if (context.eStyle & E_RowStyle::IS_MultiLine)
    {
        flags |= Qt::TextWrapAnywhere;
    }
    _paintText(context, painter, rc, flags, __ShadowAlpha, 255);
}

cqrc CListView::_paintText(tagRowContext& context, CPainter& painter, QRect& rc
                           , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    QString qsText = context.strText;
    if ((context.eStyle & E_RowStyle::IS_MultiLine) == 0)
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
    if (_checkBarArea(me.pos().x()))
    {
        if (getRowCount() > getPageRowCount())
        {
            return;
        }
    }

    if (E_MouseEventType::MET_Press == type)
    {
        if (m_uAutoScrollSeq > 0)
        {
            m_uAutoScrollSeq = 0;

            _onAutoScrollEnd();

            m_bClicking = false;
        }
    }
    else if (E_MouseEventType::MET_Click == type || E_MouseEventType::MET_DblClick == type)
    {
        tagLVRow lvRow;
        if (_hittest(me.pos().x(), me.pos().y(), lvRow))
        {
            if (E_MouseEventType::MET_Click == type)
            {
                _onRowClick(lvRow, me);
            }
            else
            {
                _onRowDblClick(lvRow, me);
            }
        }
        else
        {
            if (E_MouseEventType::MET_Click == type)
            {
                _onBlankClick(me);
            }
            else
            {
                _onBlankDblClick(me);
            }
        }
    }
}

bool CListView::_hittest(int x, int y, tagLVRow& lvRow)
{
    if (0 == m_uRowHeight)
    {
        return false;
    }

    UINT uRow = UINT((float)y/m_uRowHeight + m_fScrollPos);
    if (uRow >= getRowCount())
    {
        return false;
    }

    UINT cx_col = width() / getColumnCount();
    UINT uCol = UINT(x/cx_col);
    lvRow = tagLVRow(uRow-(UINT)m_fScrollPos, uRow, uCol, (int)uRow == m_nSelectRow);

    y = int(-(m_fScrollPos-uRow)*m_uRowHeight);
    lvRow.rc.setRect(uCol*cx_col, y, cx_col, m_uRowHeight);

    return true;
}

void CListView::_onTouchEvent(E_TouchEventType type, const CTouchEvent& te)
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
        auto uRowCount = getRowCount();
        auto uPageRowCount = getPageRowCount();
        if (uRowCount > uPageRowCount)
        {
            mtutil::yield();

            int cyBar = height()*uPageRowCount/uRowCount;
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
            auto fPos = float(uRowCount-uPageRowCount)*y/(height()-(__lvRowMargin-__cxBar)-cyBar);
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
        ulong dt = te.dt();
        if (0 == dt || dt > 500)
        {
            return;
        }

        int dy = te.dy();
        if (0 == dy) //abs(dy) < 3)
        {
            return;
        }
        mtutil::yield();

        if (dy > 0)
        {
            dy = 1;
        }
        else
        {
            dy = -1;
        }

        _onAutoScrollBegin();

        m_uAutoScrollSeq = te.timestamp();
        if (dt < 100)
        {
            _autoScroll(m_uAutoScrollSeq, m_uRowHeight*dy, 100, 3000);
        }
        else
        {
            _autoScroll(m_uAutoScrollSeq, m_uRowHeight/20*dy, 45, 3000);
        }
    }
}

bool CListView::_scroll(int dy)
{
    if (0 == m_uRowHeight)
    {
        return false;
    }

    m_fScrollPos -= (float)dy / m_uRowHeight;

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
