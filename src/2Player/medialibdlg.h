
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

public:
    CMedialibView(class CPlayerView& view, class CMedialibDlg& medialibDlg);

private:
    class CPlayerView& m_view;

    class CMedialibDlg& m_medialibDlg;

    CMediaRes& m_RootMediaRes;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    QPixmap m_pixmapFolder;
    QPixmap m_pixmapFolderLink;
    QPixmap m_pixmapFile;

    QPixmap m_pixmapSingerGroup;
    QPixmap m_pixmapDefaultSinger;
    QPixmap m_pixmapAlbum;
    QPixmap m_pixmapAlbumItem;

    QPixmap m_pixmapPlaylist;
    QPixmap m_pixmapPlayItem;

    QPixmap m_pixmapRightTip;

    CMediaRes *m_pMediaRes = NULL;

    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

public:
    void showRoot();

    void showMediaRes(CMediaRes& MediaRes, CMediaRes *pHittestItem=NULL);

    void showMediaSet(CMediaSet& MediaSet, CMedia *pHittestItem=NULL);

    bool handleReturn();

private:
    UINT getRowCount() override;
    UINT getItemCount() override;

    void _onPaintItem(QPainter&, QRect&, const tagListViewItem&) override;

    void _paintMediaResItem(QPainter& painter, QRect& rc, const tagListViewItem&, CMediaRes& MediaRes);

    void _paintMediaSetItem(QPainter& painter, QRect& rc, const tagListViewItem&, CMediaSet& MediaSet);

    void _paintItem(QPainter& painter, QRect& rc, const tagListViewItem&, QPixmap& pixmap
            , const wstring& strText, E_ItemStyle eStyle=E_ItemStyle::IS_Normal, UINT uIconSize=0);

    QPixmap& _getSingerPixmap(CSinger& Singer);

    void _handleRowClick(UINT uRowIdx, QMouseEvent& ev) override;

    void _handleItemClick(CMediaRes& MediaRes);

    void _handleItemClick(CMediaSet& MediaSet);

    void _handleItemClick(IMedia& Media);
};

class CMedialibDlg : public CDialog<>
{
    Q_OBJECT
public:
    explicit CMedialibDlg(class CPlayerView& view, QWidget *parent = 0);

private:
    class CPlayerView& m_view;

    CMedialibView m_MedialibView;

private slots:
    void slot_buttonClicked(CButton*);

public:
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

private:
    void _relayout(int cx, int cy) override;

    bool _handleReturn() override
    {
        return m_MedialibView.handleReturn();
    }
};
