
#pragma once

#include "MLListView.h"

#define __playIconOffset __size10

class CMedialibView : public CMLListView
{
    Q_OBJECT
public:
    CMedialibView(class CMedialibDlg& medialibDlg, CMediaDir& OuterDir);

public:
    CBrush m_brNullSingerHead;

private:
    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaDir &m_OuterDir;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmAlbum;

    QPixmap m_pmPlaylistSet;
    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmXmusicDir;

    struct tagCatItem
    {
        tagCatItem(cqstr qsPng, const wchar_t *pszText)
            : pmIcon(qsPng)
            , strText(pszText)
        {
        }

        QPixmap pmIcon;
        wstring strText;
    };
    tagCatItem m_lpCatItem[UINT(E_SSCatType::CT_Max)+1];

    QPixmap m_pmXpk;

    QPixmap m_pmSSFile;

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

    map<const IMedia*, wstring> m_mapDisplayName;

    map<const void*, std::list<wstring>> m_PlaylistSinger;
    map<const void*, std::list<wstring>> m_mapDirSinger;

public:
    void initpm();

    void reset() override
    {
        m_nFlashItem = -1;

        CListView::reset();
    }

    void cleanup();

#if __android
    void showDir(CPath& dir) override;
#endif

    CSinger *currentSinger() const;

    CBrush& genSingerHead(UINT uSingerID, cwstr strSingerName);

private:
    void _genDisplayTitle(const IMedia *pMedia, const wstring *pstrSingerName);
    void _genDisplayTitle(const IMedia *pMedia);
    void _genSingerMediaTitle(const IMedia *pMedia, cwstr strSingerName);

    inline cqpm _catIcon(E_SSCatType catType) const
     {
         return m_lpCatItem[(UINT)catType].pmIcon;
     }
    inline cwstr _catText(E_SSCatType catType) const
     {
         return m_lpCatItem[(UINT)catType].strText;
     }

    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet&) override;
    void _onShowDir(CPath&) override;

    size_t getColCount() const override;
    size_t getRowCount() const override;

    size_t _getRootItemCount() const override;

    void _genMLItemContext(tagMLItemContext&) override;
    void _genMLItemContext(tagMLItemContext& context, CMediaSet& MediaSet);
    void _genMLItemContext(tagMLItemContext& context, IMedia& Media);
    void _genMLItemContext(tagMLItemContext& context, XFile& file);
    void _genMLItemContext(tagMLItemContext& context, CPath& dir);

    void _onPaint(CPainter& painter, int cx, int cy) override;
    void _paintIcon(tagLVItemContext&, CPainter&, cqrc) override;
    cqrc _paintText(tagLVItemContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha) override;

    void _onItemClick(tagLVItem&, const QMouseEvent&, CMediaSet& mediaSet) override;
    void _onItemClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media) override;

    CMediaSet* _onUpward(CMediaSet& currentMediaSet) override;
    CPath* _onUpward(CPath& currentDir) override;

    void _flashItem(UINT uItem, bool bSelect = false)
    {
        _flashItem(300, uItem, bSelect);
    }
    void _flashItem(UINT uMSDelay, UINT uItem, bool bSelect = false);
};
