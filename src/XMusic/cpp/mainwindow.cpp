
#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QMovie>

#include "widget.cpp"

#include "bkgdlg.h"

#include "app.h"

#include <QScreen>

#define __size10 __size(10)

static Ui::MainWindow ui;

static bool g_bFullScreen = true;

inline static void _fixWorkArea(QWidget& wnd)
{
    (void)wnd;
#if __windows
    const RECT& rcWorkArea = getWorkArea(g_bFullScreen);
    wnd.setGeometry(rcWorkArea.left, rcWorkArea.top
                      , rcWorkArea.right-rcWorkArea.left, rcWorkArea.bottom-rcWorkArea.top);
#endif
}

void fixWorkArea(QWidget& wnd)
{
    if (__android || __ios || g_bFullScreen)
    {
        wnd.setWindowState(Qt::WindowFullScreen);
    }
    else
    {
        wnd.setWindowState(Qt::WindowMaximized);
    }

    _fixWorkArea(wnd);
}

MainWindow::MainWindow(CXMusicApp& app)
    : m_app(app)
    , m_PlayingList(app)
    , m_medialibDlg(app)
    , m_bkgDlg(app)
{
    ui.setupUi(this);

    m_PlayingList.setParent(ui.centralWidget);
    m_PlayingList.raise();

    ui.btnExit->raise();

    ui.labelBkg->setVisible(false);

    ui.btnPause->setVisible(false);

    QWidget* lpTopWidget[] = {
        ui.frameDemand, ui.btnMore

        , ui.btnDemandSinger, ui.btnDemandAlbum
        , ui.btnDemandAlbumItem, ui.btnDemandPlayItem, ui.btnDemandPlaylist

        , ui.frameDemandLanguage, ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR
        , ui.labelDemandJP, ui.labelDemandTAI, ui.labelDemandEN, ui.labelDemandEUR

        , ui.btnExit
    };
    for (auto pWidget : lpTopWidget)
    {
        m_mapTopWidgetPos[pWidget] = pWidget->geometry();
    }

    QWidget* lpWidget[] = {
        ui.labelPlayingfile, ui.wdgSingerImg, ui.labelSingerName, ui.labelAlbumName

        , ui.labelDuration, ui.barProgress, ui.labelProgress

        , ui.btnPlay, ui.btnPause, ui.btnPlayPrev, ui.btnPlayNext

        , ui.btnSetting, ui.btnOrder, ui.btnRandom
    };
    for (auto pWidget : lpWidget)
    {
        m_mapWidgetPos[pWidget] = pWidget->geometry();
    }

    ui.labelLogo->setParent(this);
    ui.labelLogoTip->setParent(this);
    ui.labelLogoCompany->setParent(this);


    connect(ui.btnFullScreen, &QPushButton::clicked, [&](){
        auto& bFullScreen = m_app.getOption().bFullScreen;
        bFullScreen = !bFullScreen;
        g_bFullScreen = bFullScreen;

        fixWorkArea(*this);
    });

    ui.btnFullScreen->setParent(this);
#if __android || __ios
    ui.btnFullScreen->setVisible(false);
#endif

    ui.labelLogo->setAttribute(Qt::WA_TranslucentBackground);
    ui.labelLogoTip->setAttribute(Qt::WA_TranslucentBackground);
    ui.labelLogoCompany->setAttribute(Qt::WA_TranslucentBackground);

    ui.frameDemand->setAttribute(Qt::WA_TranslucentBackground);
    ui.frameDemandLanguage->setAttribute(Qt::WA_TranslucentBackground);

    ui.centralWidget->setVisible(false);

    this->setWindowFlags(Qt::FramelessWindowHint);
}

