
#pragma once

#include "model.h"

#include "dialog.h"

#include "listview.h"

#include "button.h"

class CMedialibView : public CListView
{
public:
    CMedialibView(class CPlayerView& view, QWidget *parent=NULL) :
        CListView(parent)
        , m_view(view)
        , m_RootMediaRes(view.getModel().getRootMediaRes())
        , m_SingerLib(view.getModel().getSingerMgr())
        , m_PlaylistLib(view.getModel().getPlaylistMgr())
    {
    }

private:
    class CPlayerView& m_view;

    CMediaRes& m_RootMediaRes;
    CMediaSet& m_SingerLib;
    CMediaSet& m_PlaylistLib;

    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    CMediaRes *m_pMediaRes = NULL;

public:
    void showRoot();
    void showMediaSet(CMediaSet *pMediaSet);
    void showMediaRes(CMediaRes& MediaRes);

    bool handleReturn();

private:
    UINT getRowCount() override;
    UINT getItemCount() override;

    void _onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem) override;

    void _onPaintItem(QPainter& painter, CMediaSet& MediaSet, QRect& rcItem);
    void _onPaintItem(QPainter& painter, CMedia& Media, QRect& rcItem);
    void _onPaintItem(QPainter& painter, CMediaRes& MediaRes, QRect& rcItem);

    void _handleRowClick(UINT uRowIdx) override;

    void _handleItemClick(CMediaSet& MediaSet);
    void _handleItemClick(IMedia& Media);
    void _handleItemClick(CMediaRes& MediaRes);
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

private:
   void _relayout(int cx, int cy) override;

public:
    void show()
    {
         CDialog<>::show();
         m_MedialibView.showRoot();
    }

    bool _handleReturn() override
    {
        return m_MedialibView.handleReturn();
    }
};
