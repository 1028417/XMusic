
#include "ListViewEx.h"

CListViewEx::CListViewEx(QWidget *parent, UINT uColumnCount, UINT uPageRowCount)
    : CListView(parent, uColumnCount, uPageRowCount)
{
}

UINT CListViewEx::getRowCount()
{
    if (m_pMediaset)
    {
          return m_lstSubSets.size() + m_lstSubMedias.size();
    }
    else if (m_pPath)
    {
        return m_paSubPath.size();
    }
    else
    {
        return getRootCount();
    }
}

void CListViewEx::showRoot()
{
    m_pPath = NULL;
    m_paSubPath.clear();

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    update();

    _onShowRoot();
}

void CListViewEx::showMediaSet(CMediaSet& MediaSet)
{
    m_pPath = NULL;
    m_paSubPath.clear();

    m_pMediaset = &MediaSet;

    m_lstSubSets.clear();
    m_pMediaset->GetSubSets(m_lstSubSets);

    m_lstSubMedias.clear();
    m_pMediaset->GetMedias(m_lstSubMedias);

    update();

    _onShowMediaSet(MediaSet);
}

void CListViewEx::showMedia(CMedia& media)
{
    if (media.m_pParent)
    {
        showMediaSet(*media.m_pParent);

        int nIdx = media.index();
        if (nIdx >= 0)
        {
            showRow((UINT)nIdx, true);
            selectRow((UINT)nIdx);
        }
    }
}

void CListViewEx::showPath(CPath& path)
{
    if (!path.fileInfo().bDir)
    {
        auto pParent = path.fileInfo().pParent;
        if (pParent)
        {
            showPath(*pParent);

            int nIdx = pParent->files().indexOf(&path);
            if (nIdx >= 0)
            {
                UINT uIdx = pParent->dirs().size() + (UINT)nIdx;
                showRow(uIdx, true);
                selectRow(uIdx);
            }
        }

        return;
    }

    m_pPath = &path;
    m_paSubPath.assign(path.dirs());
    m_paSubPath.add(path.files());

    m_pMediaset = NULL;
    m_lstSubSets.clear();
    m_lstSubMedias.clear();

    update();

    _onShowPath(path);
}

void CListViewEx::_onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvRow.uRow, [&](CMediaSet& mediaSet){
                tagMediaContext context(mediaSet);
                _genRowContext(context);
                _paintRow(painter, rc, lvRow, context);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvRow.uRow, [&](CMedia& media) {
                tagMediaContext context(media);
                _genRowContext(context);
                _paintRow(painter, rc, lvRow, context);
            });
        }
    }
    else if (m_pPath)
    {
        m_paSubPath.get(lvRow.uRow, [&](CPath& subPath) {
            tagMediaContext context(subPath);
            _genRowContext(context);
            _paintRow(painter, rc, lvRow, context);
        });
    }
    else
    {
        tagMediaContext context;
        if (_genRootRowContext(lvRow, context))
        {
            _paintRow(painter, rc, lvRow, context);
        }
    }
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
        m_paSubPath.get(uRow, [&](CPath& subPath) {
            if (subPath.fileInfo().bDir)
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
        tagMediaContext context;
        if (_genRootRowContext(lvRow, context))
        {
            if (context.pMediaSet)
            {
                _saveScrollRecord();
                showMediaSet(*context.pMediaSet);
            }
            else if (context.pPath)
            {
                _saveScrollRecord();
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

    reset();
}

void CListViewEx::_clearScrollRecord()
{
    void *p = m_pMediaset;
    if (NULL == p)
    {
        p = m_pPath;
    }
    m_mapScrollRecord.erase(p);

    reset();
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
        auto pParent = m_pPath->fileInfo().pParent;
        if (pParent)
        {
            showPath(*pParent);
        }
        else
        {
            showRoot();
        }
    }
    else
    {
        return;
    }

    update(_scrollRecord());
}