void MainWindow::showLogo()
{
    float fFontSizeOffset = 1.0f;
#if __android || __ios
    fFontSizeOffset = 0.9;

    cauto szScreen = QApplication::primaryScreen()->size();
    int nScreenSize = MIN(szScreen.width(), szScreen.height());
    int nLogoWidth = nScreenSize*42/100;

    ui.labelLogo->setScaledContents(true);
    ui.labelLogo->resize(nLogoWidth, nLogoWidth/4);
#endif

    ui.labelLogoTip->setFont(CFont(fFontSizeOffset, E_FontWeight::FW_Light, true));

#if __android || __ios
    fFontSizeOffset = 1;
#endif
    ui.labelLogoCompany->setFont(CFont(fFontSizeOffset));

    QPalette peTip;
    peTip.setColor(QPalette::WindowText, QColor(64, 128, 255));
    ui.labelLogoTip->setPalette(peTip);

    QPalette peCompany;
    peCompany.setColor(QPalette::WindowText, QColor(64, 128, 255, 0));
    ui.labelLogoCompany->setPalette(peCompany);

    static QMovie movie(":/img/logo.gif");
    ui.labelLogo->setMovie(&movie);

    g_bFullScreen = m_app.getOption().bFullScreen;
    fixWorkArea(*this);
    this->setVisible(true);

    UINT uDelayTime = 100;
#if !__android
    uDelayTime += 800;
#endif
    timerutil::async(uDelayTime, [&](){
        ui.labelLogo->movie()->start();

        auto labelLogoTip = ui.labelLogoTip;
        timerutil::async(500, [labelLogoTip](){
            labelLogoTip->setText("播放器");

            timerutil::async(500, [labelLogoTip](){
                labelLogoTip->setText(labelLogoTip->text() + strutil::toQstr(__CNDot) + "媒体库");

                timerutil::async(500, [labelLogoTip](){
                    labelLogoTip->setText(labelLogoTip->text() + "  个性化定制");
                });
            });
        });

        timerutil::setTimerEx(40, [&](){
            auto peCompany = ui.labelLogoCompany->palette();
            auto crCompany = peCompany.color(QPalette::WindowText);

            auto alpha = crCompany.alpha() + 5;
            crCompany.setAlpha(alpha);
            peCompany.setColor(QPalette::WindowText, crCompany);
            ui.labelLogoCompany->setPalette(peCompany);

            if (alpha >= 255)
            {
                timerutil::setTimerEx(500, [&](){
                    if (!ui.labelLogoCompany->isVisible())
                    {
                        return false;
                    }

                    auto peCompany = ui.labelLogoCompany->palette();
                    auto crCompany = peCompany.color(QPalette::WindowText);
                    if (crCompany.alpha() < 255)
                    {
                        crCompany.setAlpha(255);
                    }
                    else
                    {
                        crCompany.setAlpha(170);
                    }
                    peCompany.setColor(QPalette::WindowText, crCompany);
                    ui.labelLogoCompany->setPalette(peCompany);

                    return true;
                });

                return false;
            }

            return true;
        });
    });
}

void MainWindow::_init()
{
    SList<CButton*> lstButtons {ui.btnDemandSinger, ui.btnDemandAlbum, ui.btnDemandAlbumItem
                , ui.btnDemandPlayItem, ui.btnDemandPlaylist, ui.btnMore
                , ui.btnExit, ui.btnSetting, ui.btnPause, ui.btnPlay
                , ui.btnPlayPrev, ui.btnPlayNext, ui.btnOrder, ui.btnRandom};
    for (auto button : lstButtons)
    {
        connect(button, &CButton::signal_clicked, this, &MainWindow::slot_buttonClicked);
    }

    SList<CLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR
                , ui.labelDemandJP, ui.labelDemandTAI, ui.labelDemandEN, ui.labelDemandEUR};
    for (auto label : lstLabels)
    {
        label->setFont(1, E_FontWeight::FW_SemiBold);
    }
    lstLabels.add(ui.labelSingerImg, ui.labelSingerName, ui.labelAlbumName, ui.labelPlayingfile
                , ui.labelProgress);
    for (auto label : lstLabels)
    {
        connect(label, &CLabel::signal_click, this, &MainWindow::slot_labelClick);
    }
    /*lstLabels.add(ui.labelDuration);
    for (auto label : lstLabels)
    {
        label->setTextColor(g_crText);
    }*/

    connect(this, &MainWindow::signal_updatePlayingList, this, &MainWindow::slot_updatePlayingList);
    connect(this, &MainWindow::signal_showPlaying, this, &MainWindow::slot_showPlaying);
    connect(this, &MainWindow::signal_playStoped, this, &MainWindow::slot_playStoped);

    //if (XMediaLib::m_bOnlineMediaLib)
    connect(this, &MainWindow::signal_updateSingerImg, this, [&](){
        if (m_medialibDlg.isVisible())
        {
            m_medialibDlg.updateSingerImg();
        }
    });

    ui.wdgSingerImg->setVisible(false);

    ui.labelSingerName->setFont(1);

    ui.labelPlayingfile->setFont(1);

    ui.labelDuration->setFont(0.8);

    m_PlayingList.setFont(0.9);

    if (m_app.getOption().bRandomPlay)
    {
        ui.btnRandom->setVisible(true);
        ui.btnOrder->setVisible(false);
    }
    else
    {
        ui.btnRandom->setVisible(false);
        ui.btnOrder->setVisible(true);
    }
}

void MainWindow::show()
{
    _init();

    m_medialibDlg.init();

    m_bkgDlg.init();

    ui.labelLogo->movie()->stop();
    ui.labelLogo->setVisible(false);
    ui.labelLogoTip->setVisible(false);
    ui.labelLogoCompany->setVisible(false);
    ui.btnFullScreen->setVisible(false);

    (void)startTimer(1000);

#if __android    
    ui.centralWidget->setAttribute(Qt::WA_TranslucentBackground);
#endif

    ui.centralWidget->setVisible(true);

    update();

    _relayout();

    m_PlayingList.updateList(m_app.getOption().uPlayingItem);
}

