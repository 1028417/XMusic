
#pragma once

#include <QMainWindow>

#include "label.h"

#include "button.h"

#include "PlayingList.h"

#include "medialibdlg.h"

#include "bkgdlg.h"

#define __crLogoText 100, 150, 255

struct tagPlayingInfo
{
    QString qsTitle;

    wstring strPath;

    UINT uDuration = 0;
    bool bWholeTrack = false;

    UINT uStreamSize = 0;

    E_MediaQuality eQuality = E_MediaQuality::MQ_None;
    QString qsQuality;

    wstring strSingerName;
    UINT uSingerID = 0;

    wstring strAlbum;
    UINT uRelatedAlbumItemID = 0;

    wstring strPlaylist;
    UINT uRelatedPlayItemID = 0;

    CMedia *pRelatedMedia = NULL;
};

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

    bool m_bHLayout = false;

    bool m_bDefaultBkg = false;

    int m_dxbkg = 0;
    int m_dybkg = 0;

    enum class E_SingerImgPos
    {
        SIP_Float = 1,
        SIP_Dock,
        SIP_Zoomout
    };

    E_SingerImgPos m_eSingerImgPos = E_SingerImgPos::SIP_Float;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;

private slots:
    void slot_buttonClicked(CButton*);

    void slot_labelClick(CLabel*, const QPoint&);

public:
    const tagPlayingInfo& playingInfo() const
    {
        return m_PlayingInfo;
    }

#if __windows
    HWND hwnd() const
    {
        return (HWND)winId();
    }
#endif

    void showLogo();

    void preinit()
    {
        m_medialibDlg.preinit();

        m_bkgDlg.preinit();
    }

    void show();

    void switchFullScreen();

    void updateBkg();

    void handleTouchEvent(E_TouchEventType type, const CTouchEvent& te);

    void drawDefaultBkg(CPainter& painter, cqrc rc, UINT xround=0, UINT yround=0);

    void playSingerImg(bool bReset);

private:
    void _init();

    bool event(QEvent *) override;

    void _onPaint();

    void _relayout();

    void _showUpgradeProgress();

    void _updateLogoCompany(int nAlphaOffset, cfn_void cb=NULL);

    void _updatePlayPauseButton(bool bPlaying);

    void _updateProgress();

    WString _genAlbumName();

    void _demand(CButton* btnDemand);

    void _handleTouchEnd(const CTouchEvent& te);

private:
    void onPlayingListUpdated(int nPlayingItem, bool bSetActive) override;

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;

    void onPlayStop(bool bCanceled, bool bOpenFail) override;

    void onSingerImgDownloaded() override;
};
