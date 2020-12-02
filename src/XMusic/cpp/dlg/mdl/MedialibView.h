
#pragma once

#include "MLListView.h"

#include "androidutil.h"

#define __playIconOffset __size10

class CMedialibView : public CMLListView
{
    Q_OBJECT
public:
    CMedialibView(class CMedialibDlg& medialibDlg, CMediaDir& OuterDir);

public:
    QPixmap m_pmDefaultSinger;

private:
    class CMedialibDlg& m_medialibDlg;

    CMediaSet& m_SingerLib;

    CMediaSet& m_PlaylistLib;

    CMediaDir &m_OuterDir;

    cqpm m_pmHDDisk;
    cqpm m_pmLLDisk;

    QPixmap m_pmSingerGroup;
    QPixmap m_pmAlbum;

    QPixmap m_pmPlaylistSet;
    QPixmap m_pmPlaylist;
    QPixmap m_pmPlayItem;

    QPixmap m_pmXmusicDir;
    QPixmap m_pmSSDir;
    QPixmap m_pmSSFile;

    QPixmap m_pmHires;
    QPixmap m_pmDSD;
    QPixmap m_pmMQS;
    QPixmap m_pmDTS;

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

    list<QPixmap> m_lstSingerPixmap;
    map<UINT, QPixmap*> m_mapSingerPixmap;

    int m_nFlashItem = -1;

public:
    void initpm();

    void reset() override
    {
        m_nFlashItem = -1;

        CListView::reset();
    }

    void cleanup();

#if __android
    void showDir(CPath& dir) override
    {
        if (&dir == &m_OuterDir)
        {
            if (!requestAndroidPermission("android.permission.WRITE_EXTERNAL_STORAGE"))
            {
                return;
            }
        }

        CMLListView::showDir(dir);
    }
#endif

    CSinger *currentSinger() const;

    cqpm genSingerHead(UINT uSingerID, cwstr strSingerName);

private:
    void _onShowRoot() override;
    void _onShowMediaSet(CMediaSet&) override;
    void _onShowDir(CPath&) override;

    size_t getColCount() const override;
    size_t getRowCount() const override;

    size_t _getRootItemCount() const override;

    void _genMLItemContext(tagMLItemContext&) override;

    void _onPaint(CPainter& painter, int cx, int cy) override;

    cqrc _paintText(tagLVItemContext&, CPainter&, QRect&, int flags, UINT uShadowAlpha, UINT uTextAlpha) override;

    void _onItemClick(tagLVItem&, const QMouseEvent&, CMediaSet& mediaSet) override;

    void _onItemClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media) override;

    void _onItemClick(tagLVItem& lvItem, const QMouseEvent& me, CPath& path) override;

    CMediaSet* _onUpward(CMediaSet& currentMediaSet) override;
    CPath* _onUpward(CPath& currentDir) override;

    void _flashItem(UINT uItem, UINT uMSDelay=300);
};
