
#include "listview.h"

#include <QTimer>

void CListView::selectItem(UINT uItem)
{
    showItem(uItem);

    m_nSelectItem = uItem;
    update();
}

void CListView::dselectItem()
{
    m_nSelectItem = -1;
    update();
}

void CListView::showItem(UINT uItem, bool bToTop)
{
    UINT uRowCount = getRowCount();
    if (uItem < m_fScrollPos)
    {
        m_fScrollPos = uItem;
    }
    else if (uItem+1 > m_fScrollPos+uRowCount)
    {
        m_fScrollPos = uItem+1-uRowCount;
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
    m_nFlashItem = uItem;
    update();

    QTimer::singleShot(uMSDelay, [&](){
        m_nFlashItem = -1;
        update();
    });
}

void CListView::_onMouseEvent(E_MouseEventType type, QMouseEvent& ev)
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

void CListView::_onTouchMove(int dx, int dy)
{
    (void)dx;

    _scroll(dy);
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

    this->update();

    return bFlag;
}

void CListView::_onTouchSwipe(ulong dt, int dx, int dy)
{
    if (dy < 0)
    {
        dy = -m_uRowHeight;
    }
    else if (dy > 0)
    {
        dy = m_uRowHeight;
    }
    else
    {
        return;
    }

    _scrollEx(dy, m_uRowCount);
}

void CListView::_scrollEx(int dy, UINT uCount)
{
    if (uCount > 0)
    {
        if (_scroll(dy))
        {
            return;
        }

        uCount--;
        if (uCount > 0)
        {
            QTimer::singleShot(100, [=](){
                _scrollEx(dy, uCount);
            });
        }
    }
}

void CListView::_onPaint(CPainter& painter, const QRect&)
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

        QRect rc(0, y, cx, m_uRowHeight);
        tagListViewItem item;
        item.uItem = uItem;
        item.uRow = uRowIdx;
        item.bSelect = (int)uItem == m_nSelectItem;
        item.bFlash = (int)uItem == m_nFlashItem;
        _onPaintItem(painter, rc, item);

        y += m_uRowHeight;
        if (y >= cy)
        {
            break;
        }
    }
}
