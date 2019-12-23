
#pragma once

#include "model/model.h"

#include "listview.h"

class CListViewEx : public CListView
{
protected:
    struct tagMediaContext : public tagRowContext
    {
        tagLVRow& lvRow;

        CMediaSet *pMediaSet = NULL;
        CMedia *pMedia = NULL;

        CPath *pDir = NULL;
        XFile *pFile = NULL;

        tagMediaContext(tagLVRow& t_lvRow)
            : lvRow(t_lvRow)
        {
        }

        tagMediaContext(tagLVRow& t_lvRow, CMediaSet& MediaSet) :
            tagRowContext(E_RowStyle::IS_BottomLine | E_RowStyle::IS_RightTip)
            , lvRow(t_lvRow)
            , pMediaSet(&MediaSet)
        {
            strText = MediaSet.m_strName;
        }
        tagMediaContext(tagLVRow& t_lvRow, CMedia& media) :
            tagRowContext(E_RowStyle::IS_BottomLine)
            , lvRow(t_lvRow)
            , pMedia(&media)
        {
            strText = media.GetTitle();
        }

        tagMediaContext(tagLVRow& t_lvRow, CPath& dir) :
            tagRowContext(E_RowStyle::IS_BottomLine | E_RowStyle::IS_RightTip)
            , lvRow(t_lvRow)
            , pDir(&dir)
        {
            strText = dir.name();
        }
        tagMediaContext(tagLVRow& t_lvRow, XFile& file) :
            tagRowContext(E_RowStyle::IS_BottomLine)
            , lvRow(t_lvRow)
            , pFile(&file)
        {
            strText = file.name();
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
    const TD_PathList & currentSubDirs() const
    {
        return m_paSubDirs;
    }
    const TD_XFileList & currentSubFiles() const
    {
        return m_paSubFiles;
    }

    virtual CMediaSet* _onUpward(CMediaSet& currentMediaSet)
    {
        return currentMediaSet.m_pParent;
    }
    virtual CPath* _onUpward(CPath& currentDir)
    {
        return (CPath*)currentDir.fileInfo().pParent;
    }

    void _clear()
    {
        CListView::reset();

        m_pMediaset = NULL;
        m_lstSubSets.clear();
        m_lstSubMedias.clear();

        m_pPath = NULL;
        m_paSubDirs.clear();
        m_paSubFiles.clear();
    }

private:
    virtual void _onShowRoot() {}
    virtual void _onShowMediaSet(CMediaSet&) {}
    virtual void _onShowPath(CPath&) {}

    size_t getRowCount() override;

    virtual size_t _getRootRowCount() = 0;

    void _onPaintRow(CPainter&, tagLVRow&) override;

    virtual bool _genRootRowContext(const tagLVRow&, tagMediaContext&) = 0;
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
