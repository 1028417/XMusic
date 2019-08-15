
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

ulong g_uTouchTime = 0;

void CListView::_onTouchEvent(E_TouchEventType type, const CTouchEvent& te)
{
    if (E_TouchEventType::TET_TouchMove == type)
    {
        _scroll(te.dy());
    }
    else if (E_TouchEventType::TET_TouchBegin == type)
    {
        g_uTouchTime = te.timestamp();
    }
    else if (E_TouchEventType::TET_TouchEnd == type)
    {
        ulong dt = te.dt();
        if (dt > 0)
        {
            int dy = te.dy();
            if (0 == dy)
            {
                return;
            }

            if (dy > 0)
            {
                dy = 1;
            }
            else if (dy < 0)
            {
                dy = -1;
            }
            dy *= m_uRowHeight/20;

            if (dt < 500)
            {
                if (dt < 100)
                {
                    dy *= 10;
                }

                _scrollEx(dy, 50, 3000);
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

    this->update();

    return bFlag;
}

void CListView::_scrollEx(int dy, ulong dt, ulong total)
{
    ulong uTouchTime = g_uTouchTime;
    QTimer::singleShot(dt, [=](){
        if (uTouchTime != g_uTouchTime)
        {
            return;
        }

        if (_scroll(dy))
        {
            return;
        }

        if (total >= dt*2)
        {
            _scrollEx(dy, dt, total-dt);
        }
    });
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
