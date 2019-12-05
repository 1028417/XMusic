
#include "ListViewEx.h"

CListViewEx::CListViewEx(QWidget *parent, UINT uColumnCount, UINT uPageRowCount)
    : CListView(parent, uColumnCount, uPageRowCount)
{
}

size_t CListViewEx::getRowCount()
{
    if (m_pMediaset)
    {
          return m_lstSubSets.size() + m_lstSubMedias.size();
    }
    else if (m_pPath)
    {
        return m_paSubDirs.size() + m_paSubFiles.size();
    }
    else
    {
        return _getRootRowCount();
    }
}

void CListViewEx::showRoot()
{
    _clear();

    update();

    _onShowRoot();
}

void CListViewEx::showMediaSet(CMediaSet& MediaSet)
{
    _clear();

    m_pMediaset = &MediaSet;
    m_pMediaset->GetSubSets(m_lstSubSets);
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

    _clear();

    m_pPath = &path;
    m_paSubDirs.assign(path.dirs());
    m_paSubFiles.assign(path.files());

    update();

    _onShowPath(path);
}

void CListViewEx::_onPaintRow(CPainter& painter, const tagLVRow& lvRow)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvRow.uRow, [&](CMediaSet& mediaSet) {
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
        if (lvRow.uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(lvRow.uRow-m_paSubDirs.size(), [&](XFile& subFile) {
                tagMediaContext context(subFile);
                _genMediaContext(context);
                _paintRow(painter, lvRow, context);
            });
        }
        else
        {
            m_paSubDirs.get(lvRow.uRow, [&](CPath& subPath) {
                tagMediaContext context(subPath);
                _genMediaContext(context);
                _paintRow(painter, lvRow, context);
            });
        }
    }
    else
    {
        tagMediaContext context;
        if (_genRootRowContext(lvRow, context))
        {
            _paintRow(painter, lvRow, context);
        }
    }
}

void CListViewEx::_onRowClick(const tagLVRow& lvRow, const QMouseEvent& me)
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
                _onRowClick(lvRow, me, media);
            });
        }
    }
    else if (m_pPath)
    {
        if (uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(uRow-m_paSubDirs.size(), [&](XFile& subFile) {
                _onRowClick(lvRow, me, (CPath&)subFile);
            });
        }
        else
        {
            m_paSubDirs.get(uRow, [&](CPath& subPath) {
                _saveScrollRecord();
                showPath(subPath);
            });
        }
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
            else if (context.pDir)
            {
                _saveScrollRecord();
                showPath(*context.pDir);
            }
        }
    }
}

bool CListViewEx::upward()
{
    _clearScrollRecord(_current());

    if (m_pMediaset)
    {
        auto parent = _onUpward(*m_pMediaset);
        if (parent)
        {
            showMediaSet(*parent);
        }
        else
        {
            showRoot();
        }
    }
    else if (m_pPath)
    {
        auto parent = _onUpward(*m_pPath);
        if (parent)
        {
            showPath(*parent);
        }
        else
        {
            showRoot();
        }
    }
    else
    {
        return false;
    }

    scroll(_scrollRecord(_current()));
    return true;
}
