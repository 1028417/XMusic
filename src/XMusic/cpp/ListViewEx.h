
#pragma once

#include "model/model.h"

#include "listview.h"

class CListViewEx : public CListView
{
protected:
    struct tagMediaContext : public tagRowContext
    {
        CMediaSet *pMediaSet = NULL;
        CMedia *pMedia = NULL;

        CPath *pDir = NULL;
        XFile *pFile = NULL;

        tagMediaContext(tagLVRow& lvRow) : tagRowContext(lvRow)
        {
        }

        tagMediaContext(tagLVRow& lvRow, IMedia& media) : tagRowContext(lvRow)
        {
            if (media.GetMediaSetType() == E_MediaSetType::MST_Null)
            {
                pFile = (CMediaRes*)&media;
            }
            else
            {
                pMedia = (CMedia*)&media;
            }
        }

        tagMediaContext(tagLVRow& lvRow, CMediaSet& MediaSet) :
            tagRowContext(lvRow, E_RowStyle::IS_MultiLine | E_RowStyle::IS_BottomLine)
            , pMediaSet(&MediaSet)
        {
            strText = MediaSet.m_strName;
        }
        tagMediaContext(tagLVRow& lvRow, CMedia& media) :
            tagRowContext(lvRow, E_RowStyle::IS_MultiLine | E_RowStyle::IS_BottomLine)
            , pMedia(&media)
        {
            strText = media.GetTitle();
        }

        tagMediaContext(tagLVRow& lvRow, CPath& dir) :
            tagRowContext(lvRow, E_RowStyle::IS_MultiLine | E_RowStyle::IS_BottomLine)
            , pDir(&dir)
        {
            strText = dir.fileName();
        }
        tagMediaContext(tagLVRow& lvRow, XFile& file) :
            tagRowContext(lvRow, E_RowStyle::IS_MultiLine | E_RowStyle::IS_BottomLine)
            , pFile(&file)
        {
            strText = file.fileName();
        }
    };

public:
    CListViewEx(QWidget *parent) : CListView(parent)
    {
    }

private:
    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    CPath *m_pPath = NULL;
    TD_PathList m_paSubDirs;
    TD_XFileList m_paSubFiles;

protected:
    void _clear()
    {
        reset();

        m_pMediaset = NULL;
        m_lstSubSets.clear();
        m_lstSubMedias.clear();

        m_pPath = NULL;
        m_paSubDirs.clear();
        m_paSubFiles.clear();
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

    CPath* currentPath() const
    {
        return m_pPath;
    }

    virtual CMediaSet* _onUpward(CMediaSet& currentMediaSet)
    {
        return currentMediaSet.m_pParent;
    }
    virtual CPath* _onUpward(CPath& currentDir)
    {
        return (CPath*)currentDir.fileInfo().pParent;
    }

    virtual void _onRowClick(tagLVRow&, const QMouseEvent&, CMediaSet& mediaSet)
    {
        _saveScrollRecord();
        showMediaSet(mediaSet);
    }

private:
    virtual void _onShowRoot() {}
    virtual void _onShowMediaSet(CMediaSet&) {}
    virtual void _onShowPath(CPath&) {}

    size_t getRowCount() override;

    virtual size_t _getRootRowCount() = 0;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    virtual bool _genRootRowContext(tagMediaContext&) = 0;
    virtual void _genMediaContext(tagMediaContext&) = 0;

    void _onRowClick(tagLVRow&, const QMouseEvent&) override;

    virtual void _onRowClick(tagLVRow&, const QMouseEvent&, CMedia&){}
    virtual void _onRowClick(tagLVRow&, const QMouseEvent&, CPath&){}

    inline void* _current() const
    {
        void *p = m_pMediaset;
        if (NULL == p)
        {
            p = m_pPath;
        }
        return p;
    }

    inline void _saveScrollRecord()
    {
        CListView::_saveScrollRecord(_current());
    }

public:
    void showRoot();

    void showMediaSet(CMediaSet& MediaSet);
    void showMedia(CMedia& media);

    void showPath(CPath& path);

    bool isInRoot() const
    {
        return NULL==m_pMediaset && NULL==m_pPath;
    }

    bool upward();
};
