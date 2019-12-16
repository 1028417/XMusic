
#pragma once

#include <QMainWindow>

#include "PlayingList.h"

#include "button.h"

#include "label.h"

#include "medialibdlg.h"

#include "bkgdlg.h"

struct tagPlayingInfo
{
    UINT uPlaySeq = 0;

    wstring strTitle;

    int nDuration = -1;
    UINT uStreamSize = 0;

    wstring strSinger;
    UINT uSingerID = 0;

    wstring strAlbum;
    UINT uRelatedAlbumItemID = 0;

    wstring strPlaylist;
    UINT uRelatedPlayItemID = 0;

    wstring strPath;
};

class MainWindow : public QMainWindow, public IModelObserver
{
    Q_OBJECT
public:
    MainWindow(class CXMusicApp& app);

private:
    class CXMusicApp& m_app;

    class CPlayingList m_PlayingList;

    CMedialibDlg m_medialibDlg;

    CBkgDlg m_bkgDlg;

    map<QWidget*, QRect> m_mapTopWidgetPos;

    map<QWidget*, QRect> m_mapWidgetPos;
    map<QWidget*, QRect> m_mapWidgetNewPos;

    CMtxLock<tagPlayingInfo> m_mtxPlayingInfo;

    tagPlayingInfo m_PlayingInfo;
    wstring m_strSingerName;

    bool m_bHScreen = false;

    bool m_bUseDefaultBkg = false;

    bool m_bZoomoutSingerImg = false;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;

signals:
    void signal_updatePlayingList(int nPlayingItem);

    void signal_showPlaying(unsigned int uPlayingItem, bool bManual);

    void signal_playStoped(bool bOpenFail);

    void signal_updateSingerImg();

private slots:
    void slot_updatePlayingList(int nPlayingItem)
    {
        m_PlayingList.updateList(nPlayingItem);
    }

    void slot_showPlaying(unsigned int uPlayingItem, bool bManual);

    void slot_playStoped(bool bOpenFail);

    void slot_buttonClicked(CButton*);

    void slot_labelClick(CLabel*, const QPoint&);

public:
    void showLogo();

    void show();

    void updateBkg();

    void drawDefaultBkg(CPainter& painter, const QRect& rc, UINT xround=0, UINT yround=0);

private:
    bool event(QEvent *) override;

    void _onPaint(CPainter& painter);

    void _init();

    void _relayout();

    void _showAlbumName();

    void _updatePlayPauseButton(bool bPlaying);

    void _playSingerImg(bool bReset);

    void _demand(CButton* btnDemand);

private:
    void onPlayingListUpdated(int nPlayingItem, bool bSetActive) override
    {
        (void)bSetActive;
        emit signal_updatePlayingList(nPlayingItem);
    }

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;

    void onPlayStoped(E_DecodeStatus decodeStatus) override;

    void onSingerImgDownloaded(const wstring& strFile) override
    {
        (void)strFile;

        emit signal_updateSingerImg();
    }
};
