
#pragma once

#include "MLListView.h"

#define __playIconOffset __size10

class CLocalDir : public CMediaDir
{
public:
#if __windows
    CLocalDir() = default;
#elif __android
    CLocalDir() : CMediaDir(__sdcardDir) {}
#else
    CLocalDir() : CMediaDir(fsutil::getHomeDir()) {}
#endif

    CLocalDir(cwstr strPath, CMediaDir& parent)
        : CMediaDir(strPath, &parent)
    {
    }

private:
    wstring path() const override
    {
        return m_fi.strName;
    }

    wstring GetPath() const override // 所有平台都需要
    {
        return m_fi.strName;
    }

#if __windows
    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override
    {
        if (NULL == m_fi.pParent)
        {
            std::list<std::wstring> lstDrivers;
            winfsutil::getSysDrivers(lstDrivers);
            for (cauto strDriver : lstDrivers)
            {
                paSubDir.add(new CLocalDir(strDriver, *this));
            }
        }
        else
        {
            CMediaDir::_onFindFile(paSubDir, paSubFile);
        }
    }

#else
#if __android
    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override
    {
        CMediaDir::_onFindFile(paSubDir, paSubFile);

        if (NULL == m_fi.pParent)
        {
            auto strRoot = L"/storage/";
            (void)fsutil::findSubDir(strRoot, [&](tagFileInfo& fi) {
                paSubDir.addFront(new CLocalDir(strRoot + fi.strName, *this));
            });
        }
    }
#endif

    CMediaRes* subPath(cwstr strSubPath, bool bDir) override
    {
        cauto strOppPath = fsutil::GetOppPath(m_fi.strName, strSubPath);
        if (strOppPath.empty())
        {
#if __android
            if (NULL == m_fi.pParent)
            {
                for (auto pSubDir : dirs())
                {
                    auto pLocalDir = dynamic_cast<CLocalDir*>(pSubDir);
                    if (NULL == pLocalDir)
                    {
                        break;
                    }

                    auto pMediaRes = pLocalDir->subPath(strSubPath, bDir);
                    if (pMediaRes)
                    {
                        return pMediaRes;
                    }
                }
            }
#endif

            return NULL;
        }

        return (CMediaRes*)CMediaDir::subPath(strOppPath, bDir);
    }
#endif

    CPath* _newSubDir(const tagFileInfo& fileInfo) override;
};

class CMedialibView : public CMLListView
{
    Q_OBJECT
public:
    CMedialibView(class CMedialibDlg& medialibDlg);

public:
    CBrush m_brNullSingerHead;

private:
    class CMedialibDlg& m_medialibDlg;

    CMediaDir& m_xpkRoot;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CLocalDir m_LocalDir;

    QPixmap m_pmXpk;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmAlbum;

    QPixmap m_pmPlaylistSet;
    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmXmusicDir;

    QPixmap m_pmMedia;
    //QPixmap m_pmXpkMedia;
    QPixmap m_pmFlac;

    cqpm m_pmHDDisk;
    cqpm m_pmSQDisk;

    QPixmap m_pmDir;
    QPixmap m_pmFile;

#if __android || __windows
    QPixmap m_pmLocalDir;
#endif
    QPixmap m_pmDirLink;

    QPixmap m_pmPlay;
    QPixmap m_pmAddPlay;

    map<E_LanguageType, QPixmap> m_mapLanguageIcon;
    map<wstring, E_LanguageType> m_mapLanguage;

    list<CBrush> m_lstSingerHead;
    map<UINT, CBrush*> m_mapSingerHead;

    int m_nFlashItem = -1;

    map<const IMedia*, wstring> m_mapMediaTitle;

    map<const void*, std::list<UINT>> m_PlaylistSinger;
    map<const void*, std::list<UINT>> m_mapDirSinger;

    bool m_bShowXpkRoot = false;

private:
    enum class E_MdlRootType
    {
        RT_Singer = 0,
        RT_Playlist,
        RT_XMusic,
        RT_Xpk,
        RT_Local,
        RT_Max
    };
    struct tagRootItem
    {
        tagRootItem(cqpm pmIcon, cwstr strTitle, CMediaSet& mediaSet)
            : pmIcon(pmIcon)
            , strTitle(strTitle)
            , pMediaSet(&mediaSet)
        {
        }

        tagRootItem(cqpm pmIcon, cwstr strTitle, CPath& dir)
            : pmIcon(pmIcon)
            , strTitle(strTitle)
            , pDir(&dir)
        {
        }

        void resetPos(int _nHRow=-1, int _nHCol=-1, int _nVRow=-1)
        {
            nHRow = _nHRow;
            nHCol = _nHCol;
            nVRow = _nVRow;
        }

        bool checkPos(bool bHLayout, UINT uRow, UINT uCol) const
        {
            if (bHLayout)
            {
                return (int)uRow == nHRow && (int)uCol == nHCol;
            }
            else
            {
                return (int)uRow == nVRow;
            }
        }

