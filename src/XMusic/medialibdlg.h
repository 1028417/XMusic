
#pragma once

#include "dialog.h"

#include "button.h"

#include "ListViewEx.h"

class CMedialibView : public CListViewEx
{
public:
    CMedialibView(class CXMusicApp& app, class CMedialibDlg& medialibDlg, CMediaRes *pOuterDir=NULL);

private:
    class CXMusicApp& m_app;

    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaRes& m_MediaLib;

    CMediaRes *m_pOuterDir = NULL;

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

class CAndroidRootDir : public CMediaRes
{
public:
    CAndroidRootDir()
        : CMediaRes(L"/sdcard")
    {
        fileInfo().pParent = &m_root;
    }

    CAndroidRootDir(const tagFileInfo& fileInfo, E_MediaFileType eFileType)
        : CMediaRes(fileInfo, eFileType)
    {
    }

private:
    static CMediaRes m_root;

private:
    CPath* _newSubPath(const tagFileInfo& fileInfo) override
    {
        if (fileInfo.bDir)
        {
            if (wsutil::matchIgnoreCase(fileInfo.strName, L"XMusic"))
            {
                return NULL;
            }
        }

        return CMediaRes::_genSubPath<CAndroidRootDir>(fileInfo);
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
        return strOppPath;
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
    CAndroidRootDir m_AndroidRootDir;
    CMediaRes *m_pOuterDir = &m_AndroidRootDir;
#else
    CMediaRes *m_pOuterDir = NULL;
#endif

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
