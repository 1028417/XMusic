
#pragma once

#include <QMainWindow>

#include "widget/label.h"

#include "widget/button.h"

#include "PlayingList.h"

#include "dlg/mdl/medialibdlg.h"

#include "dlg/bkg/bkgdlg.h"

#define __crLogoText 100, 150, 255

#define __cxBkg 1080
#define __cyBkg 2340

struct tagPlayingInfo
{
    QString qsTitle;

    wstring strPath;

#if __OnlineMediaLib
    UINT uFileSize = 0;
#endif

    QString qsDuration = 0;
    E_TrackType eTrackType = E_TrackType::TT_Single;

    QString qsQuality;

    UINT uSingerID = 0;
    wstring strSingerName;

    IMedia *pRelatedMedia = NULL;
    CMediaSet *pRelatedMediaSet = NULL;
};

enum class E_SingerImgPos
{
    SIP_Float = 0,
    SIP_Dock,
    SIP_Zoomout
};

class MainWindow : public QMainWindow, public IModelObserver
{
    Q_OBJECT
public:
    MainWindow();

private:
    bool m_bHLayout = false;
    E_SingerImgPos m_eSingerImgPos = E_SingerImgPos::SIP_Float;

    QBrush m_brBkg;
    QPixmap m_pmCDCover;
    bool m_bDefaultBkg = false;

    pair<int, int>& m_prHBkgOffset;
    pair<int, int>& m_prVBkgOffset;

    E_LanguageType m_eDemandLanguage = E_LanguageType::LT_None;

    tagPlayingInfo m_PlayingInfo;

    class CPlayingList m_PlayingList;

    CMedialibDlg m_medialibDlg;

    CBkgDlg m_bkgDlg;

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

#if !__android
    void showBlank();
#endif

    void showLogo();

    void preinit();

    void show();

    void switchFullScreen();

    void updateBkg();

    void handleTouchEvent(E_TouchEventType type, const CTouchEvent& te);

    bool drawBkg(bool bHLayout, CPainter& painter, cqrc rc);
    void drawDefaultBkg(CPainter& painter, cqrc rc, UINT szRound=0, float fCDCover=1.0f);

private:
    void _ctor();
    void _init();

    bool event(QEvent *) override;

    void _onPaint();

    void _relayout();

    void _showUpgradeProgress();

    void _updateLogoCompany(int nAlphaOffset, cfn_void cb=NULL);

    void _updatePlayPauseButton(bool bPlaying);

    void _updateProgress();

    void _demand(CButton* btnDemand);

    void _handleTouchEnd(const CTouchEvent& te);

    void _playSingerImg(bool bReset);
    void _playSingerImg();

private:
    void onPlayingListUpdated(int nPlayingItem, bool bSetActive) override;

    void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;

    void onPlayStop(bool bOpenSuccess, bool bPlayFinish) override;

    void onSingerImgDownloaded(cwstr strSingerName, const tagSingerImg&) override;

    bool installApp(const CByteBuffer& bbfBuff) override;
};