        void setContext(tagMLItemContext& context, int szIcon) const
        {
            context.setIcon(pmIcon, szIcon);
            context.strText = strTitle;
            context.pMediaSet = pMediaSet;
            context.pDir = pDir;
        }

        cqpm pmIcon;

        wstring strTitle;

        CMediaSet *pMediaSet = NULL;

        CPath *pDir = NULL;

        int nHRow = -1;
        int nHCol = -1;

        int nVRow = -1;
    };
    tagRootItem m_lpRootItem[(UINT)E_MdlRootType::RT_Max];

    tagRootItem& _rootItem(E_MdlRootType eRootType)
    {
        return m_lpRootItem[(UINT)eRootType];
    }

    const tagRootItem* _rootItem(CPath& dir) const
    {
        for (cauto rootItem : m_lpRootItem)
        {
            if (rootItem.pDir == &dir)
            {
                return &rootItem;
            }
        }
        return NULL;
    }

    const tagRootItem* _rootItem(bool bHLayout, UINT uRow, UINT uCol) const
    {
        for (cauto rootItem : m_lpRootItem)
        {
            if (rootItem.checkPos(bHLayout, uRow, uCol))
            {
                return &rootItem;
            }
        }
        return NULL;
    }

private:
    struct tagCatItem
    {
        tagCatItem(cqstr qsIcon, const wchar_t *pszCatDetail)
            : pmIcon(qsIcon)
            , strCatDetail(pszCatDetail)
        {
        }

        tagCatItem(cqstr qsIcon, const wchar_t *pszCatDetail, const wchar_t *pszCatTitle)
            : pmIcon(qsIcon)
            , strCatDetail(pszCatDetail)
            , strCatTitle(pszCatTitle)
        {
        }

        QPixmap pmIcon;
        wstring strCatDetail;
        wstring strCatTitle;
    };
    tagCatItem m_lpCatItem[UINT(E_MdlCatType::CT_Max)+1];

    inline cqpm _catIcon(const CSnapshotDir& dir) const
    {
        return m_lpCatItem[(UINT)dir.catType()].pmIcon;
    }
    inline wstring _catTitle(const CSnapshotDir& dir) const
    {
        cauto catTitle = m_lpCatItem[(UINT)dir.catType()].strCatTitle;
        if (!catTitle.empty())
        {
            return catTitle;
        }
        return dir.catName();
    }

    inline const tagCatItem* _catItem(CPath& dir) const
    {
        auto pSnapshotDir = _snapshotDir(dir);
        if (NULL == pSnapshotDir)
        {
            return NULL;
        }
        return &m_lpCatItem[(UINT)pSnapshotDir->catType()];
    }

public:
    void cleanup();

    void reset() override
    {
        m_nFlashItem = -1;

        CListView::reset();
    }

    void resetRootItem(bool bShowXpkRoot);

    void initpm();

    cqpm pmXpk() const
    {
        return m_pmXpk;
    }

    CMediaRes* hittestLocalFile(cwstr strPath);

    CSinger *currentSinger() const;

    CBrush& genSingerHead(UINT uSingerID);

    inline wstring genAttachTitle(const CSnapshotDir& dir) const
    {
        return _catTitle(dir) + __CNDot + dir.fileName();
    }

private:
    void _genMediaTitle(const IMedia *pMedia, cwstr strSingerName);
    void _genMediaTitle(const IMedia *pMedia);
    void _genSingerMediaTitle(const IMedia *pMedia, CSinger& singer);

    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet&) override;
    void _onShowDir(CPath&) override;

    size_t getColCount() const override;
    size_t getRowCount() const override;

    size_t _getRootItemCount() const override;

    void _genMLItemContext(tagMLItemContext&) override;

    void _genMediaSetContext(tagMLItemContext& context, CMediaSet& MediaSet);
    void _genMediaContext(tagMLItemContext& context, IMedia& Media);
    void _genFileContext(tagMLItemContext& context, XFile& file);
    void _genDirContext(tagMLItemContext& context, CPath& dir);

    void _onPaint(CPainter& painter, int cx, int cy) override;
    void _paintIcon(tagLVItemContext&, CPainter&, cqrc) override;
    cqrc _paintText(tagLVItemContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha) override;

    void _onMediasetClick(tagLVItem&, const QMouseEvent&, CMediaSet& mediaSet) override;
    void _onMediaClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media) override;
    void _onDirClick(tagLVItem&, const QMouseEvent&, CPath& dir) override;

    CMediaSet* _onUpward(CMediaSet& currentMediaSet) override;

    void _flashItem(UINT uItem, bool bSelect = false)
    {
        _flashItem(300, uItem, bSelect);
    }
    void _flashItem(UINT uMSDelay, UINT uItem, bool bSelect = false);
};
