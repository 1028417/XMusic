#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "PlayingList.h"

#include "button.h"

#include "label.h"

//namespace Ui { class MainWindow; }

class MainWindow : public QMainWindow, public IModelObserver
{
    Q_OBJECT

public:
    explicit MainWindow(class CPlayerView& view);

private:
    class CPlayerView& m_view;

    class CPlayingList m_PlayingList;

    map<QWidget*, QRect> m_mapWidgetPos;
    map<QWidget*, QRect> m_mapWidgetNewPos;

    int m_timer = 0;

    tagPlayingItem m_PlayingItem;
    wstring m_strSingerName;

    WString m_strHBkgDir;
    WString m_strVBkgDir;

    list<wstring> m_lstHBkg;
    list<wstring> m_lstVBkg;

    QPixmap m_HBkgPixmap;
    QPixmap m_VBkgPixmap;

    bool m_bUsingCustomBkg = false;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;
    E_DemandMode m_eDemandMode = E_DemandMode::DM_Null;

signals:
    void signal_showPlaying(unsigned int uPlayingItem, bool bManual);
    void signal_playFinish();

private slots:
    void slot_showPlaying(unsigned int uPlayingItem, bool bManual);
    void slot_playFinish();

    void slot_buttonClicked(CButton*);

    void slot_labelClicked(CLabel*);

    void slot_progressMousePress(CLabel*, const QPoint& pos);

private slots:
    void timerEvent(QTimerEvent *) override;

    bool event(QEvent *) override;

public:
    void showLogo();
    void show();

private:
    void _init();

    void _relayout();

    void _showAlbumName(tagPlayingItem& PlayingItem);

    void _updatePlayPauseButton(bool bPlaying);

    void _playSingerImg(bool bReset);

    void _showSingerImg(const QPixmap& pixmap);

    void _demand(CButton* btnDemand);

private:
    void refreshPlayingList(int nPlayingItem, bool bSetActive = false) override;

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;
    void onPlayFinish() override;
};

#endif // MAINWINDOW_H
