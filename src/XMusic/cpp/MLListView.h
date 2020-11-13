
#pragma once

#include "model/model.h"

#include "listview.h"

struct tagMLItemContext : public tagLVItemContext
{
    CMediaSet *pMediaSet = NULL;
    CMedia *pMedia = NULL;

    CPath *pDir = NULL;
    XFile *pFile = NULL;

    operator bool() const
    {
        return pMediaSet || pMedia || pDir || pFile;
    }

    tagMLItemContext(tagLVItem& lvItem) : tagLVItemContext(lvItem)
    {
    }

    tagMLItemContext(tagLVItem& lvItem, IMedia& media) : tagLVItemContext(lvItem)
    {
        if (media.type() == E_MediaType::MT_MediaRes)
        {
            pFile = (CMediaRes*)&media;
        }
        else
        {
            pMedia = (CMedia*)&media;
        }
    }

    tagMLItemContext(tagLVItem& lvItem, CMediaSet& MediaSet) :
        tagLVItemContext(lvItem, E_LVItemStyle::IS_BottomLine)
        , pMediaSet(&MediaSet)
    {
        strText = MediaSet.m_strName;
    }
    tagMLItemContext(tagLVItem& lvItem, CMedia& media) :
        tagLVItemContext(lvItem, E_LVItemStyle::IS_BottomLine)
        , pMedia(&media)
    {
        strText = media.GetTitle();
    }

    tagMLItemContext(tagLVItem& lvItem, CPath& dir) :
        tagLVItemContext(lvItem, E_LVItemStyle::IS_BottomLine)
        , pDir(&dir)
    {
        strText = dir.fileName();
    }
    tagMLItemContext(tagLVItem& lvItem, XFile& file) :
        tagLVItemContext(lvItem, E_LVItemStyle::IS_BottomLine)
        , pFile(&file)
    {
        strText = file.fileName();
    }

    bool playable() const
    {
        if (pFile || pMedia || (pMediaSet && (E_MediaSetType::MST_Playlist == pMediaSet->m_eType
             || E_MediaSetType::MST_Album == pMediaSet->m_eType
             || E_MediaSetType::MST_Singer == pMediaSet->m_eType)))
        {
            return true;
        }

        return false;
    }
};

class CMLListView : public CListView
{
public:
    CMLListView(QWidget *parent, E_LVScrollBar eScrollBar) : CListView(parent, eScrollBar)
    {
    }

private:
    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    CPath *m_pDir = NULL;

protected:
    void _cleanup()
    {
        reset();

        m_pMediaset = NULL;
        m_lstSubSets.clear();
        m_lstSubMedias.clear();

        m_pDir = NULL;
    }

    virtual CMediaSet* _onUpward(CMediaSet& currentMediaSet)
    {
        return currentMediaSet.m_pParent;
    }
    virtual CPath* _onUpward(CPath& currentDir)
    {
        return currentDir.parent();
    }

    virtual void _onItemClick(tagLVItem&, const QMouseEvent&, CMediaSet& mediaSet)
    {
        _saveScrollRecord();
        showMediaSet(mediaSet);
    }

private:
    virtual void _onShowRoot() {}
    virtual void _onShowMediaSet(CMediaSet&) {}
    virtual void _onShowDir(CPath&) {}

    virtual size_t _getRootItemCount() const = 0;

    void _onPaintItem(CPainter&, tagLVItem&) override;

    virtual void _genMLItemContext(tagMLItemContext&) = 0;

    void _onItemClick(tagLVItem&, const QMouseEvent&) override;

    virtual void _onItemClick(tagLVItem&, const QMouseEvent&, CMedia&){}
    virtual void _onItemClick(tagLVItem&, const QMouseEvent&, CPath&){}

    inline void _saveScrollRecord()
    {
        CListView::_saveScrollRecord(current());
    }

    void _showMediaSet(CMediaSet& MediaSet);

public:
    size_t getItemCount() const override;

    void* current() const
    {
        if (m_pMediaset)
        {
            return m_pMediaset;
        }

        return m_pDir;
    }

    CMediaSet* currentMediaSet() const
    {
        return m_pMediaset;
    }
    const TD_MediaSetList& currentSubSets() const
    {
        return m_lstSubSets;
    }
    const TD_MediaList& currentSubMedias() const
    {
        return m_lstSubMedias;
    }

    CPath* currentDir() const
    {
        return m_pDir;
    }

    void showRoot();

    void showMediaSet(CMediaSet& MediaSet, bool bUpward=false);
    void hittestMedia(const CMedia& media);

    virtual void showDir(CPath& dir);

    void hittestFile(XFile& file);

    bool upward();
};
