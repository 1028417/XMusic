
#pragma once

#include "dialog.h"

#include "button.h"

#include "ListViewEx.h"

class CMedialibView : public CListViewEx
{
public:
    CMedialibView(class CXMusicApp& app, class CMedialibDlg& medialibDlg, CMediaDir *pOuterDir=NULL);

private:
    class CXMusicApp& m_app;

    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    XMediaLib& m_MediaLib;

    CMediaDir *m_pOuterDir = NULL;

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

    void showFile(const wstring& strFile);

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
    void _getTitle(CMediaDir&, WString& strTitle);

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

class CAndroidDir : public CMediaDir
{
public:
    CAndroidDir()
        : CMediaDir(L"/sdcard")
    {
        fileInfo().pParent = &m_root;
    }

    CAndroidDir(const tagFileInfo& fileInfo)
        : CMediaDir(fileInfo)
    {
    }

private:
    static CMediaDir m_root;

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override
    {
        if (wsutil::matchIgnoreCase(fileInfo.strName, L"XMusic"))
        {
            return NULL;
        }

        return new CAndroidDir(fileInfo);
    }

    wstring GetPath() const override
    {
        auto pParent = fileinfo.pParent;
        if (NULL == pParent || &m_root == pParent)
        {
            return L"..";
        }

        WString strOppPath(pParent->oppPath());
        strOppPath << __wcFSSlant << fileinfo.strName;
        return std::move(strOppPath);
    }
};

class CMedialibDlg : public CDialog
{
    Q_OBJECT
public:
    CMedialibDlg(class CXMusicApp& m_app);

private:
    class CXMusicApp& m_app;

#if __android
    CAndroidDir m_AndroidRootDir;
    CMediaDir *m_pOuterDir = &m_AndroidRootDir;
#else
    CMediaDir *m_pOuterDir = NULL;
#endif

    CMedialibView m_MedialibView;

public:
    void init();

    void show()
    {
        m_MedialibView.showRoot();

        _show();
    }

    void showMediaSet(CMediaSet& MediaSet)
    {
        m_MedialibView.showMediaSet(MediaSet);

        _show();
    }

    void showMedia(CMedia& media)
    {
        m_MedialibView.showMedia(media);

        _show();
    }

    void showFile(const wstring& strPath)
    {
        m_MedialibView.showFile(strPath);

        _show();
    }

    void updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton);

private:
    void _show()
    {
        timerutil::async(0, [&](){
            _resizeTitle();
        });

        CDialog::show();
    }

    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.upward();
    }
};
