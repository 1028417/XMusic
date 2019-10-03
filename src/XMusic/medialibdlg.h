
#pragma once

#include "dialog.h"

#include "button.h"

#include "ListViewEx.h"

class CRootDir : public CMediaRes
{
public:
    CRootDir()
        : CMediaRes(L"/sdcard")
    {
        ((tagFileInfo&)m_FileInfo).pParent = &m_root;
    }

private:
    CMediaRes m_root;

private:
    CPath* _newSubPath(const tagFileInfo& FileInfo) override
    {
        if (FileInfo.bDir)
        {
            if (wsutil::matchIgnoreCase(FileInfo.strName, L"XMusic"))
            {
                return NULL;
            }
        }

        return CMediaRes::_newSubPath(FileInfo);
    }
};

class CMedialibView : public CListViewEx
{
public:
    CMedialibView(class CApp& app, class CMedialibDlg& medialibDlg);

private:
    class CApp& m_app;

    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaRes& m_MediaLib;

    CRootDir m_rootDir;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmDefaultSinger;
    QPixmap m_pmAlbum;
    QPixmap m_pmAlbumItem;

    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmDir;
    QPixmap m_pmDirLink;
    QPixmap m_pmFile;

    list<QPixmap> m_lstSingerPixmap;
    map<CSinger*, QPixmap*> m_mapSingerPixmap;

public:
    void init();

    void showFile(const wstring& strPath);

    void play();

private:
    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet& MediaSet) override;
    void _onShowPath(CPath& path) override;

    size_t getPageRowCount() override;

    size_t getColumnCount() override;

    size_t getRootCount() override;

    bool _genRootRowContext(const tagLVRow&, tagMediaContext&) override;
    void _genMediaContext(tagMediaContext&) override;

    const QPixmap& _getSingerPixmap(CSinger&);

    void _getTitle(CMediaSet&, WString& strTitle);
    void _getTitle(CMediaRes&, WString& strTitle);

    void _onMediaClick(const tagLVRow&, IMedia&);

    void _onRowClick(const tagLVRow& lvRow, CMedia& media) override
    {
        _onMediaClick(lvRow, media);
    }

    void _onRowClick(const tagLVRow& lvRow, CPath& path) override
    {
        _onMediaClick(lvRow, (CMediaRes&)path);
    }

    bool _onUpward() override;
};

class CMedialibDlg : public CDialog
{
    Q_OBJECT
public:
    CMedialibDlg(class CApp& m_app);

private:
    class CApp& m_app;

    CMedialibView m_MedialibView;

public:
    void init();

    void show()
    {
        CDialog::show();
        m_MedialibView.showRoot();
    }

    void showMediaSet(CMediaSet& MediaSet)
    {
        CDialog::show();
        m_MedialibView.showMediaSet(MediaSet);
    }

    void showMedia(CMedia& media)
    {
        CDialog::show();
        m_MedialibView.showMedia(media);
    }

    void showFile(const wstring& strPath)
    {
        CDialog::show();
        m_MedialibView.showFile(strPath);
    }

    void updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton);

private:
    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.upward();
    }
};
