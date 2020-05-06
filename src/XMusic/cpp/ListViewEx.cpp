
#include "ListViewEx.h"

size_t CListViewEx::getRowCount() const
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
    _cleanup();

    update();

    _onShowRoot();
}

void CListViewEx::showMediaSet(CMediaSet& MediaSet)
{
    _cleanup();

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
    _cleanup();

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

void CListViewEx::_onPaintRow(CPainter& painter, tagLVItem& lvItem)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvItem.uRow, [&](CMediaSet& mediaSet) {
                tagMLItemContext context(lvItem, mediaSet);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvItem.uRow, [&](CMedia& media) {
                tagMLItemContext context(lvItem, media);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else if (m_pDir)
    {
        if (lvItem.uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(lvItem.uRow-m_paSubDirs.size(), [&](XFile& subFile) {
                tagMLItemContext context(lvItem, subFile);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else
        {
            m_paSubDirs.get(lvItem.uRow, [&](CPath& subPath) {
                tagMLItemContext context(lvItem, subPath);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else
    {
        tagMLItemContext context(lvItem);
        if (_genRootRowContext(context))
        {
            _paintRow(painter, context);
        }
    }
}

void CListViewEx::_onRowClick(tagLVItem& lvItem, const QMouseEvent& me)
{
    UINT uRow = lvItem.uRow;

    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(uRow, [&](CMediaSet& mediaSet){
                _onRowClick(lvItem, me, mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(uRow, [&](CMedia& media){
                _onRowClick(lvItem, me, media);
            });
        }
    }
    else if (m_pDir)
    {
        if (uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(uRow-m_paSubDirs.size(), [&](XFile& subFile) {
                _onRowClick(lvItem, me, (CPath&)subFile);
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
        tagMLItemContext context(lvItem);
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
