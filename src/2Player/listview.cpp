
#include "listview.h"

#include <QTimer>

void CListView::showRow(UINT uRow, bool bToCenter)
{
    UINT uPageRowCount = getPageRowCount();

    if (bToCenter)
    {
        m_fScrollPos = MAX(.0f, (float)uRow - uPageRowCount/2);
    }
    else  if (uRow < m_fScrollPos)
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

    update();
}

void CListView::flashRow(UINT uRow, UINT uMSDelay)
{
    m_nFlashRow = uRow;
    update();

    QTimer::singleShot(uMSDelay, [&](){
        m_nFlashRow = -1;
        update();
    });
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
        if (0 != m_uRowHeight)
        {
            UINT uRow = UINT((float)me.pos().y()/m_uRowHeight + m_fScrollPos);
            UINT cx_col = width() / getColumnCount();
            UINT uCol = UINT(me.pos().x()/cx_col);
            tagListViewRow lvRow(uRow, uCol, (int)uRow == m_nSelectRow, (int)uRow == m_nFlashRow);

            if (E_MouseEventType::MET_Click == type)
            {
                _handleRowClick(lvRow, me);
            }
            else
            {
                _handleRowDblClick(lvRow, me);
            }
        }
    }
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
        if (dt > 0 && dt < 600)
        {
            int dy = te.dy();
            if (0 == dy)
            {
                return;
            }
            else if (dy > 0)
            {
                dy = 1;
            }
            else
            {
                dy = -1;
            }

            _onAutoScrollBegin();

            m_uAutoScrollSeq = te.timestamp();
            if (dt < 200)
            {
                _autoScroll(m_uAutoScrollSeq, m_uRowHeight*dy, 100, 30000);
            }
            else
            {
                _autoScroll(m_uAutoScrollSeq, 10*dy, 50, 30000);
            }
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
    QTimer::singleShot(dt, [=]() {
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

void CListView::_onPaint(CPainter& painter, const QRect&)
{
    int cx = width();
    int cy = height();

    UINT uRowCount = getRowCount();

    UINT uPageRowCount = getPageRowCount();
    if (0 == uPageRowCount)
    {
        uPageRowCount = MAX(1, uRowCount);
    }

    m_uRowHeight = cy/uPageRowCount;

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
    int y = (-m_fScrollPos+uRow)*m_uRowHeight;

    UINT uColumnCount = getColumnCount();
    if (0 == uColumnCount)
    {
        uColumnCount = 1;
    }
    UINT cx_col = cx / uColumnCount;

    for (UINT uIdx = 0; uRow < uRowCount; uRow++, uIdx++)
    {
        painter.setFont(this->font());
        painter.setPen(m_crText);

        tagListViewRow lvRow(uRow, 0, (int)uRow == m_nSelectRow, (int)uRow == m_nFlashRow);
        lvRow.uDislpayRow = uIdx;
        for (auto& uCol = lvRow.uCol; uCol < uColumnCount; uCol++)
        {
            QRect rc(uCol * cx_col, y, cx_col, m_uRowHeight);
            _onPaintRow(painter, rc, lvRow);
        }

        y += m_uRowHeight;
        if (y >= cy)
        {
            break;
        }
    }
}
