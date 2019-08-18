
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

    CListViewEx(QWidget *parent=NULL, UINT uColumnCount = 1, UINT uPageRowCount=0)
        : CListView(parent, uColumnCount, uPageRowCount)
    {
    }

protected:
    CMediaRes *m_pMediaRes = NULL;

    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    map<void*, float> m_mapScrollRecord;

private:
    UINT getRowCount() override;

    virtual UINT getRootCount() = 0;

public:
    void showRoot();

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem=NULL);

    void showMediaRes(CMediaRes& MediaRes);

protected:
    bool handleReturn();

    float& _scrollRecord();
    void _saveScrollRecord();
    void _clearScrollRecord();

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

    QPixmap m_pmRightTip;

public:
    void init();

    void showRoot();

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem=NULL);

    void showMediaRes(CMediaRes& MediaRes);

    void showMediaRes(const wstring& strPath);

    bool handleReturn();

private:
    UINT getPageRowCount() override;

    UINT getColumnCount() override;

    UINT getRootCount() override;

    bool _getRootItemContext(const tagListViewRow&, tagRootItemContext&);

    void _onPaintRow(CPainter&, QRect&, const tagListViewRow&) override;

    void _handleRowClick(const tagListViewRow&, const QMouseEvent&) override;

private:
    void _getTitle(CMediaSet& MediaSet, WString& strTitle);
    void _getTitle(CMediaRes& MediaRes, WString& strTitle);

    void _paintMediaSetItem(CPainter& painter, QRect& rc, const tagListViewRow&, CMediaSet& MediaSet);

    void _paintMediaResItem(CPainter& painter, QRect& rc, const tagListViewRow&, CMediaRes& MediaRes);

    void _paintItem(CPainter& painter, QRect& rc, const tagListViewRow&, const tagItemContext& context);

    QPixmap& _getSingerPixmap(CSinger& Singer);

    void _handleItemClick(CMediaSet& MediaSet);

    void _handleItemClick(IMedia& Media);

    void _handleItemClick(CMediaRes& MediaRes);
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
