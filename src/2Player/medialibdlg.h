
#pragma once

#include "model.h"

#include "dialog.h"

#include "listview.h"

#include "button.h"

class CMedialibView : public CListView
{
private:
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

public:
    CMedialibView(class CPlayerView& view, class CMedialibDlg& medialibDlg);

private:
    class CPlayerView& m_view;

    class CMedialibDlg& m_medialibDlg;

    CMediaRes& m_RootMediaRes;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaRes *m_pMediaRes = NULL;

    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    map<void*, float> m_mapScrollRecord;

    QPixmap m_pixmapDir;
    QPixmap m_pixmapDirLink;
    QPixmap m_pixmapFile;

    QPixmap m_pixmapSingerGroup;
    QPixmap m_pixmapDefaultSinger;
    QPixmap m_pixmapAlbum;
    QPixmap m_pixmapAlbumItem;

    QPixmap m_pixmapPlaylist;
    QPixmap m_pixmapPlayItem;

    QPixmap m_pixmapRightTip;

public:
    void init();

    void showRoot();

    void showMediaRes(CMediaRes& MediaRes, CMediaRes *pHittestItem=NULL);

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem=NULL);

    bool handleReturn();

private:
    UINT getPageRowCount() override;
    UINT getRowCount() override;

    void _onPaintItem(CPainter&, QRect&, const tagListViewRow&) override;

    void _handleRowClick(UINT uRowIdx, const QMouseEvent&) override;

private:
    void _getTitle(CMediaRes& MediaRes, WString& strTitle);
    void _getTitle(CMediaSet& MediaSet, WString& strTitle);

    float& _scrollRecord();
    void _saveScrollRecord();
    void _clearScrollRecord();

    void _paintMediaResItem(CPainter& painter, QRect& rc, const tagListViewRow&, CMediaRes& MediaRes);

    void _paintMediaSetItem(CPainter& painter, QRect& rc, const tagListViewRow&, CMediaSet& MediaSet);

    void _paintItem(CPainter& painter, QRect& rc, const tagListViewRow&, const tagItemContext& context);

    QPixmap& _getSingerPixmap(CSinger& Singer);

    void _handleItemClick(CMediaRes& MediaRes);

    void _handleItemClick(CMediaSet& MediaSet);

    void _handleItemClick(IMedia& Media);
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

    void showMediaRes(CMediaRes& MediaRes)
    {
        if (MediaRes.IsDir())
        {
            CDialog<>::show();
            m_MedialibView.showMediaRes(MediaRes);
        }
        else
        {
            auto parent = MediaRes.parent();
            if (parent)
            {
                CDialog<>::show();
                m_MedialibView.showMediaRes(*parent, &MediaRes);
            }
        }
    }

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestMedia=NULL)
    {
        CDialog<>::show();
        m_MedialibView.showMediaSet(MediaSet, pHittestMedia);
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
