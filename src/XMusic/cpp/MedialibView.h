
#pragma once

#include "ListViewEx.h"

#define __playIconMagin (__lvRowMargin- __size(6))

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

    void play();

    void clear();

    void updateSingerImg();

    void reset() override
    {
        m_nFlashingRow = -1;

        CListView::reset();
    }

private:
    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet&) override;
    void _onShowDir(CPath&) override;

    size_t getPageRowCount() const override;

    size_t getColumnCount() const override;

    size_t _getRootRowCount() const override;

    bool _genRootRowContext(tagMediaContext&) override;
    void _genMediaContext(tagMediaContext&) override;

    cqrc _paintText(tagRowContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha) override;

    bool event(QEvent *ev) override;

    cqpm _getSingerPixmap(UINT uSingerID, const wstring& strSingerName);

    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CPath&, WString& strTitle);

    void _onRowClick(tagLVRow&, const QMouseEvent&, CMediaSet& mediaSet) override;

    void _onRowClick(tagLVRow& lvRow, const QMouseEvent& me, CMedia& media) override
    {
        _onMediaClick(lvRow, me, media);
    }

    void _onRowClick(tagLVRow& lvRow, const QMouseEvent& me, CPath& path) override;

    void _onMediaClick(tagLVRow&, const QMouseEvent&, IMedia&);

    CMediaSet* _onUpward(CMediaSet& currentMediaSet) override;
    CPath* _onUpward(CPath& currentDir) override;

    void _flashRow(UINT uRow, UINT uMSDelay=300);
};