void MainWindow::_onPaint(CPainter& painter)
{
    cauto rc = this->rect();

    if (ui.labelLogo->isVisible())
    {
        painter.fillRect(rc, QColor(__defThemeColor));
        return;
    }

    if (m_app.getOption().bUseThemeColor)
    {
        painter.fillRect(rc, g_crTheme);
        return;
    }

    bool bHScreen = rc.width() > rc.height();
    cauto pmBkg = bHScreen?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
    if (!pmBkg.isNull())
    {
       painter.drawPixmapEx(rc, pmBkg);
    }
    else
    {
        drawDefaultBkg(painter, rc);
    }
}

bool MainWindow::event(QEvent *ev)
{
    switch (ev->type())
    {
    case QEvent::Move:
    case QEvent::Resize:
        _fixWorkArea(*this);
        _relayout();

        CDialog::resetPos();

        break;
    case QEvent::Paint:
    {
        CPainter painter(this);
        _onPaint(painter);
    }

    break;
    case QEvent::Close:
        m_app.quit();

        break;
    case QEvent::Timer:
    {
        _playSingerImg(false);

        auto ePlayStatus = m_app.getPlayMgr().GetPlayStatus();
        if (ePlayStatus != E_PlayStatus::PS_Play)
        {
            if (!XMediaLib::m_bOnlineMediaLib || ePlayStatus != E_PlayStatus::PS_Pause)
            {
                break;
            }
        }

//        UINT streamPos = m_app.getPlayMgr().mediaOpaque().streamPos()/1000;
//        UINT pos = streamPos * ui.barProgress->maximum() / ui.barProgress->maxBuffer();
//        ui.barProgress->setValue(pos);

        int nProgress = int(m_app.getPlayMgr().player().GetClock()/__1e6);
        if (nProgress <= ui.barProgress->maximum())
        {
            UINT bufferValue = 0;
            if (XMediaLib::m_bOnlineMediaLib)
            {
                bufferValue = UINT(m_app.getPlayMgr().mediaOpaque().streamSize()/1000);
            }
            ui.barProgress->setValue(nProgress, bufferValue);
        }
    }
    break;
    default:
        break;
    }

    return QMainWindow::event(ev);
}

