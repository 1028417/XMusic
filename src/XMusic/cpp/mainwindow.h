
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

    CMediaSet *pMediaSet = NULL;
};

class CMainWnd : public QMainWindow
{
protected:
    CMainWnd();
};

class MainWindow : public CMainWnd, public IModelObserver
{
    Q_OBJECT
public:
    MainWindow(class CApp& app);

private:
    class CApp& m_app;

    QBrush m_brBkg;
    int m_cxBkg = 0;
    int m_cyBkg = 0;
    float m_fBkgHWRate = .0f;
    float m_fBkgTopReserve = .0f;

    class CPlayingList m_PlayingList;

    CMedialibDlg m_medialibDlg;

    CBkgDlg m_bkgDlg;

    map<QWidget*, QRect> m_mapTopWidgetPos;

    map<QWidget*, QRect> m_mapWidgetPos;
    map<QWidget*, QRect> m_mapWidgetNewPos;

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

    void drawDefaultBkg(CPainter& painter, cqrc rc, UINT xround=0, UINT yround=0, bool bDrawDisk=true);

private:
    void _init();

    bool event(QEvent *) override;

    void _onPaint();

    void _relayout();

    float _caleBkgZoomRate(int& cxDst, int cyDst, int& xDst);

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
