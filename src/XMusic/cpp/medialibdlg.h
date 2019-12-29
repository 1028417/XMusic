
#pragma once

#include "dialog.h"

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

    list<QPixmap> m_lstSingerPixmap;
    map<UINT, QPixmap*> m_mapSingerPixmap;

    map<UINT, class CButton*> m_mapButton;
    map<class CButton*, UINT> m_mapButtonIdx;

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

    bool _genRootRowContext(tagMediaContext&) override;
    void _genMediaContext(tagMediaContext&) override;

    bool event(QEvent *ev) override;

    void _showButton(tagLVRow& lvRow, bool bMedia);
    void _onButton(UINT uRow);

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
#if __windows
        m_strMediaLibDir = strMediaLibDir;
#else
        m_strMediaLibDir = fsutil::GetFileName(strMediaLibDir);
#endif

        m_strOuterDir = strOuterDir;

        CPath::setDir(strMediaLibDir + strOuterDir);
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
        return m_strOuterDir + CMediaDir::GetPath();
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
    CMedialibDlg(class CApp& m_app);

private:
    class CApp& m_app;

    COuterDir m_OuterDir;

    CMedialibView m_MedialibView;

public:
    size_t getPageRowCount();

    void init();

    void show()
    {
        _initOuter();

        m_MedialibView.showRoot();

        _show();
    }

    void showMediaSet(CMediaSet& MediaSet)
    {
        _initOuter();

        m_MedialibView.showMediaSet(MediaSet);

        _show();
    }

    void showMedia(CMedia& media)
    {
        _initOuter();

        m_MedialibView.showMedia(media);

        _show();
    }

    bool showFile(const wstring& strPath)
    {
        _initOuter();

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
    void _initOuter();

    void _show();

    void _relayout(int cx, int cy) override;

    void _resizeTitle() const;

    bool _handleReturn() override
    {
        return m_MedialibView.upward();
    }

    void _onClose() override;
};
