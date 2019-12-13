
#pragma once

#include "dialog.h"

#include "button.h"

#include "ListViewEx.h"

class CMedialibView : public CListViewEx
{
    Q_OBJECT
public:
    CMedialibView(class CXMusicApp& app, class CMedialibDlg& medialibDlg, CMediaDir& OuterDir);

private:
    class CXMusicApp& m_app;

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

    list<QPixmap> m_lstSingerPixmap;
    map<UINT, QPixmap*> m_mapSingerPixmap;

    map<UINT, CButton*> m_mapButton;

signals:
    void signal_update();

public:
    void init();

    bool showFile(const wstring& strFile);

    void play();

    void clear();

    void updateSingerImg();

    void update() override;

private:
    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet& MediaSet) override;
    void _onShowPath(CPath& path) override;

    size_t getPageRowCount() override;

    size_t getColumnCount() override;

    size_t _getRootRowCount() override;

    bool _genRootRowContext(const tagLVRow&, tagMediaContext&) override;
    void _genMediaContext(tagMediaContext&) override;

    bool event(QEvent *ev) override;

    void _showButton(tagLVRow& lvRow);

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

class COuterDir : public CMediaDir
{
public:
    COuterDir() {}

    COuterDir(const tagFileInfo& fileInfo, const wstring& strMediaLibDir)
        : CMediaDir(fileInfo)
        , m_strMediaLibDir(strMediaLibDir)
    {
    }

private:
    wstring m_strMediaLibDir;

    wstring m_strOuterDir;

public:
    void setDir(const wstring& strMediaLibDir, const wstring& strOuterDir)
    {
        m_strOuterDir = strOuterDir;
        CPath::setDir(strMediaLibDir + strOuterDir);

#if __windows
        m_strMediaLibDir = strMediaLibDir;
#else
        m_strMediaLibDir = fsutil::GetFileName(strMediaLibDir);
#endif
    }

private:
    CPath* _newSubDir(const tagFileInfo& fileInfo) override
    {
#if __windows
        wstring strSubDir = this->absPath() + __wcDirSeparator + fileInfo.strName;
        QString qsSubDir = QDir(strutil::toQstr(strSubDir)).absolutePath();
        strSubDir = QDir::toNativeSeparators(qsSubDir).toStdWString();
        if (strutil::matchIgnoreCase(strSubDir, m_strMediaLibDir))
        {
            return NULL;
        }

        return new COuterDir(fileInfo, m_strMediaLibDir);

#else
        if (fileInfo.strName == m_strMediaLibDir)
        {
            return NULL;
        }

        return new CMediaDir(fileInfo);
#endif
    }

    wstring GetPath() const override
    {
        if (fileinfo.pParent)
        {
            return m_strOuterDir;
        }

        return CMediaDir::GetPath();
    }

    CMediaRes* findSubFile(const wstring& strSubFile) override
    {
        if (__substr(strSubFile, 1, 2) != L"..")
        {
            return NULL;
        }

        cauto t_strSubFile = __substr(strSubFile, m_strOuterDir.size());
        //strutil::replace_r(strutil::replace_r(strSubFile, L"/.."), L"\\..");

        return CMediaDir::findSubFile(t_strSubFile);
    }
};

class CMedialibDlg : public CDialog
{
    Q_OBJECT
public:
    CMedialibDlg(class CXMusicApp& m_app);

private:
    class CXMusicApp& m_app;

    COuterDir m_OuterDir;

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

    bool showFile(const wstring& strPath)
    {
        if (!m_MedialibView.showFile(strPath))
        {
            return false;
        }

        _show();

        return true;
    }

    void updateHead(const wstring& strTitle, bool bShowPlayButton, bool bShowUpwardButton);

    void updateSingerImg()
    {
        m_MedialibView.updateSingerImg();
    }

private:
    void _show();

    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.upward();
    }

    void _onClose() override;
};