void MainWindow::_relayout()
{
    int cx = this->width();
    int cy = this->height();
    m_bHScreen = cx > cy; // 橫屏

    int x_Logo = (cx - ui.labelLogo->width())/2;
    int y_Logo = (cy - ui.labelLogo->height())/2;
    if (m_bHScreen)
    {
        y_Logo -= __size(60);
    }
    else
    {
        y_Logo -= __size(100);
    }
    ui.labelLogo->move(x_Logo, y_Logo);

    int y_LogoTip = ui.labelLogo->geometry().bottom() + __size(30);
    ui.labelLogoTip->setGeometry(0, y_LogoTip, cx, ui.labelLogoTip->height());

#define __LogoCompanyMargin __size(50)
    int y_LogoCompany = cy - __LogoCompanyMargin - ui.labelLogoCompany->height();
    ui.labelLogoCompany->setGeometry(__LogoCompanyMargin, y_LogoCompany, cx-__LogoCompanyMargin*2, ui.labelLogoCompany->height());
    if (m_bHScreen)
    {
        ui.labelLogoCompany->setAlignment(Qt::AlignmentFlag::AlignBottom | Qt::AlignmentFlag::AlignRight);
    }
    else
    {
        ui.labelLogoCompany->setAlignment(Qt::AlignmentFlag::AlignBottom | Qt::AlignmentFlag::AlignHCenter);
    }

    float fCXRate = 0;
    float fCXRateEx = 0;
    if (m_bHScreen)
    {
        fCXRate = (float)cx/ui.labelBkg->pixmap()->width();
    }
    else
    {
        fCXRate = (float)cx/1080;
    }
    fCXRateEx = fCXRate * g_fPixelRatio;

    int cy_bkg = fCXRate * ui.labelBkg->pixmap()->height();
    int dy_bkg = cy - cy_bkg;

    UINT uShadowAlpha = 0;
    if (m_app.getOption().bUseThemeColor)
    {
        m_bUseDefaultBkg = false;

        uShadowAlpha = 85;
    }
    else
    {
        const QPixmap &pmBkg = m_bHScreen?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
        m_bUseDefaultBkg = pmBkg.isNull();

        if (m_bUseDefaultBkg)
        {
            m_bUseDefaultBkg = 1;
        }
        else
        {
            uShadowAlpha = 170;
        }
    }

    ui.labelDemandHK->setShadow(uShadowAlpha);
    ui.labelDemandCN->setShadow(uShadowAlpha);
    ui.labelDemandKR->setShadow(uShadowAlpha);
    ui.labelDemandJP->setShadow(uShadowAlpha);
    ui.labelDemandTAI->setShadow(uShadowAlpha);
    ui.labelDemandEN->setShadow(uShadowAlpha);
    ui.labelDemandEUR->setShadow(uShadowAlpha);

    ui.labelSingerName->setShadow(100, 2);

    ui.labelAlbumName->setShadow(uShadowAlpha);

    ui.labelDuration->setShadow(uShadowAlpha);

    for (cauto widgetPos : m_mapTopWidgetPos)
    {
        QRect pos = widgetPos.second;
        if (fCXRate < 1)
        {
            pos.setRect(fCXRate*pos.left(), fCXRate*pos.top()
                        , fCXRate*pos.width(), fCXRate*pos.height());
        }
        widgetPos.first->setGeometry(pos);
    }

    for (cauto widgetPos : m_mapWidgetPos)
    {
        cauto pos = widgetPos.second;
        auto& newPos = m_mapWidgetNewPos[widgetPos.first];

        int width = pos.width();
        int height = pos.height();
        if (fCXRate <= 1 || NULL == dynamic_cast<QPushButton*>(widgetPos.first))
        {
            width = int(width * fCXRate);
            height = int(height * fCXRate);
        }

        newPos.setRect(fCXRate*pos.center().x() - width/2
                       , fCXRate*pos.center().y() - height/2 + dy_bkg, width, height);
        widgetPos.first->setGeometry(newPos);
    }

    int y_frameDemand = __size(20);
    if (cy > __size(1920))
    {
        y_frameDemand = __size(36);
    }
    else if (__size(1920) == cy)
    {
        y_frameDemand = __size(28);
    }
    else if (cy < __size(1000))
    {
        if (m_bUseDefaultBkg)
        {
            y_frameDemand = __size(12);
        }
    }

    int x_frameDemand = 0;
    if (m_bHScreen)
    {
        x_frameDemand = (ui.barProgress->geometry().right()
            + ui.barProgress->x() - ui.frameDemand->width())/2;
    }
    else
    {
        x_frameDemand = (cx - ui.frameDemand->width())/2;
        x_frameDemand -= __size10;
    }
    ui.frameDemand->move(x_frameDemand, y_frameDemand);

    int y_btnMore = ui.frameDemand->y() + ui.btnDemandSinger->geometry().center().y() - ui.btnMore->height()/2;
    int x_btnMore = __size(25);

#if __android || __ios
     if (!m_bHScreen)
     {
         x_btnMore = cx - __size(25) - ui.btnMore->width();
     }

     ui.btnExit->move(-200,-200);
#else
    int x_btnExit = cx - ui.btnExit->width() - (y_frameDemand + __size10);
    ui.btnExit->move(x_btnExit, y_btnMore);
#endif

    ui.btnMore->move(x_btnMore, y_btnMore);

    if (!m_bUseDefaultBkg)
    {
        int yOffset = 0;

        if (fCXRate <= 1)
        {
#define __offset __size(10.0f)
            yOffset = (int)round(__offset/fCXRate);

            for (auto pWidget : SList<QWidget*>({ui.labelDuration, ui.barProgress, ui.labelProgress}))
            {
                pWidget->move(pWidget->x(), pWidget->y() - yOffset*2);
            }
        }

#define __dy __size(4)
        int dy = (int)round(fCXRate*__dy);
        for (auto pWidget : SList<QWidget*>(ui.btnPlay, ui.btnPause, ui.btnPlayPrev, ui.btnPlayNext
                                            , ui.btnSetting, ui.btnOrder, ui.btnRandom))
        {
            pWidget->move(pWidget->x(), pWidget->y() + dy - yOffset);
        }
    }

    int y_PlayingListMax = 0;

    ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    ui.labelAlbumName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignBottom);

    bool bZoomoutSingerImg = m_bZoomoutSingerImg && ui.wdgSingerImg->isVisible();

    auto& rcSingerImg = m_mapWidgetNewPos[ui.wdgSingerImg];

    m_PlayingList.setShadow(uShadowAlpha);

    ui.labelPlayingfile->setShadow(uShadowAlpha);

    if (!m_bUseDefaultBkg)
    {
        int x = ui.barProgress->x();

        int cy_Playingfile = ui.labelPlayingfile->height();
        int y_Playingfile = ui.labelDuration->geometry().bottom() -  cy_Playingfile;
        ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x() - x, cy_Playingfile);

        int cy_labelAlbumName = __size(80);
        int y_labelAlbumName = y_Playingfile - cy_labelAlbumName;
        int cx_barProgress = ui.barProgress->width();
        ui.labelAlbumName->setGeometry(x, y_labelAlbumName, cx_barProgress, cy_labelAlbumName);

        int y_SingerImg = 0;
        if (bZoomoutSingerImg)
        {
            ui.labelAlbumName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

            ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

            y_SingerImg = y_labelAlbumName-__size(300);
        }
        else
        {
            if (m_bHScreen)
            {
                y_SingerImg = ui.frameDemandLanguage->geometry().bottom() + __size(60);
            }
            else
            {
                y_SingerImg = cy/2+__size(150);
            }
        }

        int cy_SingerImg = y_labelAlbumName-y_SingerImg;
        int cx_SingerImg = cy_SingerImg * rcSingerImg.width() / rcSingerImg.height();

        int x_SingerImg = x;
        if (!bZoomoutSingerImg)
        {
            x_SingerImg += (cx_barProgress-cx_SingerImg)/2;
        }

        rcSingerImg.setRect(x_SingerImg, y_SingerImg, cx_SingerImg, cy_SingerImg);
        ui.wdgSingerImg->setGeometry(rcSingerImg);

        int y_labelSingerName = y_labelAlbumName-ui.labelSingerName->height();
        ui.labelSingerName->setGeometry(x_SingerImg+__size(15), y_labelSingerName, cx_SingerImg-__size(15), ui.labelSingerName->height());

        if (ui.wdgSingerImg->isVisible())
        {
            y_PlayingListMax = y_SingerImg;
        }
        else
        {
            if (!ui.labelSingerName->text().isEmpty())
            {
                y_PlayingListMax = y_labelSingerName;
            }
            else
            {
                y_PlayingListMax = y_labelAlbumName;
            }
        }

        //m_PlayingList.setTextColor(g_crText);
        m_PlayingList.setInactiveAlpha(0.4);
    }
    else
    {
        m_PlayingList.setInactiveAlpha(0.3);
        /*auto crText = g_crText;
        crText.setAlpha(160);
        m_PlayingList.setTextColor(crText);*/

        bool bFlag = false;//fCXRateEx > 1;
        if (m_bHScreen)
        {
             if (cy < __size(1080)*fCXRateEx)
             {
                 bFlag = true;
             }
        }
        else
        {
            bFlag = fCXRateEx > 1; // bFlag = cy < __size(1920);
        }
        if (bFlag)
        {
            int y_Playingfile = rcSingerImg.bottom()- ui.labelPlayingfile->height() - __size(20);
            ui.labelPlayingfile->move(rcSingerImg.left() + __size(30), y_Playingfile);

            y_PlayingListMax = rcSingerImg.top();
        }
        else
        {
            ui.labelPlayingfile->setShadow(0);

            ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignBottom);

            y_PlayingListMax = ui.labelPlayingfile->y();
        }
    }

