
#pragma once

#include <QMainWindow>

#include "widget/label.h"

#include "widget/button.h"

#include "PlayingList.h"

#include "dlg/mdl/medialibdlg.h"

#include "dlg/bkgdlg.h"

#define __crLogoText 100, 150, 255

struct tagPlayingInfo
{
    QString qsTitle;

    wstring strPath;

    QString qsDuration = 0;
    bool bWholeTrack = false;

#if __OnlineMediaLib
    UINT uFileSize = 0;
#endif

    E_MediaQuality eQuality = E_MediaQuality::MQ_None;
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
    QBrush m_brBkg;

    class CPlayingList m_PlayingList;

    CMedialibDlg m_medialibDlg;

    CBkgDlg m_bkgDlg;

    tagPlayingInfo m_PlayingInfo;

    bool m_bHLayout = false;

    bool m_bDefaultBkg = false;

    E_SingerImgPos m_eSingerImgPos = E_SingerImgPos::SIP_Float;

    int m_dxbkg = 0;
    int m_dybkg = 0;

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

    void showBlank();

    void showLogo();

    void preinit();

    void show();

    void switchFullScreen();

    void updateBkg();

    void handleTouchEvent(E_TouchEventType type, const CTouchEvent& te);

    void drawDefaultBkg(CPainter& painter, cqrc rc, UINT xround=0, UINT yround=0, bool bDrawDisk=true);

private:
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

    bool installApp(const CByteBuffer& bbfBuff) override
    {
        return installApp(bbfBuff);
    }
};
