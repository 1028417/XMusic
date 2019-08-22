
#pragma once

#include "model.h"

#include "listview.h"

class CListViewEx : public CListView
{
protected:
    struct tagMediaContext : public tagRowContext
    {
        CMediaSet *pMediaSet = NULL;
        CMedia *pMedia = NULL;

        CPath *pPath = NULL;

        tagMediaContext(){}

        tagMediaContext(CMediaSet& MediaSet)
        {
            pMediaSet = &MediaSet;

            eStyle = E_RowStyle::IS_RightTip;
            strText = MediaSet.m_strName;
        }

        tagMediaContext(CMedia& media)
        {
            pMedia = &media;

            eStyle = E_RowStyle::IS_Underline;
            strText = media.GetTitle();
        }

        tagMediaContext(CPath& path)
        {
            pPath = &path;

            if (path.IsDir())
            {
                eStyle = E_RowStyle::IS_RightTip;
            }
            else
            {
                eStyle = E_RowStyle::IS_Underline;
            }
            strText = path.GetName();
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

    map<void*, float> m_mapScrollRecord;

private:
    virtual void _onShowRoot() {}
    virtual void _onShowMediaSet(CMediaSet&) {}
    virtual void _onShowPath(CPath&) {}

    UINT getRowCount() override;
    virtual UINT getRootCount() = 0;

    virtual bool _genRootRowContext(const tagLVRow&, tagMediaContext&) = 0;
    virtual void _genRowContext(tagMediaContext&) {}

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;
    virtual void _onPaintRow(CPainter& painter, QRect& rc, const tagLVRow& lvRow, const tagMediaContext& context)
    {
        _paintRow(painter, rc, lvRow, context);
    }

    void _onRowClick(const tagLVRow&, const QMouseEvent&) override;
    virtual void _onRowClick(const tagLVRow&, CMedia&){}
    virtual void _onRowClick(const tagLVRow&, CPath&){}

    float& _scrollRecord();
    void _saveScrollRecord();
    void _clearScrollRecord();

public:
    void showRoot();

    void showMediaSet(CMediaSet& MediaSet);
    void showMedia(CMedia& media);

    void showPath(CPath& path);

    bool isInRoot() const
    {
        return NULL==m_pMediaset && NULL==m_pPath;
    }

    void upward();
};
