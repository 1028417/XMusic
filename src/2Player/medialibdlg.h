
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
    {
    }

private:
    class CPlayerView& m_view;

    CMediaSet *m_pMediaset = NULL;
    TD_MediaSetList m_lstSubSets;
    TD_MediaList m_lstSubMedias;

    CMediaRes *m_pMediaRes = NULL;

private:
    void _showMediaSet(CMediaSet& MediaSet);
    void _showMediaRes(CMediaRes& MediaRes);

    UINT getItemCount() override;

    void _onPaintItem(QPainter& painter, UINT uItem, QRect& rcItem) override;

    void _onPaintItem(QPainter& painter, CMediaSet& MediaSet, QRect& rcItem);
    void _onPaintItem(QPainter& painter, CMedia& Media, QRect& rcItem);
    void _onPaintItem(QPainter& painter, CMediaRes& MediaRes, QRect& rcItem);

    void _handleMouseDoubleClick(UINT uRowIdx) override;

    void _handleMouseDoubleClick(CMediaSet& MediaSet);
    void _handleMouseDoubleClick(CMedia& Media);
    void _handleMouseDoubleClick(CMediaRes& MediaRes);
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
   void _relayout(int cx, int cy) override;
};
