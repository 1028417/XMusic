
#pragma once

#include "model.h"

#include "dialog.h"

#include "listview.h"

#include "button.h"

enum class E_ItemStyle
{
    IS_Normal
    , IS_Underline
    , IS_RightTip
};

struct tagMediaContext
{
    CMediaSet *pMediaSet = NULL;
    CMedia *pMedia = NULL;

    CMediaRes *pMediaRes = NULL;

    E_ItemStyle eStyle = E_ItemStyle::IS_Normal;

    QPixmap *pixmap = NULL;

    wstring strText;

    wstring strRemark;

    UINT uIconSize = 0;  

    tagMediaContext(){}

    tagMediaContext(CMediaSet& MediaSet)
    {
        pMediaSet = &MediaSet;

        eStyle = E_ItemStyle::IS_RightTip;
        strText = MediaSet.m_strName;
    }

    tagMediaContext(CMedia& media)
    {
        pMedia = &media;

        eStyle = E_ItemStyle::IS_Underline;
        strText = media.GetTitle();
    }

    tagMediaContext(CMediaRes& MediaRes)
    {
        pMediaRes = &MediaRes;

        if (MediaRes.IsDir())
        {
            eStyle = E_ItemStyle::IS_RightTip;
            strText = MediaRes.GetName();
        }
        else
        {
            eStyle = E_ItemStyle::IS_Underline;
            strText = MediaRes.GetTitle();
        }
    }
};

class CListViewEx : public CListView
{
public:
    CListViewEx(QWidget *parent=NULL, UINT uColumnCount = 1, UINT uPageRowCount=0);

protected:
    CMediaRes *m_pMediaRes = NULL;

    CMediaSet *m_pMediaset = NULL;

private:
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    map<void*, float> m_mapScrollRecord;

    QPixmap m_pmRightTip;

private:
    UINT getRowCount() override;

    virtual UINT getRootCount() = 0;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&) override;

    virtual bool _genRootRowContext(const tagLVRow&, tagMediaContext&) = 0;

    virtual void _genRowContext(tagMediaContext&) {}

    void _onRowClick(const tagLVRow&, const QMouseEvent&) override;
    virtual void _onRowClick(const tagLVRow&, IMedia&){}

protected:
    virtual void _paintRow(CPainter&, QRect&, const tagLVRow&, const tagMediaContext&);

    float& _scrollRecord();
    void _saveScrollRecord();
    void _clearScrollRecord();

    bool isInRoot() const
    {
        return NULL==m_pMediaset && NULL==m_pMediaRes;
    }

public:
    virtual void showRoot();

    virtual void showMediaSet(CMediaSet&, CMedia *pHittestItem=NULL);

    virtual void showMediaRes(CMediaRes&);

    bool handleReturn();
};

class CMedialibView : public CListViewEx
{
public:
    CMedialibView(class CPlayerView& view, class CMedialibDlg& medialibDlg);

private:
    class CPlayerView& m_view;

    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaRes& m_RootMediaRes;

    CMediaRes m_sdcard;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmDefaultSinger;
    QPixmap m_pmAlbum;
    QPixmap m_pmAlbumItem;

    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmDir;
    QPixmap m_pmDirLink;
    QPixmap m_pmFile;

public:
    void init();

    void showRoot() override;

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem=NULL) override;

    void showMediaRes(CMediaRes& MediaRes) override;

    void showMediaRes(const wstring& strPath);

    bool handleReturn();

private:
    UINT getPageRowCount() override;

    UINT getColumnCount() override;

    UINT getRootCount() override;

    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CMediaRes&, WString& strTitle);

    bool _genRootRowContext(const tagLVRow&, tagMediaContext&) override;

    void _genRowContext(tagMediaContext&) override;

    void _paintRow(CPainter&, QRect&, const tagLVRow&, const tagMediaContext&) override;

    QPixmap& _getSingerPixmap(CSinger&);

    void _onRowClick(const tagLVRow&, IMedia&) override;
};

class CMedialibDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CMedialibDlg(class CPlayerView& view);

private:
    class CPlayerView& m_view;

    CMedialibView m_MedialibView;

public:
    void init();

    void show()
    {
        CDialog<>::show();
        m_MedialibView.showRoot();
    }

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestMedia=NULL)
    {
        CDialog<>::show();
        m_MedialibView.showMediaSet(MediaSet, pHittestMedia);
    }

    void showMediaRes(const wstring& strPath)
    {
        CDialog<>::show();
        m_MedialibView.showMediaRes(strPath);
    }

    void showUpwardButton(bool bVisible) const;

    void setTitle(const wstring& strTitle) const;

private:
    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.handleReturn();
    }
};