#if __android || __ios
#define __SingerImgMargin 2
#else
#define __SingerImgMargin 1
#endif
    ui.labelSingerImg->setGeometry(__SingerImgMargin, __SingerImgMargin, rcSingerImg.width() - __SingerImgMargin*2
                                                   , rcSingerImg.height() - __SingerImgMargin*2);

    auto pPixmap = ui.labelSingerImg->pixmap();
    if (pPixmap && !pPixmap->isNull())
    {
        ui.labelSingerImg->setPixmap(*pPixmap);
    }

    _showAlbumName();

#define __CyPlayItem __size(115)
    UINT uRowCount = 0;
    if (m_bHScreen)
    {
        int x_PlayingList = ui.barProgress->geometry().right();
        if (cx >= __size(1920))
        {
            x_PlayingList += __size(100) * fCXRate;
        }
        else
        {
            x_PlayingList += __size(80);
        }

        UINT uMargin = __size(45);
        int cy_PlayingList = cx - x_PlayingList - uMargin * fCXRate;
        m_PlayingList.setGeometry(x_PlayingList, uMargin-1, cy_PlayingList, cy-uMargin*2);

        uRowCount = cy/__CyPlayItem;
        uRowCount = MAX(uRowCount,7);
        uRowCount = MIN(uRowCount,10);
    }
    else
    {
        UINT y_Margin = __size(30);

        int y_frameDemandBottom = ui.frameDemand->geometry().bottom();
        uRowCount = (y_PlayingListMax - y_frameDemandBottom - y_Margin*2)/__CyPlayItem;
        if (uRowCount > 10)
        {
            uRowCount = 10;
            y_Margin += __size10;

            if (bZoomoutSingerImg)
            {
                y_Margin += __size10;
            }
        }
        else if (uRowCount < 7)
        {
            uRowCount = 7;
            y_Margin -= __size10;
        }

        UINT x_Margin = ui.frameDemand->x();
        int y_PlayingList = y_frameDemandBottom + y_Margin;
        int cy_PlayingList = y_PlayingListMax - y_Margin - y_PlayingList;
        m_PlayingList.setGeometry(x_Margin, y_PlayingList, cx-x_Margin*2, cy_PlayingList);
    }
    m_PlayingList.setPageRowCount(uRowCount);
}

