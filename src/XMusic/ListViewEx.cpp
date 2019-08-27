
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

void CListViewEx::_onPaintRow(CPainter& painter, const tagLVRow& lvRow)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvRow.uRow, [&](CMediaSet& mediaSet){
                tagMediaContext context(mediaSet);
                _genMediaContext(context);
                _paintRow(painter, lvRow, context);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvRow.uRow, [&](CMedia& media) {
                tagMediaContext context(media);
                _genMediaContext(context);
                _paintRow(painter, lvRow, context);
            });
        }
    }
    else if (m_pPath)
    {
        m_paSubPath.get(lvRow.uRow, [&](CPath& subPath) {
            tagMediaContext context(subPath);
            _genMediaContext(context);
            _paintRow(painter, lvRow, context);
        });
    }
    else
    {
        tagMediaContext context;
        if (_genRootRowContext(lvRow, context))
        {
            if (context.pMediaSet)
            {
                m_setRootObject.add(context.pMediaSet);
            }
            else if (context.pPath)
            {
                m_setRootObject.add(context.pPath);
            }

            _paintRow(painter, lvRow, context);
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

bool CListViewEx::_onUpward()
{
    if (m_pMediaset)
    {
        if (m_setRootObject.includes(m_pMediaset))
        {
            showRoot();
            return true;
        }

        if (m_pMediaset->m_pParent)
        {
            showMediaSet(*m_pMediaset->m_pParent);
        }
    }
    else if (m_pPath)
    {
        if (m_setRootObject.includes(m_pPath))
        {
            showRoot();
            return true;
        }

        auto pParent = m_pPath->fileInfo().pParent;
        if (pParent)
        {
            showPath(*pParent);
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool CListViewEx::upward()
{
    _clearScrollRecord(_current());

    if (_onUpward())
    {
        scroll(_scrollRecord(_current()));
        return true;
    }

    return false;
}
