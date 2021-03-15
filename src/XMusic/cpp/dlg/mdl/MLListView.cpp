
#include "xmusic.h"

#include "MLListView.h"

size_t CMLListView::getItemCount() const
{
    if (m_pMediaset)
    {
        return m_lstSubSets.size() + m_lstSubMedias.size();
    }
    else if (m_pDir)
    {
        return m_pDir->dirs().size() + m_pDir->files().size();
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

void CMLListView::_showMediaSet(CMediaSet& MediaSet)
{
    _cleanup();

    m_pMediaset = &MediaSet;
    m_pMediaset->GetSubSets(m_lstSubSets);
    m_pMediaset->GetMedias(m_lstSubMedias);

    update();

    _onShowMediaSet(MediaSet);
}

void CMLListView::showMediaSet(CMediaSet& MediaSet, bool bUpward)
{
    if (E_MediaSetType::MST_Singer == MediaSet.m_eType)
    {
        CSinger& singer = (CSinger&)MediaSet;
        if (singer.albums().size() == 1 && singer.attachDir().empty())
        {
            if (bUpward)
            {
                _showMediaSet(*singer.m_pParent);
            }
            else
            {
                _showMediaSet((CMediaSet&)singer.albums().front());
            }
            return;
        }
    }

    _showMediaSet(MediaSet);
}

bool CMLListView::hittestMedia(const IMedia& media)
{
    auto pMediaSet = media.mediaSet();
    if (NULL == pMediaSet)
    {
        return false;
    }

    _showMediaSet(*pMediaSet);

    int nIdx = m_lstSubMedias.indexOf(media);
    if (nIdx >= 0)
    {
        UINT uItem = m_lstSubSets.size() + nIdx;
        showItem(uItem, true);
        selectItem(uItem);
    }

    return true;
}

void CMLListView::showDir(CPath& dir)
{
    _cleanup();

    m_pDir = &dir;

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
            showItem(uItem, true);
            selectItem(uItem);
        }
    }
}

void CMLListView::_onPaintItem(CPainter& painter, tagLVItem& lvItem)
{
    if (m_pMediaset)
    {
        if (lvItem.uItem >= m_lstSubSets.size())
        {
            m_lstSubMedias.get(lvItem.uItem - m_lstSubSets.size(), [&](IMedia& media) {
                tagMLItemContext context(lvItem, media);
                _genMLItemContext(context);
                _paintRow(painter, context);
            });
        }
        else
        {
            m_lstSubSets.get(lvItem.uItem, [&](CMediaSet& mediaSet) {
                tagMLItemContext context(lvItem, mediaSet);
                _genMLItemContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else if (m_pDir)
    {
        cauto paSubDirs = m_pDir->dirs();
        if (lvItem.uItem >= paSubDirs.size())
        {
            m_pDir->files().get(lvItem.uItem-paSubDirs.size(), [&](XFile& subFile) {
                tagMLItemContext context(lvItem, subFile);
                _genMLItemContext(context);
                _paintRow(painter, context);
            });
        }
        else
        {
            paSubDirs.get(lvItem.uItem, [&](CPath& subPath) {
                tagMLItemContext context(lvItem, subPath);
                _genMLItemContext(context);
                _paintRow(painter, context);
            });
        }
    }
    else
    {
        tagMLItemContext context(lvItem);
        _genMLItemContext(context);
        if (context)
        {
            _paintRow(painter, context);
        }
    }
}

void CMLListView::_onItemClick(tagLVItem& lvItem, const QMouseEvent& me)
{
    if (m_pMediaset)
    {
        if (lvItem.uItem >= m_lstSubSets.size())
        {
            m_lstSubMedias.get(lvItem.uItem - m_lstSubSets.size(), [&](IMedia& media){
                _onItemClick(lvItem, me, media);
            });
        }
        else
        {
            m_lstSubSets.get(lvItem.uItem, [&](CMediaSet& mediaSet){
                _onItemClick(lvItem, me, mediaSet);
            });
        }
    }
    else if (m_pDir)
    {
        cauto paSubDirs = m_pDir->dirs();
        if (lvItem.uItem >= paSubDirs.size())
        {
            m_pDir->files().get(lvItem.uItem-paSubDirs.size(), [&](XFile& subFile) {
                _onItemClick(lvItem, me, (CPath&)subFile);
            });
        }
        else
        {
            paSubDirs.get(lvItem.uItem, [&](CPath& subPath) {
                _saveScrollRecord();
                showDir(subPath);
            });
        }
    }
    else
    {
        tagMLItemContext context(lvItem);
        _genMLItemContext(context);
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

bool CMLListView::upward()
{
    _clearScrollRecord(current());

    if (m_pMediaset)
    {
        auto parent = _onUpward(*m_pMediaset);
        if (parent)
        {
            showMediaSet(*parent, true);
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

    scrollToItem(_scrollRecord(current()));
    return true;
}
