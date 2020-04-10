
#pragma once

#include <QMainWindow>

#include "label.h"

#include "button.h"

#include "PlayingList.h"

#include "medialibdlg.h"

#include "bkgdlg.h"

struct tagPlayingInfo
{
    wstring strTitle;

    int nDuration = -1;
    UINT uStreamSize = 0;

    QString qsQuality;

    wstring strSingerName;
    UINT uSingerID = 0;

    wstring strAlbum;
    UINT uRelatedAlbumItemID = 0;

    wstring strPlaylist;
    UINT uRelatedPlayItemID = 0;

    CMedia *pRelatedMedia = NULL;

    wstring strPath;
};
Q_DECLARE_METATYPE(tagPlayingInfo);

class MainWindow : public QMainWindow, public IModelObserver
{
    Q_OBJECT
public:
    MainWindow(class CApp& app);

private:
    class CApp& m_app;

    class CPlayingList m_PlayingList;

    CMedialibDlg m_medialibDlg;

    CBkgDlg m_bkgDlg;

    map<QWidget*, QRect> m_mapTopWidgetPos;

    map<QWidget*, QRect> m_mapWidgetPos;
    map<QWidget*, QRect> m_mapWidgetNewPos;

    UINT m_uPlaySeq = 0;
    tagPlayingInfo m_PlayingInfo;

    bool m_bHScreen = false;

    bool m_bUseDefaultBkg = false;

    int m_dxbkg = 0;
    int m_dybkg = 0;

    bool m_bZoomoutSingerImg = false;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;

signals:
    void signal_updatePlayingList(int nPlayingItem);

    void signal_showPlaying(unsigned int uPlayingItem, bool bManual, QVariant var);

    void signal_playStoped(bool bOpenFail);

    void signal_updateSingerImg();

private slots:
    void slot_updatePlayingList(int nPlayingItem)
    {
        m_PlayingList.updateList(nPlayingItem);
    }

    void slot_showPlaying(unsigned int uPlayingItem, bool bManual, QVariant var);

    void slot_playStoped(bool bOpenFail);

    void slot_buttonClicked(CButton*);

    void slot_labelClick(CLabel*, const QPoint&);

public:
#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif

    bool isHScreen() const
    {
        return m_bHScreen;
    }

    void showLogo();

    void initBkg();
    void show();

    void updateBkg();

    void handleTouchMove(const CTouchEvent& te);

    void drawDefaultBkg(CPainter& painter, cqrc rc, UINT xround=0, UINT yround=0);

    const tagPlayingInfo& playingInfo() const
    {
        return m_PlayingInfo;
    }

private:
    void _init();

    bool event(QEvent *) override;

    void _onPaint(CPainter& painter);

    void _relayout();

    void _updateLogoTip();
    void _showUpgradeProgress();

    void _updateLogoCompany(int nAlphaOffset, cfn_void cb=NULL);

    void _updatePlayPauseButton(bool bPlaying);

    void _updateProgress();

    void _showAlbumName();

    void _playSingerImg(bool bReset);

    void _demand(CButton* btnDemand);

private:
    void onPlayingListUpdated(int nPlayingItem, bool bSetActive) override
    {
        (void)bSetActive;
        emit signal_updatePlayingList(nPlayingItem);
    }

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;

    void onPlayStop(bool bCanceled, bool bOpenFail) override;

    void onSingerImgDownloaded() override
    {
        emit signal_updateSingerImg();
    }
};
