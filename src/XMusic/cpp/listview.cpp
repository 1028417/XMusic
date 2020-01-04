
#include "listview.h"

#include "app.h"

#define __FlashingAlpha 170

void CListView::showRow(UINT uRow, bool bToCenter)
{
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

void CListView::flashRow(UINT uRow, UINT uMSDelay)
{
    m_nFlashRow = uRow;
    update();

    CApp::async(uMSDelay, [&](){
        m_nFlashRow = -1;
        update();
    });
}

void CListView::_onPaint(CPainter& painter, const QRect&)
{
    int cx = width();
    int cy = height();
    _onPaint(painter, cx, cy);
}

#define __OppAlpha(f) (255-(255*pow(__ColorOffsetAvg(g_crText, g_crTheme)/255.0, f)))

UINT CListView::oppTextAlpha(UINT uMinAlpha, float fPow)
{
    UINT uOppAlpha = __OppAlpha(fPow);
    return MAX(uMinAlpha, uOppAlpha);
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
    UINT cx_col = cx / uColumnCount;

    for (UINT uIdx = 0; uRow < uRowCount; uRow++, uIdx++)
    {
        painter.setFont(this->font());
        painter.setPen(g_crText);

        bool bSelected = (int)uRow == m_nSelectRow;
        tagLVRow lvRow(uIdx, uRow, 0, bSelected, (int)uRow == m_nFlashRow);
        QRect& rc = lvRow.rc;
        for (auto& uCol = lvRow.uCol; uCol < uColumnCount; uCol++)
        {
            rc.setRect(uCol * cx_col, y, cx_col, m_uRowHeight);

            if (bSelected)
            {
                UINT uAlpha = oppTextAlpha(20);
                QColor cr = g_crText;
                cr.setAlpha(uAlpha);
                painter.fillRect(rc.left(), rc.top(), rc.width(), m_uRowHeight-1, cr);
            }

            _onPaintRow(painter, lvRow);
        }

        y += m_uRowHeight;
        if (y >= cy)
        {
            break;
        }
    }
}

void CListView::_paintRow(CPainter& painter, const tagRowContext& context)
{
    cauto lvRow = context.lvRow;
    QRect rc = lvRow.rc;
    int cy = rc.height();

    int nMargin = __size(40);
    if (context.pixmap && !context.pixmap->isNull())
    {
        UINT sz_icon = cy;
        if (context.fIconMargin > 0)
        {
            sz_icon -= UINT(sz_icon * context.fIconMargin * 2);
        }

        //nMargin = (cy-sz_icon)/2;

        int x_icon = 0;
        if (context.eStyle & E_RowStyle::IS_CenterAlign)
        {
            x_icon = rc.center().x()-sz_icon;

            rc.setLeft(x_icon + sz_icon + nMargin/2);
        }
        else
        {
            x_icon = rc.left() + nMargin;
        }

        int y_icon = rc.center().y()-sz_icon/2;
        QRect rcPixmap(x_icon, y_icon, sz_icon, sz_icon);
        painter.drawPixmapEx(rcPixmap, *context.pixmap, 5);

        rc.setLeft(x_icon + sz_icon + nMargin);

        //nMargin += __size(3);
    }
    else
    {
        //nMargin = cy/5;

        rc.setLeft(nMargin);
    }

    if (context.eStyle & E_RowStyle::IS_BottomLine)
    {
        UINT uAlpha = oppTextAlpha(20);
        QColor cr = g_crText;
        cr.setAlpha(uAlpha*2);
        painter.fillRect(rc.left(), rc.bottom(), rc.width()-nMargin, 1, cr);
    }

    if (context.eStyle & E_RowStyle::IS_RightTip)
    {
        rc.setRight(rc.right() - nMargin);

        int sz_righttip = cy*22/100;
        int x_righttip = rc.right()-sz_righttip;
        int y_righttip = rc.center().y()-sz_righttip/2;
        QRect rcRighttip(x_righttip, y_righttip, sz_righttip, sz_righttip);
        painter.drawPixmap(rcRighttip, m_pmRightTip);

        rc.setRight(x_righttip - nMargin);
    }

    _paintText(painter, rc, context);
}

void CListView::_paintText(CPainter& painter, QRect& rc, const tagRowContext& context)
{
    if (context.lvRow.bSelected)
    {
        auto font = painter.font();
        font.setBold(true);
        painter.setFont(font);
    }

    UINT uTextAlpha = 255;
    UINT uShadowAlpha = __ShadowAlpha;
    if (context.lvRow.bFlashing)
    {
        uTextAlpha = __FlashingAlpha;
        uShadowAlpha = uShadowAlpha*__FlashingAlpha/300;
    }

    QString qsText = strutil::toQstr(context.strText);
    int flags = Qt::AlignLeft|Qt::AlignVCenter;
    if (context.eStyle & E_RowStyle::IS_MultiLine)
    {
        flags |= Qt::TextWrapAnywhere;
    }
    else
    {
        int nTextFlag = Qt::TextShowMnemonic | Qt::TextSingleLine;
        qsText = painter.fontMetrics().elidedText(qsText, Qt::ElideRight, rc.width(), nTextFlag);
    }

    painter.drawTextEx(rc, flags, qsText, 1, uShadowAlpha, uTextAlpha);
}

void CListView::_onMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
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
    lvRow = tagLVRow(uRow-(UINT)m_fScrollPos, uRow, uCol, (int)uRow == m_nSelectRow, (int)uRow == m_nFlashRow);

    y = int(-(m_fScrollPos-uRow)*m_uRowHeight);
    lvRow.rc.setRect(uCol*cx_col, y, cx_col, m_uRowHeight);

    return true;
}

void CListView::_onTouchEvent(E_TouchEventType type, const CTouchEvent& te)
{
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
        if (abs(dy) < 3)
        {
            return;
        }

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
