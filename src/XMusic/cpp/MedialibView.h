
#pragma once

#include "ListViewEx.h"

#define __playIconOffset __size(12)

class CMedialibView : public CListViewEx
{
    Q_OBJECT
public:
    CMedialibView(class CMedialibDlg& medialibDlg, class CApp& app, CMediaDir& OuterDir);

private:
    class CMedialibDlg& m_medialibDlg;

    class CApp& m_app;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaDir &m_OuterDir;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmDefaultSinger;
    QPixmap m_pmAlbum;

    QPixmap m_pmPlaylistSet;
    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmXmusicDir;
    QPixmap m_pmSSDir;
    QPixmap m_pmSSFile;

    QPixmap m_pmHires;
    QPixmap m_pmDSD;

    QPixmap m_pmDir;
    QPixmap m_pmDirLink;
    QPixmap m_pmFile;

    QPixmap m_pmPlay;
    QPixmap m_pmPlayOpacity;
    QPixmap m_pmAddPlay;
    QPixmap m_pmAddPlayOpacity;

    list<QPixmap> m_lstSingerPixmap;
    map<UINT, QPixmap*> m_mapSingerPixmap;

    int m_nFlashingRow = -1;

    XThread m_thrAsyncTask;

signals:
    void signal_update();

public:
    void initpm();

    void updateSingerImg();

    void play();

    void reset() override
    {
        m_nFlashingRow = -1;

        CListView::reset();
    }

    void cleanup();

private:
    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet&) override;
    void _onShowDir(CPath&) override;

    size_t getPageRowCount() const override;

    size_t getColumnCount() const override;

    size_t _getRootRowCount() const override;

    bool _genRootRowContext(tagMLItemContext&) override;
    void _genMediaContext(tagMLItemContext&) override;

    cqrc _paintText(tagLVItemContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha) override;

    bool event(QEvent *ev) override;

    cqpm _getSingerPixmap(UINT uSingerID, cwstr strSingerName);

    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CPath&, WString& strTitle);

    void _onRowClick(tagLVItem&, const QMouseEvent&, CMediaSet& mediaSet) override;

    void _onRowClick(tagLVItem& lvItem, const QMouseEvent& me, CMedia& media) override
    {
        _onMediaClick(lvItem, me, media);
    }

    void _onRowClick(tagLVItem& lvItem, const QMouseEvent& me, CPath& path) override;

    void _onMediaClick(tagLVItem&, const QMouseEvent&, IMedia&);

    CMediaSet* _onUpward(CMediaSet& currentMediaSet) override;
    CPath* _onUpward(CPath& currentDir) override;

    void _flashRow(UINT uRow, UINT uMSDelay=300);
};
