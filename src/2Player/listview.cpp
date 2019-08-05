
#include "listview.h"

void CListView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    QWidget::mouseDoubleClickEvent(ev);

    float fRowIdx = (float)ev->pos().y()/m_uRowHeight + m_fScrollPos;
    _handleMouseDoubleClick((UINT)fRowIdx);
}

void CListView::_onTouchMove(int dy)
{
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

void CListView::_onPaint(QPainter& painter, const QRect& rcPos)
{
    if (0 == m_uRowCount)
    {
        return;
    }
    m_uRowHeight = rcPos.height()/m_uRowCount;

    UINT uItemCount = getItemCount();
    if (uItemCount > m_uRowCount)
    {
        m_uMaxScrollPos = uItemCount - m_uRowCount;
    }
    else
    {
        m_uMaxScrollPos = 0;
    }

    m_fScrollPos = MIN(m_fScrollPos, m_uMaxScrollPos);

    painter.setPen(m_crText);

    UINT uItem = m_fScrollPos;
    int y = rcPos.top() + (-m_fScrollPos+uItem)*m_uRowHeight;
    for (UINT uRowIdx = 0; uItem < uItemCount; uItem++, uRowIdx++)
    {
        QRect rcItem(0, y, rcPos.width(), m_uRowHeight);

        _onPaintItem(painter, uItem, rcItem);

        y += m_uRowHeight;
        if (y >= rcPos.bottom())
        {
            break;
        }
    }
}