void MainWindow::_updatePlayPauseButton(bool bPlaying)
{
    ui.btnPlay->setVisible(!bPlaying);
    ui.btnPause->setVisible(bPlaying);
}

void MainWindow::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual)
{
    m_mtxPlayingInfo.lock([&](tagPlayingInfo& PlayingInfo) {
        PlayingInfo.uPlaySeq++;

        PlayingInfo.strTitle = PlayItem.GetTitle();

        PlayingInfo.nDuration = PlayItem.duration();

        PlayingInfo.uStreamSize = 0;
        if (XMediaLib::m_bOnlineMediaLib)
        {
            int nStreamSize = PlayItem.fileSize()/1000;
            if (nStreamSize > 0)
            {
                PlayingInfo.uStreamSize = (UINT)nStreamSize;
            }
        }

        PlayingInfo.strSinger = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Singer);
        PlayingInfo.uSingerID = PlayItem.GetRelatedMediaSetID(E_MediaSetType::MST_Singer);
        PlayingInfo.strAlbum = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Album);
        PlayingInfo.uRelatedAlbumItemID = PlayItem.GetRelatedMediaID(E_MediaSetType::MST_Album);

        PlayingInfo.strPlaylist = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Playlist);
        PlayingInfo.uRelatedPlayItemID = PlayItem.GetRelatedMediaID(E_MediaSetType::MST_Playlist);

        PlayingInfo.strPath = PlayItem.GetPath();
    });

    emit signal_showPlaying(uPlayingItem, bManual);
}

void MainWindow::slot_showPlaying(unsigned int uPlayingItem, bool bManual)
{
    m_PlayingList.updatePlayingItem(uPlayingItem, bManual);

    m_mtxPlayingInfo.get(m_PlayingInfo);
    _showAlbumName();

    ui.labelPlayingfile->setText(strutil::toQstr(m_PlayingInfo.strTitle));

    if (m_PlayingInfo.strSinger != m_strSingerName)
    {
        m_strSingerName = m_PlayingInfo.strSinger;

        ui.labelSingerName->setText(strutil::toQstr(m_strSingerName));

        ui.labelSingerImg->setPixmap(QPixmap());

        ui.wdgSingerImg->setVisible(false);
        _playSingerImg(true);

        _relayout();
    }

    _updatePlayPauseButton(true);

    ui.barProgress->setValue(0);
    ui.barProgress->setMaximum(m_PlayingInfo.nDuration, m_PlayingInfo.uStreamSize);

    QString qsDuration = strutil::toQstr(CMedia::GetDurationString(m_PlayingInfo.nDuration));
    ui.labelDuration->setText(qsDuration);
}

void MainWindow::onPlayStoped(E_DecodeStatus decodeStatus)
{
    if (decodeStatus != E_DecodeStatus::DS_Cancel)
    {
        bool bOpenFail = E_DecodeStatus::DS_OpenFail == decodeStatus;
        if (bOpenFail)
        {
            mtutil::usleep(1000);
        }

        emit signal_playStoped(bOpenFail);

        m_app.getCtrl().callPlayCtrl(E_PlayCtrl::PC_AutoPlayNext);
    }
}

void MainWindow::slot_playStoped(bool bOpenFail)
{
    ui.barProgress->setValue(0);

    if (bOpenFail)
    {
        _updatePlayPauseButton(false);
    }
    else
    {
        auto uPlaySeq = m_PlayingInfo.uPlaySeq;
        __async(1000, [&, uPlaySeq]() {
            if (uPlaySeq == m_PlayingInfo.uPlaySeq)
            {
                _updatePlayPauseButton(false);
            }
        });
    }
}

