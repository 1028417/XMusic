
#pragma once

#include <QMainWindow>

#include "widget/label.h"

#include "widget/button.h"

#include "dlg/mdl/medialibdlg.h"

#include "dlg/bkg/bkgdlg.h"

#include "PlayingList.h"

#define __crLogoText 100, 150, 255

#define __cxBkg 1080
#define __cyBkg 2340

struct tagPlayingInfo
{
    wstring strPath;

#if __OnlineMediaLib
    UINT uFileSize = 0;
#endif

    QString qsDuration = 0;

    //E_TrackType eTrackType = E_TrackType::TT_Single;

    CSinger *pSinger = NULL;

    IMedia *pIMedia = NULL;
    wstring strMediaSet;

    const CMediaRes *pXpkMediaRes = NULL;
};

enum class E_SingerImgPos
{
    SIP_Float = 0,
    SIP_Dock,
    SIP_Zoomout
};

extern bool g_bHLayout;

class MainWindow : public QMainWindow, public IModelObserver, private CXObj
{
    Q_OBJECT
public:
    MainWindow();

private:
    tagOption& m_opt;

    bool m_bSingerDemandable = false;
    bool m_bAlbumDemandable = false;
    bool m_bPlaylistDemandable = false;

    map<E_LanguageType, set<E_DemandMode>> m_mapDemandLanguage;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;

#if !__android
    CTipLabel m_labelLoginTip;
#endif

    QBrush m_brBkg;
    QPixmap m_pmCDCover;
    bool m_bDefaultBkg = false;

    tagPlayingInfo m_PlayingInfo;

    CPlayingList m_PlayingList;

    CMedialibDlg m_medialibDlg;

    CBkgDlg m_bkgDlg;

    bool m_bStopLogo = false;
    UINT m_uShowLogoState = 0;

    UINT m_uSingerImgIdx = 0;

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

    CBkgDlg& bkgDlg()
    {
        return m_bkgDlg;
    }

    void exec();

    void showLogo();
    void startLogo();
    void stopLogo();

    void preinit(XThread& thr);

    void show();

#if !__android
    void showLoginLabel(cwstr strUser);
#endif

    void quit(cfn_void cb);

    void switchFullScreen();

    void updateBkg();

    void handleTouchEvent(E_TouchEventType type, const CTouchEvent& te);

    bool drawBkg(bool bHLayout, CPainter&, cqrc) const;
    void drawDefaultBkg(CPainter&, cqrc, UINT szRound=0, float fCDCover=1.0f);

    void checkDemandable();

private:
    void _ctor();
    void _init();

    bool _checkDemandLanguage(E_LanguageType eLanguage, E_DemandMode eDemandMode=E_DemandMode::DM_Null)
    {
        auto itr = m_mapDemandLanguage.find(eLanguage);
        if (itr == m_mapDemandLanguage.end())
        {
            return false;
        }

        if (E_DemandMode::DM_Null == eDemandMode)
        {
            return true;
        }
        return itr->second.find(eDemandMode) != itr->second.end();
    }

    bool _checkSingerDemandable()
    {
        if (E_LanguageType::LT_None == m_eDemandLanguage)
        {
            return m_bSingerDemandable;
        }
        return _checkDemandLanguage(m_eDemandLanguage, E_DemandMode::DM_DemandSinger);
    }
    bool _checkAlbumDemandable()
    {
        if (E_LanguageType::LT_None == m_eDemandLanguage)
        {
            return m_bAlbumDemandable;
        }
        return _checkDemandLanguage(m_eDemandLanguage, E_DemandMode::DM_DemandAlbum);
    }

    bool _checkPlaylistDemandable()
    {
        if (E_LanguageType::LT_None == m_eDemandLanguage)
        {
            return m_bPlaylistDemandable;
        }
        return _checkDemandLanguage(m_eDemandLanguage, E_DemandMode::DM_DemandPlaylist);
    }

    void updateDemandButton();

    bool event(QEvent *) override;

    void _onPaint();

    void _relayout();

    void _showUpgradeProgress();

    void _updateLogoCompany(int nAlphaOffset, cfn_void cb=NULL);

    void _onPlay(const tagPlayingInfo& PlayingInfo, UINT uDuration, E_TrackType eTrackType);

    void _updatePlayPauseButton(bool bPlaying);

    void _updateProgress();

    void _demand(CButton* btnDemand);

    void _playSingerImg(UINT uSingerID, bool bReset);
    void _playSingerImg(UINT uSingerID);

private:
    void onPlayingListUpdated(int nPlayingItem, bool bSetActive) override;

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, CMedia *pMedia, CMediaRes *pMediaRes, bool bManual) override;

    void onPlayStop(bool bOpenSuccess, bool bPlayFinish) override;

    void onSingerImgDownloaded(CSinger& singer, const tagSingerImg&) override;

    bool installApp(const CByteBuffer& bbfBuff) override;
    bool installApp(const string& strFile) override;
};
