
#include "ListViewEx.h"

CListViewEx::CListViewEx(QWidget *parent, UINT uColumnCount, UINT uPageRowCount)
    : CListView(parent, uColumnCount, uPageRowCount)
{
    (void)m_pmRightTip.load(":/img/righttip.png");
}

UINT CListViewEx::getRowCount()
{
    if (m_pMediaset)
    {
          return m_lstSubSets.size() + m_lstSubMedias.size();
    }
    else if (m_pPath)
    {
        return m_pPath->GetSubPath().size();
    }
    else
    {
        return getRootCount();
    }
}

void CListViewEx::showRoot()
{
    m_pPath = NULL;

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    update(0, true);
}

void CListViewEx::showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem)
{
    m_pPath = NULL;

    m_pMediaset = &MediaSet;

    m_lstSubSets.clear();
    m_pMediaset->GetSubSets(m_lstSubSets);

    m_lstSubMedias.clear();
    m_pMediaset->GetMedias(m_lstSubMedias);

    if (pHittestItem)
    {
        int nIdx = pHittestItem->index();
        if (nIdx >= 0)
        {
            showRow((UINT)nIdx, true);
            selectRow((UINT)nIdx);
        }
    }
    else
    {
        update(_scrollRecord(), true);
    }
}

void CListViewEx::showPath(CPath& path)
{
    CPath *pHittestItem = NULL;
    if (path.IsDir())
    {
        m_pPath = &path;
    }
    else
    {
        auto pParent = path.parent();
        if (NULL == pParent)
        {
            return;
        }

        m_pPath = pParent;
        pHittestItem = &path;
    }

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    if (pHittestItem)
    {
        int nIdx = m_pPath->GetSubPath().indexOf(pHittestItem);
        if (nIdx >= 0)
        {
            showRow((UINT)nIdx, true);
            selectRow((UINT)nIdx);
        }
    }
    else
    {
        update(_scrollRecord(), true);
    }
}

void CListViewEx::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvRow.uRow, [&](CMediaSet& mediaSet){
                tagRowContext context(mediaSet);
                _genRowContext(context);
                _paintRow(painter, rc, lvRow, context);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvRow.uRow, [&](CMedia& media) {
                tagRowContext context(media);
                _genRowContext(context);
                _paintRow(painter, rc, lvRow, context);
            });
        }
    }
    else if (m_pPath)
    {
        m_pPath->GetSubPath().get(lvRow.uRow, [&](CPath& subPath) {
            tagRowContext context(subPath);
            _genRowContext(context);
            _paintRow(painter, rc, lvRow, context);
        });
    }
    else
    {
        tagRowContext context;
        if (_genRootRowContext(lvRow, context))
        {
            _paintRow(painter, rc, lvRow, context);
        }
    }
}

void CListViewEx::_paintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, const tagRowContext& context)
{
    UINT sz_icon = rc.height();
    if (context.uIconSize > 0 && context.uIconSize < sz_icon)
    {
        sz_icon = context.uIconSize;
    }
    else
    {
        sz_icon = sz_icon *65/100;
    }

    int nMargin = (rc.height()-sz_icon)/2;

    int x_icon = 0;
    if (E_RowStyle::IS_Normal == context.eStyle)
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
    QRect rcDst(x_icon, y_icon, sz_icon, sz_icon);
    if (context.pixmap && !context.pixmap->isNull())
    {
        painter.drawPixmapEx(rcDst, *context.pixmap);
    }

    rc.setLeft(x_icon + sz_icon + nMargin);

    if (context.eStyle != E_RowStyle::IS_Normal)
    {
        painter.fillRect(rc.left(), rc.bottom(), rc.width(), 1, QColor(255,255,255,128));

        if (E_RowStyle::IS_RightTip == context.eStyle)
        {
            int sz_righttip = sz_icon*30/100;
            int x_righttip = rc.right()-sz_righttip;
            int y_righttip = rc.center().y()-sz_righttip/2;

            painter.drawPixmap(x_righttip, y_righttip, sz_righttip, sz_righttip, m_pmRightTip);

            rc.setRight(x_righttip - nMargin);
        }
    }

    QString qsText = painter.fontMetrics(). elidedText(wsutil::toQStr(context.strText)
                            , Qt::ElideRight, rc.width(), Qt::TextShowMnemonic);
    painter.drawText(rc, Qt::AlignLeft|Qt::AlignVCenter, qsText);
}

void CListViewEx::_onRowClick(const tagLVRow& lvRow, const QMouseEvent&)
{
    UINT uRow = lvRow.uRow;

    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uRow, [&](CMediaSet& mediaSet){
                _saveScrollRecord();

                showMediaSet(mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uRow, [&](CMedia& media){
                _onRowClick(lvRow, media);
            });
        }
    }
    else if (m_pPath)
    {
        m_pPath->GetSubPath().get(uRow, [&](CPath& subPath) {
            if (subPath.IsDir())
            {
                _saveScrollRecord();

                showPath(subPath);
            }
            else
            {
                _onRowClick(lvRow, subPath);
            }
        });
    }
    else
    {
        _saveScrollRecord();

        tagRowContext context;
        if (_genRootRowContext(lvRow, context))
        {
            if (context.pMediaSet)
            {
                showMediaSet(*context.pMediaSet);
            }
            else if (context.pPath)
            {
                showPath(*context.pPath);
            }
        }
    }
}

float& CListViewEx::_scrollRecord()
{
    void *p = m_pMediaset;
    if (NULL == p)
    {
        p = m_pPath;
    }

    return m_mapScrollRecord[p];
}

void CListViewEx::_saveScrollRecord()
{
    _scrollRecord() = scrollPos();
}

void CListViewEx::_clearScrollRecord()
{
    if (m_pMediaset)
    {
        m_mapScrollRecord.erase(m_pMediaset);
    }
    else if (m_pPath)
    {
        m_mapScrollRecord.erase(m_pPath);
    }
}

void CListViewEx::upward()
{
    _clearScrollRecord();

    if (m_pMediaset)
    {
        if (m_pMediaset->m_pParent)
        {
            showMediaSet(*m_pMediaset->m_pParent);
        }
        else
        {
            showRoot();
        }
    }
    else if (m_pPath)
    {
        auto pParent = m_pPath->parent();
        if (pParent)
        {
            showPath(*pParent);
        }
        else
        {
            showRoot();
        }
    }
}