void MainWindow::_showAlbumName()
{
    auto eDemandMode = m_app.getPlayMgr().demandMode();

    WString strMediaSet;
    if (E_DemandMode::DM_DemandPlayItem == eDemandMode || m_PlayingInfo.strAlbum.empty())
    {
        if (!m_PlayingInfo.strPlaylist.empty())
        {
            if (!m_bUseDefaultBkg)
            {
                strMediaSet << L"歌单: ";
            }
            strMediaSet << m_PlayingInfo.strPlaylist;

            m_PlayingInfo.uRelatedAlbumItemID = 0;
        }
    }
    else if (!m_PlayingInfo.strAlbum.empty())
    {
        if (!m_bUseDefaultBkg)
        {
            strMediaSet << L"专辑: ";
        }
        strMediaSet << m_PlayingInfo.strAlbum;

        m_PlayingInfo.uRelatedPlayItemID = 0;
    }

    CLabel& labelAlbumName = *ui.labelAlbumName;
    if (strMediaSet->empty())
    {
        labelAlbumName.setVisible(false);
        return;
    }

    labelAlbumName.setVisible(true);
    labelAlbumName.setText(strMediaSet);

    labelAlbumName.setFont(1);

    if (m_bUseDefaultBkg)
    {
        cauto rcAlbumNamePrev = m_mapWidgetNewPos[&labelAlbumName];
        labelAlbumName.adjustSize();
        if (labelAlbumName.width() > rcAlbumNamePrev.width())
        {
            labelAlbumName.adjustFont(0.95f);
            //labelAlbumName.move(rcAlbumNamePrev.left(), labelAlbumName.y());
        }
        //else
        {
            int x_labelAlbumName = rcAlbumNamePrev.left() + (rcAlbumNamePrev.width()-labelAlbumName.width())/2;
            labelAlbumName.move(x_labelAlbumName, labelAlbumName.y());
        }
    }
}

void MainWindow::_playSingerImg(bool bReset)
{
    if (m_strSingerName.empty())
    {
        return;
    }

    static UINT uTickCount = 0;
    static UINT uSingerImgIdx = 0;
    if (bReset)
    {
        uTickCount = 0;
        uSingerImgIdx = 0;
    }
    else
    {
        if (uTickCount >= 8)
        {
            uTickCount = 0;
        }
        else
        {
            uTickCount++;
            return;
        }
    }

    cauto strSingerImg = m_app.getSingerImgMgr().getSingerImg(m_strSingerName, uSingerImgIdx);
    if (!strSingerImg.empty())
    {
        QPixmap pm;
        if (pm.load(strutil::toQstr(strSingerImg)))
        {
            ui.labelSingerImg->setPixmap(pm);

            if (!ui.wdgSingerImg->isVisible())
            {
                ui.wdgSingerImg->setVisible(true);
                _relayout();
            }
        }

        uSingerImgIdx++;
    }
    else
    {
        if (uSingerImgIdx > 1)
        {
            _playSingerImg(true);
        }
    }
}

void MainWindow::slot_buttonClicked(CButton* button)
{
    if (button == ui.btnExit)
    {
        this->close();
    }
    else if (button == ui.btnPause)
    {
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Pause));

        timerutil::async(100, [&](){
            if (E_PlayStatus::PS_Pause == m_app.getPlayMgr().GetPlayStatus())
            {
                _updatePlayPauseButton(false);
            }
        });
    }
    else if (button == ui.btnPlay)
    {
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Play));

        timerutil::async(100, [&](){
            if (E_PlayStatus::PS_Play == m_app.getPlayMgr().GetPlayStatus())
            {
                _updatePlayPauseButton(true);
            }
        });
    }
    else if (button == ui.btnPlayPrev)
    {
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_PlayPrev));
    }
    else if (button == ui.btnPlayNext)
    {
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_PlayNext));
    }
    else if (button == ui.btnRandom || button == ui.btnOrder)
    {
        auto& bRandomPlay = m_app.getOption().bRandomPlay;
        bRandomPlay = !bRandomPlay;

        ui.btnRandom->setVisible(bRandomPlay);
        ui.btnOrder->setVisible(!bRandomPlay);
    }
    else if (button == ui.btnSetting)
    {
        m_bkgDlg.show();
    }
    else if (button == ui.btnMore)
    {
        m_medialibDlg.show();
    }
    else
    {
        _demand(button);
    }
}

void MainWindow::updateBkg()
{
    _relayout();

    update();
}

static const QString __qsCheck = strutil::toQstr(L"√");

