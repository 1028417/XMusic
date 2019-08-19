
#pragma once

#include "dialog.h"

#include "button.h"

#include "ListViewEx.h"

class CMedialibView : public CListViewEx
{
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

    void showFile(const wstring& strPath);

    void upward();

    void play();

private:
    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet& MediaSet) override;
    void _onShowPath(CPath& path) override;

    UINT getPageRowCount() override;

    UINT getColumnCount() override;

    UINT getRootCount() override;

    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CMediaRes&, WString& strTitle);

    bool _genRootRowContext(const tagLVRow&, tagRowContext&) override;
    void _genRowContext(tagRowContext&) override;

    void _onPaintRow(CPainter&, QRect&, const tagLVRow&, const tagRowContext&) override;

    QPixmap& _getSingerPixmap(CSinger&);

    void _onMediaClick(const tagLVRow&, IMedia&);

    void _onRowClick(const tagLVRow& lvRow, CMedia& media) override
    {
        _onMediaClick(lvRow, media);
    }

    void _onRowClick(const tagLVRow& lvRow, CPath& path) override
    {
        _onMediaClick(lvRow, (CMediaRes&)path);
    }
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

    void showMediaSet(CMediaSet& MediaSet)
    {
        CDialog<>::show();
        m_MedialibView.showMediaSet(MediaSet);
    }

    void showMedia(CMedia& media)
    {
        CDialog<>::show();
        m_MedialibView.showMedia(media);
    }

    void showFile(const wstring& strPath)
    {
        CDialog<>::show();
        m_MedialibView.showFile(strPath);
    }

    void update(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton);

private:
    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override;
};
