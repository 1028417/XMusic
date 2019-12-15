
#include "listview.h"

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

    timerutil::async(uMSDelay, [&](){
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

void CListView::_onPaint(CPainter& painter, int cx, int cy)
{
    size_t uPageRowCount = getPageRowCount();
    if (0 == uPageRowCount)
    {
        return;
        //uPageRowCount = uRowCount;
    }

    m_uRowHeight = cy/uPageRowCount;

    UINT uRowCount = getRowCount();
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

    UINT uColumnCount = getColumnCount();
    if (0 == uColumnCount)
    {
        uColumnCount = 1;
    }
    UINT cx_col = cx / uColumnCount;

    for (UINT uIdx = 0; uRow < uRowCount; uRow++, uIdx++)
    {
        painter.setFont(this->font());
        painter.setPen(g_crText);

        tagLVRow lvRow(uIdx, uRow, 0, (int)uRow == m_nSelectRow, (int)uRow == m_nFlashRow);
        QRect& rc = lvRow.rc;
        for (auto& uCol = lvRow.uCol; uCol < uColumnCount; uCol++)
        {
            rc.setRect(uCol * cx_col, y, cx_col, m_uRowHeight);

            if (lvRow.bSelect)
            {
                painter.fillRect(rc.left(), rc.top(), rc.width(), rc.height()-1, m_crSelectedBkg);
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

void CListView::_paintRow(CPainter& painter, const tagLVRow& lvRow, const tagRowContext& context)
{
    QRect rc = lvRow.rc;

    if (lvRow.bFlash)
    {
        QColor crFlashText(g_crText);
        crFlashText.setAlpha(220);
        painter.setPen(crFlashText);
    }

    if (context.pixmap && !context.pixmap->isNull())
    {
        UINT sz_icon = rc.height();
        if (context.fIconMargin > 0)
        {
            sz_icon -= UINT(sz_icon * context.fIconMargin * 2);
        }

        int nMargin = (rc.height()-sz_icon)/2;

        int x_icon = 0;
        if (context.eStyle & E_RowStyle::IS_CenterAlign)
        {
            x_icon = rc.center().x()-sz_icon;

            rc.setLeft(x_icon + sz_icon + nMargin/2);
        }
        else
        {
            x_icon = rc.left() + nMargin;

            rc.setRight(rc.right() - nMargin);
        }

        int y_icon = rc.center().y()-sz_icon/2;
        QRect rcPixmap(x_icon, y_icon, sz_icon, sz_icon);
        painter.drawPixmapEx(rcPixmap, *context.pixmap);

        rc.setLeft(x_icon + sz_icon + nMargin);
    }

    if (context.eStyle & E_RowStyle::IS_BottomLine)
    {
        painter.fillRect(rc.left(), rc.bottom(), rc.width()-3, 1, QColor(255,255,255,70));
    }

    if (context.eStyle & E_RowStyle::IS_RightTip)
    {
        int sz_righttip = rc.height()*22/100;
        int x_righttip = rc.right()-sz_righttip;
        int y_righttip = rc.center().y()-sz_righttip/2;
        painter.drawPixmap(x_righttip, y_righttip, sz_righttip, sz_righttip, m_pmRightTip);

        rc.setRight(x_righttip);
    }

    QString qsText = strutil::toQstr(context.strText);
    if (context.eStyle & E_RowStyle::IS_MultiLine)
    {
        painter.drawText(rc, Qt::AlignLeft|Qt::AlignVCenter|Qt::TextWrapAnywhere, qsText);
    }
    else
    {
        int nTextFlag = Qt::TextShowMnemonic | Qt::TextSingleLine;
        qsText = painter.fontMetrics().elidedText(qsText, Qt::ElideRight, rc.width(), nTextFlag);
        painter.drawText(rc, Qt::AlignLeft|Qt::AlignVCenter, qsText);
    }
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
            _autoScroll(m_uAutoScrollSeq, 10*dy, 45, 3000);
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

void CListView::_autoScroll(ulong uSeq, int dy, ulong dt, ulong total)
{
    timerutil::async(dt, [=]() {
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
