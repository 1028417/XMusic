
#pragma once

#include "ListViewEx.h"

class CMedialibView : public CListViewEx
{
    Q_OBJECT
public:
    CMedialibView(class CApp& app, class CMedialibDlg& medialibDlg, CMediaDir& OuterDir);

private:
    class CApp& m_app;

    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    XMediaLib& m_MediaLib;

    CMediaDir &m_OuterDir;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmDefaultSinger;
    QPixmap m_pmAlbum;
    QPixmap m_pmAlbumItem;

    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmDir;
    QPixmap m_pmDirLink;
    QPixmap m_pmFile;

    QPixmap m_pmPlayButton;
    QPixmap m_pmAddPlayButton;

    list<QPixmap> m_lstSingerPixmap;
    map<UINT, QPixmap*> m_mapSingerPixmap;

signals:
    void signal_update();

public:
    void init();

    bool showFile(const wstring& strFile);

    void play();

    void clear();

    void updateSingerImg();

private:
    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet& MediaSet) override;
    void _onShowPath(CPath& path) override;

    size_t getPageRowCount() override;

    size_t getColumnCount() override;

    size_t _getRootRowCount() override;

    bool _genRootRowContext(tagMediaContext&) override;
    void _genMediaContext(tagMediaContext&) override;

    void _paintText(CPainter& painter, QRect& rc, const tagRowContext& context) override;

    bool event(QEvent *ev) override;

    const QPixmap& _getSingerPixmap(UINT uSingerID, const wstring& strSingerName);

    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CMediaDir&, WString& strTitle);

    void _onMediaClick(tagLVRow&, const QMouseEvent&, IMedia&);

    void _onRowClick(tagLVRow& lvRow, const QMouseEvent& me, CMedia& media) override
    {
        _onMediaClick(lvRow, me, media);
    }

    void _onRowClick(tagLVRow& lvRow, const QMouseEvent& me, CPath& path) override
    {
        _onMediaClick(lvRow, me, (CMediaRes&)path);
    }

    CMediaSet* _onUpward(CMediaSet& currentMediaSet) override;
    CPath* _onUpward(CPath& currentDir) override;
};
