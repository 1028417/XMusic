
#pragma once

#include "model.h"

#include "listview.h"

class CListViewEx : public CListView
{
protected:
    enum class E_RowStyle
    {
        IS_Normal
        , IS_Underline
        , IS_RightTip
    };

    struct tagRowContext
    {
        CMediaSet *pMediaSet = NULL;
        CMedia *pMedia = NULL;

        CPath *pPath = NULL;

        E_RowStyle eStyle = E_RowStyle::IS_Normal;

        QPixmap *pixmap = NULL;

        wstring strText;

        wstring strRemark;

        UINT uIconSize = 0;

        tagRowContext(){}

        tagRowContext(CMediaSet& MediaSet)
        {
            pMediaSet = &MediaSet;

            eStyle = E_RowStyle::IS_RightTip;
            strText = MediaSet.m_strName;
        }

        tagRowContext(CMedia& media)
        {
            pMedia = &media;

            eStyle = E_RowStyle::IS_Underline;
            strText = media.GetTitle();
        }

        tagRowContext(CPath& path)
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
    CPath *m_pPath = NULL;

    CMediaSet *m_pMediaset = NULL;

private:
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    map<void*, float> m_mapScrollRecord;

    QPixmap m_pmRightTip;

private:
    UINT getRowCount() override;
    virtual UINT getRootCount() = 0;

    virtual bool _genRootRowContext(const tagLVRow&, tagRowContext&) = 0;
    virtual void _genRowContext(tagRowContext&) {}

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;

    void _onRowClick(const tagLVRow&, const QMouseEvent&) override;
    virtual void _onRowClick(const tagLVRow&, CMedia&){}
    virtual void _onRowClick(const tagLVRow&, CPath&){}

    float& _scrollRecord();
    void _saveScrollRecord();
    void _clearScrollRecord();

protected:
    virtual void _paintRow(CPainter&, QRect&, const tagLVRow&, const tagRowContext&);

public:
    virtual void showRoot();

    virtual void showMediaSet(CMediaSet&, CMedia *pHittestItem=NULL);

    virtual void showPath(CPath&);

    bool isInRoot() const
    {
        return NULL==m_pMediaset && NULL==m_pPath;
    }

    void upward();
};
