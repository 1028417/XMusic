
#pragma once

#include <QMainWindow>

#include "PlayingList.h"

#include "button.h"

#include "label.h"

#include "medialibdlg.h"

#include "bkgdlg.h"

struct tagPlayingInfo
{
    wstring strTitle;

    int nDuration = -1;

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

    int m_timer = 0;

    CMtxLock<tagPlayingInfo> m_mtxPlayingInfo;

    tagPlayingInfo m_PlayingInfo;
    wstring m_strSingerName;

    bool m_bHScreen = false;

    bool m_bUsingCustomBkg = false;

    bool m_bZoomoutSingerImg = false;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;

signals:
    void signal_showPlaying(unsigned int uPlayingItem, bool bManual);

    void signal_playStoped(bool bOpenFail);

private slots:
    void slot_showPlaying(unsigned int uPlayingItem, bool bManual);

    void slot_playStoped(bool bOpenFail);

    void slot_buttonClicked(CButton*);

    void slot_labelClick(CLabel*, const QPoint&);

public:
    void showLogo();

    void show();

    void updateBkg();

    void drawDefaultBkg(QPainter& painter, const QRect& rc);

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
    void refreshPlayingList(int nPlayingItem, bool bSetActive) override
    {
        (void)bSetActive;
        m_PlayingList.updateList(nPlayingItem);
    }

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;

    void onPlayStoped(E_DecodeStatus decodeStatus) override
    {
        if (decodeStatus != E_DecodeStatus::DS_Cancel)
        {
            emit signal_playStoped(E_DecodeStatus::DS_OpenFail == decodeStatus);
        }
    }
};
