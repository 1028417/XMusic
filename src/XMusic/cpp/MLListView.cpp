
#include "MLListView.h"

size_t CMLListView::getItemCount() const
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
        return _getRootItemCount();
    }
}

void CMLListView::showRoot()
{
    _cleanup();

    update();

    _onShowRoot();
}

void CMLListView::showMediaSet(CMediaSet& MediaSet)
{
    _cleanup();

    m_pMediaset = &MediaSet;
    m_pMediaset->GetSubSets(m_lstSubSets);
    m_pMediaset->GetMedias(m_lstSubMedias);

    update();

    _onShowMediaSet(MediaSet);
}

void CMLListView::hittestMedia(CMedia& media)
{
    if (media.m_pParent)
    {
        showMediaSet(*media.m_pParent);

        int nIdx = media.index();
        if (nIdx >= 0)
        {
            showItemCenter((UINT)nIdx);
            selectItem((UINT)nIdx);
        }
    }
}

void CMLListView::showDir(CPath& dir)
{
    _cleanup();

    m_pDir = &dir;
    m_paSubDirs.assign(dir.dirs());
    m_paSubFiles.assign(dir.files());

    update();

    _onShowDir(dir);
}

void CMLListView::hittestFile(XFile& file)
{
    auto pParent = file.parent();
    if (pParent)
    {
        showDir(*pParent);

        int nIdx = pParent->files().indexOf(&file);
        if (nIdx >= 0)
        {
            auto uItem = pParent->dirs().size() + nIdx;
            showItemCenter(uItem);
            selectItem(uItem);
        }
    }
}

void CMLListView::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvItem.uItem, [&](CMediaSet& mediaSet) {
                tagMLItemContext context(lvItem, mediaSet);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvItem.uItem, [&](CMedia& media) {
                tagMLItemContext context(lvItem, media);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else if (m_pDir)
    {
        if (lvItem.uItem >= m_paSubDirs.size())
        {
            m_paSubFiles.get(lvItem.uItem-m_paSubDirs.size(), [&](XFile& subFile) {
                tagMLItemContext context(lvItem, subFile);
                _genMediaContext(context);
                _paintRow(painter, context);
            });
        }
        else
        {
            m_paSubDirs.get(lvItem.uItem, [&](CPath& subPath) {
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

void CMLListView::_onRowClick(tagLVItem& lvItem, const QMouseEvent& me)
{
    if (m_pMediaset)
    {
        if (m_lstSubSets)
        {
            m_lstSubSets.get(lvItem.uItem, [&](CMediaSet& mediaSet){
                _onRowClick(lvItem, me, mediaSet);
            });
        }
        else if (m_lstSubMedias)
        {
            m_lstSubMedias.get(lvItem.uItem, [&](CMedia& media){
                _onRowClick(lvItem, me, media);
            });
        }
    }
    else if (m_pDir)
    {
        if (lvItem.uRow >= m_paSubDirs.size())
        {
            m_paSubFiles.get(lvItem.uItem-m_paSubDirs.size(), [&](XFile& subFile) {
                _onRowClick(lvItem, me, (CPath&)subFile);
            });
        }
        else
        {
            m_paSubDirs.get(lvItem.uItem, [&](CPath& subPath) {
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

bool CMLListView::upward()
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
