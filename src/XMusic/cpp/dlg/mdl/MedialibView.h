
#pragma once

#include "MLListView.h"

#define __playIconOffset __size10

#if __android
#define __OuterDir __sdcardDir
#elif __windows
#define __OuterDir L""
#else
#define __OuterDir fsutil::getHomeDir()
#endif

class COuterDir : public CMediaDir
{
public:
    COuterDir() : CMediaDir(__OuterDir)
    {
    }

    COuterDir(cwstr strPath, CMediaDir& parent)
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
                paSubDir.add(new COuterDir(strDriver, *this));
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
                paSubDir.addFront(new COuterDir(strRoot + fi.strName, *this));
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
                    auto pOuterDir = dynamic_cast<COuterDir*>(pSubDir);
                    if (NULL == pOuterDir)
                    {
                        break;
                    }

                    auto pMediaRes = pOuterDir->subPath(strSubPath, bDir);
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

    COuterDir m_OuterDir;

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

#if __android || __windows
    QPixmap m_pmOuterDir;
#endif

    QPixmap m_pmDirLink;
    QPixmap m_pmDir;
    QPixmap m_pmFile;

    QPixmap m_pmPlay;
    QPixmap m_pmPlayOpacity;
    QPixmap m_pmAddPlay;
    QPixmap m_pmAddPlayOpacity;

    QPixmap m_pmCN;
    QPixmap m_pmHK;
    QPixmap m_pmKR;
    QPixmap m_pmJP;
    QPixmap m_pmEN;

    list<CBrush> m_lstSingerHead;
    map<UINT, CBrush*> m_mapSingerHead;

    int m_nFlashItem = -1;

    map<const IMedia*, wstring> m_mapDisplayTitle;

    map<const void*, std::list<wstring>> m_PlaylistSinger;
    map<const void*, std::list<wstring>> m_mapDirSinger;

private:
    struct tagCatItem
    {
        tagCatItem(cqstr qsPng, const wchar_t *pszCatDetail)
            : pmIcon(qsPng)
            , strCatDetail(pszCatDetail)
        {
        }

        tagCatItem(cqstr qsPng, const wchar_t *pszCatDetail, const wchar_t *pszCatTitle)
            : pmIcon(qsPng)
            , strCatDetail(pszCatDetail)
            , strCatTitle(pszCatTitle)
        {
        }

        QPixmap pmIcon;
        wstring strCatDetail;
        wstring strCatTitle;
    };
    tagCatItem m_lpCatItem[UINT(E_SSCatType::CT_Max)+1];

    inline const tagCatItem& _catItem(CSnapshotDir& dir) const
    {
        return m_lpCatItem[(UINT)dir.catType()];
    }

public:
    void reset() override
    {
        m_nFlashItem = -1;

        CListView::reset();
    }

    void cleanup();

    void initpm();

    CMediaRes* hittestMediaRes(cwstr strPath);

    CSinger *currentSinger() const;

    CBrush& genSingerHead(UINT uSingerID, cwstr strSingerName);

private:
    void _genDisplayTitle(const IMedia *pMedia, cwstr strSingerName);
    void _genDisplayTitle(const IMedia *pMedia);
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
