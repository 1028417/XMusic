
#include "listview.h"

void CListView::mouseDoubleClickEvent(QMouseEvent *ev)
{
    QWidget::mouseDoubleClickEvent(ev);

    if (0 == m_uRowHeight)
    {
        return;
    }

    float fRowIdx = (float)ev->pos().y()/m_uRowHeight + m_fScrollPos;
    _handleMouseDoubleClick((UINT)fRowIdx);
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
