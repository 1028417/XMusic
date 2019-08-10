
#include "listview.h"

void CListView::scroll(UINT uRow)
{
    if (uRow < m_fScrollPos)
    {
        m_fScrollPos = uRow;
    }
    else if (uRow+1 > m_fScrollPos+m_uRowCount)
    {
        m_fScrollPos = uRow+1-m_uRowCount;
    }
}

void CListView::_handleMouseEvent(E_MouseEventType type, QMouseEvent& ev)
{
    if (E_MouseEventType::MET_Click == type || E_MouseEventType::MET_DblClick == type)
    {
        if (0 != m_uRowHeight)
        {
            float fRowIdx = (float)ev.pos().y()/m_uRowHeight + m_fScrollPos;

            if (E_MouseEventType::MET_Click == type)
            {
                _handleRowClick((UINT)fRowIdx, ev);
            }
            else
            {
                _handleRowDblClick((UINT)fRowIdx, ev);
            }
        }
    }
}

void CListView::_onTouchMove(int dy)
{
    if (0 == m_uRowHeight)
    {
        return;
    }

    m_fScrollPos -= (float)dy / m_uRowHeight;
    if (dy < 0)
    {
        m_fScrollPos = MIN(m_fScrollPos, m_uMaxScrollPos);
    }
    else
    {
        m_fScrollPos = MAX(0, m_fScrollPos);
    }

    this->update();
}

void CListView::_onPaint(QPainter& painter, const QRect&)
{
    m_uRowHeight = 0;

    UINT uItemCount = getItemCount();
    UINT uRowCount = getRowCount();
    if (0 == uRowCount)
    {
        if (0 == uItemCount)
        {
            return;
        }

        uRowCount = uItemCount;
    }

    int cy = rect().bottom();
    m_uRowHeight = cy/uRowCount;

    if (uItemCount > uRowCount)
    {
        m_uMaxScrollPos = uItemCount - uRowCount;
    }
    else
    {
        m_uMaxScrollPos = 0;
    }

    m_fScrollPos = MIN(m_fScrollPos, m_uMaxScrollPos);

    UINT uItem = m_fScrollPos;
    int y = (-m_fScrollPos+uItem)*m_uRowHeight;
    int cx = rect().right();

    for (UINT uRowIdx = 0; uItem < uItemCount; uItem++, uRowIdx++)
    {
        painter.setFont(this->font());
        painter.setPen(m_crText);

        QRect rcItem(0, y, cx, m_uRowHeight);
        _onPaintItem(painter, uItem, rcItem);

        y += m_uRowHeight;
        if (y >= cy)
        {
            break;
        }
    }
}
