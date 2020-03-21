
#include "ListViewEx.h"

size_t CListViewEx::getRowCount()
{
    if (m_pMediaset)
    {
          return m_lstSubSets.size() + m_lstSubMedias.size();
    }
    else if (m_pDir)
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

void CListViewEx::hittestMedia(CMedia& media)
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

void CListViewEx::showDir(CPath& dir)
{
    _clear();

    m_pDir = &dir;
    m_paSubDirs.assign(dir.dirs());
    m_paSubFiles.assign(dir.files());

    update();

    _onShowDir(dir);
}

void CListViewEx::hittestFile(XFile& file)
{
    auto pParent = file.parent();
    if (pParent)
    {
        showDir(*pParent);

        int nIdx = pParent->files().indexOf(&file);
        if (nIdx >= 0)
        {
            UINT uIdx = UINT(pParent->dirs().size() + nIdx);
            showRow(uIdx, true);
            selectRow(uIdx);
        }
    }
}

void CListViewEx::_onPaintRow(CPainter& painter, tagLVRow& lvRow)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvRow.uRow, [&](CMediaSet& mediaSet) {
                tagMediaContext context(lvRow, mediaSet);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvRow.uRow, [&](CMedia& media) {
                tagMediaContext context(lvRow, media);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else if (m_pDir)
    {
        if (lvRow.uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(lvRow.uRow-m_paSubDirs.size(), [&](XFile& subFile) {
                tagMediaContext context(lvRow, subFile);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else
        {
            m_paSubDirs.get(lvRow.uRow, [&](CPath& subPath) {
                tagMediaContext context(lvRow, subPath);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else
    {
        tagMediaContext context(lvRow);
        if (_genRootRowContext(context))
        {
            _paintRow(painter, context);
        }
    }
}

void CListViewEx::_onRowClick(tagLVRow& lvRow, const QMouseEvent& me)
{
    UINT uRow = lvRow.uRow;

    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uRow, [&](CMediaSet& mediaSet){
                _onRowClick(lvRow, me, mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uRow, [&](CMedia& media){
                _onRowClick(lvRow, me, media);
            });
        }
    }
    else if (m_pDir)
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
                showDir(subPath);
            });
        }
    }
    else
    {
        tagMediaContext context(lvRow);
        if (_genRootRowContext(context))
        {
            if (context.pMediaSet)
            {
                _saveScrollRecord();
                showMediaSet(*context.pMediaSet);
            }
            else if (context.pDir)
            {
                _saveScrollRecord();
                showDir(*context.pDir);
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
    else if (m_pDir)
    {
        auto parent = _onUpward(*m_pDir);
        if (parent)
        {
            showDir(*parent);
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
