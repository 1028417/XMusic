
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

        CPath *pPath = NULL;

        tagMediaContext()
        {
            bSingleLine = true;
        }

        tagMediaContext(CMediaSet& MediaSet) : tagRowContext(E_RowStyle::IS_RightTip, MediaSet.m_strName)
            , pMediaSet(&MediaSet)
        {
            bSingleLine = true;
        }

        tagMediaContext(CMedia& media) : tagRowContext(E_RowStyle::IS_Underline, media.GetTitle())
            , pMedia(&media)
        {
            bSingleLine = true;
        }

        tagMediaContext(CPath& path) :
            tagRowContext(path.fileInfo().bDir ? E_RowStyle::IS_RightTip : E_RowStyle::IS_Underline, path.name())
            , pPath(&path)
        {
            bSingleLine = true;
        }
    };

public:
    CListViewEx(QWidget *parent=NULL, UINT uColumnCount = 1, UINT uPageRowCount=0);

protected:
    CMediaSet *m_pMediaset = NULL;

    CPath *m_pPath = NULL;

private:
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    TD_PathList m_paSubPath;

    SSet<void*> m_setRootObject;

private:
    virtual void _onShowRoot() {}
    virtual void _onShowMediaSet(CMediaSet&) {}
    virtual void _onShowPath(CPath&) {}

    size_t getRowCount() override;
    virtual size_t getRootCount() = 0;

    void _onPaintRow(CPainter&, const tagLVRow&) override;

    virtual bool _genRootRowContext(const tagLVRow&, tagMediaContext&) = 0;
    virtual void _genMediaContext(tagMediaContext&) {}

    void _onRowClick(const tagLVRow&, const QMouseEvent&) override;
    virtual void _onRowClick(const tagLVRow&, CMedia&){}
    virtual void _onRowClick(const tagLVRow&, CPath&){}

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

protected:
    virtual bool _onUpward();

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