void MainWindow::slot_labelClick(CLabel* label, const QPoint& pos)
{
    if (label == ui.labelSingerImg)
    {
        if (!m_bUseDefaultBkg)
        {
            m_bZoomoutSingerImg = !m_bZoomoutSingerImg;
            _relayout();
        }
    }
    else if (label == ui.labelSingerName)
    {
        if (m_PlayingInfo.uSingerID != 0)
        {
            CMediaSet *pMediaSet = m_app.getSingerMgr().FindSubSet(E_MediaSetType::MST_Singer, m_PlayingInfo.uSingerID);
            if (pMediaSet)
            {
                m_medialibDlg.showMediaSet(*pMediaSet);
            }
        }
    }
    else if (label == ui.labelAlbumName)
    {
        CMedia *pMedia = NULL;
        if (m_PlayingInfo.uRelatedAlbumItemID != 0)
        {
            pMedia = m_app.getSingerMgr().FindMedia(
                                    E_MediaSetType::MST_Album, m_PlayingInfo.uRelatedAlbumItemID);
        }
        else if (m_PlayingInfo.uRelatedPlayItemID != 0)
        {
            pMedia = m_app.getPlaylistMgr().FindMedia(
                                    E_MediaSetType::MST_Playlist, m_PlayingInfo.uRelatedPlayItemID);
        }
        if (pMedia && pMedia->m_pParent)
        {
            m_medialibDlg.showMedia(*pMedia);
        }
    }
    else if (label == ui.labelPlayingfile)
    {
        if (!m_medialibDlg.showFile(m_PlayingInfo.strPath))
        {
            slot_labelClick(ui.labelAlbumName, pos);
        }
    }
    else if (label == ui.labelProgress)
    {
        if (E_PlayStatus::PS_Play == m_app.getPlayMgr().GetPlayStatus())
        {
            auto barProgress = ui.barProgress;
            UINT uSeekPos = pos.x() * barProgress->maximum() / barProgress->width();
            if (barProgress->maxBuffer() > 0)
            {
                /*if (uSeekPos > barProgress->value())
                {
                    UINT uCutPos = pos.x() * barProgress->maxBuffer() / barProgress->width();
                    if (uCutPos < barProgress->bufferValue())
                    {
                        m_app.getPlayMgr().mediaOpaque().cutData((uint64_t)uCutPos * 1000);

                        m_app.getPlayMgr().player().Seek(uSeekPos);
                        barProgress->setValue(uSeekPos);
                    }
                }*/
            }
            else
            {
                m_app.getPlayMgr().player().Seek(uSeekPos);
                barProgress->setValue(uSeekPos);
            }
        }
    }
    else
    {
        m_eDemandLanguage = E_LanguageType::LT_None;

        PairList<E_LanguageType, CLabel*> plLabels {
            {E_LanguageType::LT_CN, ui.labelDemandCN}
            , {E_LanguageType::LT_HK, ui.labelDemandHK}
            , {E_LanguageType::LT_KR, ui.labelDemandKR}
            , {E_LanguageType::LT_JP, ui.labelDemandJP}
            , {E_LanguageType::LT_TAI, ui.labelDemandTAI}
            , {E_LanguageType::LT_EN, ui.labelDemandEN}
            , {E_LanguageType::LT_EUR, ui.labelDemandEUR}};
        plLabels([&](E_LanguageType eLanguage, CLabel* lblLanguage) {
            if (lblLanguage->text().startsWith(__qsCheck))
            {
                lblLanguage->setText(lblLanguage->text().mid(__qsCheck.length()));

                //lblLanguage->setTextColor(g_crText);
            }
            else
            {
                if (lblLanguage == label)
                {
                    m_eDemandLanguage = eLanguage;

                    lblLanguage->setText(__qsCheck + lblLanguage->text());

                    //lblLanguage->setTextColor(210,255,220);
                }
            }
        });
    }
}

void MainWindow::_demand(CButton* btnDemand)
{
    E_DemandMode eDemandMode = E_DemandMode::DM_Null;
    if (btnDemand == ui.btnDemandSinger)
    {
        eDemandMode = E_DemandMode::DM_DemandSinger;
    }
    else if (btnDemand == ui.btnDemandAlbum)
    {
        eDemandMode = E_DemandMode::DM_DemandAlbum;
    }
    else if (btnDemand == ui.btnDemandAlbumItem)
    {
        eDemandMode = E_DemandMode::DM_DemandAlbumItem;
    }
    else if (btnDemand == ui.btnDemandPlayItem)
    {
        eDemandMode = E_DemandMode::DM_DemandPlayItem;
    }
    else if (btnDemand == ui.btnDemandPlaylist)
    {
        eDemandMode = E_DemandMode::DM_DemandPlaylist;
    }
    else
    {
        return;
    }

    m_app.getCtrl().callPlayCtrl(tagPlayCtrl(eDemandMode, m_eDemandLanguage));
}

void MainWindow::drawDefaultBkg(QPainter& painter, const QRect& rc)
{
    QRect rcSrc = ui.labelBkg->pixmap()->rect();

    float fHWRate = (float)rc.height()/rc.width();
    if (fHWRate < 1)
    {
        rcSrc.setTop(rcSrc.bottom()-rcSrc.width()*fHWRate);
    }
    else
    {
        rcSrc.setRight(1080);
        rcSrc.setTop(rcSrc.bottom()-rcSrc.right()*fHWRate);
    }

    painter.drawPixmap(rc, *ui.labelBkg->pixmap(), rcSrc);
}
