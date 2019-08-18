
#pragma once

#include "model.h"

#include "dialog.h"

#include "listview.h"

#include "button.h"

class CListViewEx : public CListView
{
protected:
    enum class E_ItemStyle
    {
        IS_Normal
        , IS_Underline
        , IS_RightTip
    };

    struct tagItemContext
    {
        E_ItemStyle eStyle = E_ItemStyle::IS_Normal;

        QPixmap *pixmap = NULL;

        wstring strText;

        wstring strRemark;

        UINT uIconSize = 0;
    };

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

    void _onPaintRow(CPainter&, QRect&, const tagListViewRow&) override;
    virtual void _onPaintRootRow(CPainter&, QRect&, const tagListViewRow&){}
    virtual void _onPaintRow(CPainter&, QRect&, const tagListViewRow&, CMediaSet&){}
    virtual void _onPaintRow(CPainter&, QRect&, const tagListViewRow&, CMedia&){}
    virtual void _onPaintRow(CPainter&, QRect&, const tagListViewRow&, CMediaRes&){}

    void _onRowClick(const tagListViewRow&, const QMouseEvent&) override;
    virtual void _onRootRowClick(const tagListViewRow&){}
    virtual void _onRowClick(const tagListViewRow&, IMedia&){}

public:
    virtual void showRoot();

    virtual void showMediaSet(CMediaSet&, CMedia *pHittestItem=NULL);

    virtual void showMediaRes(CMediaRes&);

    bool handleReturn();

protected:
    void _paintRow(CPainter&, QRect&, const tagListViewRow&, const tagItemContext&);

    float& _scrollRecord();
    void _saveScrollRecord();
    void _clearScrollRecord();

    bool isInRoot() const
    {
        return NULL==m_pMediaset && NULL==m_pMediaRes;
    }
};

class CMedialibView : public CListViewEx
{
private:
    struct tagRootItemContext : tagItemContext
    {
        CMediaSet *pMediaSet = NULL;
        CMediaRes *pMediaRes = NULL;
    };

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

    bool _getRootItemContext(const tagListViewRow&, tagRootItemContext&);

private:
    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CMediaRes&, WString& strTitle);

    void _onPaintRootRow(CPainter&, QRect& rc, const tagListViewRow&) override;
    void _onPaintRow(CPainter&, QRect& rc, const tagListViewRow&, CMediaSet&) override;
    void _onPaintRow(CPainter&, QRect& rc, const tagListViewRow&, CMedia&) override;
    void _onPaintRow(CPainter&, QRect& rc, const tagListViewRow&, CMediaRes&) override;

    void _paintRow(CPainter&, QRect&, const tagListViewRow&, const tagItemContext&);

    QPixmap& _getSingerPixmap(CSinger&);

    void _onRootRowClick(const tagListViewRow& lvRow) override;
    void _onRowClick(const tagListViewRow&, IMedia&) override;
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
