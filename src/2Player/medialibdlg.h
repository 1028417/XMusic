
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

    CMediaRes *m_pMediaRes = NULL;

    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

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

    QPoint m_ptClicking;

public:
    void showRoot();

    void showMediaRes(CMediaRes& MediaRes);

    void showMediaSet(CMediaSet& MediaSet);

    bool handleReturn();

private:
    UINT getRowCount() override;
    UINT getItemCount() override;

    void _onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem) override;

    void _paintMediaResItem(QPainter& painter, QRect& rcItem, CMediaRes& MediaRes);

    void _paintMediaSetItem(QPainter& painter, QRect& rcItem, CMediaSet& MediaSet);

    void _paintItem(QPainter& painter, QRect& rcItem, QPixmap& pixmap, const wstring& strText
                    , E_ItemStyle eStyle=E_ItemStyle::IS_Normal, UINT uIconSize=0);

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

    void showUpwardButton(bool bVisible) const;

private:
   void _relayout(int cx, int cy) override;

    bool _handleReturn() override
    {
        return m_MedialibView.handleReturn();
    }
};
