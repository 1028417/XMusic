
#pragma once

#include <QMainWindow>

#include "PlayingList.h"

#include "button.h"

#include "label.h"

#include "medialibdlg.h"

//namespace Ui { class MainWindow; }

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
    explicit MainWindow(class CPlayerView& view);

private:
    class CPlayerView& m_view;

    class CPlayingList m_PlayingList;

    map<QWidget*, QRect> m_mapTopWidgetPos;

    map<QWidget*, QRect> m_mapWidgetPos;
    map<QWidget*, QRect> m_mapWidgetNewPos;

    int m_timer = 0;

    tagPlayingInfo m_PlayingInfo;
    wstring m_strSingerName;

    WString m_strHBkgDir;
    WString m_strVBkgDir;

    list<wstring> m_lstHBkg;
    list<wstring> m_lstVBkg;

    QPixmap m_HBkgPixmap;
    QPixmap m_VBkgPixmap;

    bool m_bUsingCustomBkg = false;

    bool m_bZoomoutSingerImg = false;

    bool m_bHScreen = false;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;

    CMedialibDlg m_medialibDlg;

signals:
    void signal_showPlaying(unsigned int uPlayingItem, bool bManual);
    void signal_playFinish();

private slots:
    void slot_showPlaying(unsigned int uPlayingItem, bool bManual);
    void slot_playFinish();

    void slot_buttonClicked(CButton*);

    void slot_labelClick(CLabel*, const QPoint&);

public:
    void showLogo();

    void show();

private:
    bool event(QEvent *) override;

    void _init();

    void _relayout();

    void _showAlbumName();

    void _updatePlayPauseButton(bool bPlaying);

    void _playSingerImg(bool bReset);

    void _demand(CButton* btnDemand);

private:
    void refreshPlayingList(int nPlayingItem, bool bSetActive = false) override;

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;
    void onPlayFinish() override;
};
