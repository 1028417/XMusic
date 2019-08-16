
#include "listview.h"

#include <QTimer>

void CListView::showItem(UINT uItem, bool bToTop)
{
    UINT uPageRowCount = getPageRowCount();
    if (uItem < m_fScrollPos)
    {
        m_fScrollPos = uItem;
    }
    else if (uItem+1 > m_fScrollPos+uPageRowCount)
    {
        m_fScrollPos = uItem+1-uPageRowCount;
    }
    else
    {
        if (bToTop)
        {
            m_fScrollPos = uItem;
        }
        else
        {
            return;
        }
    }

    update();
}

void CListView::flashItem(UINT uItem, UINT uMSDelay)
{
    m_nFlashRow = uItem;
    update();

    QTimer::singleShot(uMSDelay, [&](){
        m_nFlashRow = -1;
        update();
    });
}

void CListView::_onMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
    if (E_MouseEventType::MET_Click == type || E_MouseEventType::MET_DblClick == type)
    {
        if (0 != m_uRowHeight)
        {
            float fRowIdx = (float)me.pos().y()/m_uRowHeight + m_fScrollPos;

            if (E_MouseEventType::MET_Click == type)
            {
                _handleRowClick((UINT)fRowIdx, me);
            }
            else
            {
                _handleRowDblClick((UINT)fRowIdx, me);
            }
        }
    }
}

void CListView::_handleMouseEvent(E_MouseEventType type, const QMouseEvent& me)
{
    if (E_MouseEventType::MET_Press == type)
    {
        if (m_uAutoScrollSeq > 0)
        {
            m_uAutoScrollSeq = 0;

            _onAutoScrollEnd();

            return;
        }
    }

    CWidget<>::_handleMouseEvent(type, me);
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
                _autoScroll(m_uAutoScrollSeq, m_uRowHeight*dy, 50, 30000);
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

    if (0 == m_uColumnCount)
    {
        m_uColumnCount = 1;
    }
    UINT cx_col = cx / m_uColumnCount;

    for (UINT uIdx = 0; uRow < uRowCount; uRow++, uIdx++)
    {
        painter.setFont(this->font());
        painter.setPen(m_crText);

        tagListViewRow lvRow;
        lvRow.uRow = uRow;
        lvRow.bSelect = (int)uRow == m_nSelectRow;
        lvRow.bFlash = (int)uRow == m_nFlashRow;

        for (auto& uCol = lvRow.uCol; uCol < m_uColumnCount; uCol++)
        {
            QRect rc(uCol * cx_col, y, cx_col, m_uRowHeight);
            _onPaintItem(painter, rc, lvRow);
        }

        y += m_uRowHeight;
        if (y >= cy)
        {
            break;
        }
    }
}
