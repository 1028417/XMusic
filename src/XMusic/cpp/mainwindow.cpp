
#include "widget.cpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMovie>

#include <QScreen>

#include "app.h"

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

MainWindow::MainWindow(CApp& app)
    : m_app(app)
    , m_PlayingList(app)
    , m_medialibDlg(*this, app)
    , m_bkgDlg(*this, app)
{
    ui.setupUi(this);

    m_PlayingList.setParent(ui.centralWidget);
    m_PlayingList.raise();

    ui.labelBkg->setVisible(false);

    ui.btnPause->setVisible(false);

    for (auto pWidget : SList<QWidget*>(
             ui.btnExit, ui.frameDemand, ui.btnMore, ui.btnDemandSinger, ui.btnDemandAlbum
             , ui.btnDemandAlbumItem, ui.btnDemandPlayItem, ui.btnDemandPlaylist
             , ui.frameDemandLanguage, ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR
             , ui.labelDemandJP, ui.labelDemandTAI, ui.labelDemandEN, ui.labelDemandEUR))
    {
        m_mapTopWidgetPos[pWidget] = pWidget->geometry();
    }

    for (auto pWidget : SList<QWidget*>(
             ui.labelPlayingfile, ui.labelSingerImg, ui.labelSingerName, ui.labelAlbumName
             , ui.labelDuration, ui.barProgress, ui.labelProgress, ui.btnPlay, ui.btnPause
             , ui.btnPlayPrev, ui.btnPlayNext, ui.btnSetting, ui.btnOrder, ui.btnRandom))
    {
        m_mapWidgetPos[pWidget] = pWidget->geometry();
    }

    ui.labelLogo->setParent(this);
    ui.labelLogoTip->setParent(this);
    ui.labelLogoCompany->setParent(this);

    ui.btnExit->setParent(this); // ui.btnExit->raise();

    for (auto button : SList<CButton*>(ui.btnDemandSinger, ui.btnDemandAlbum, ui.btnDemandAlbumItem
                , ui.btnDemandPlayItem, ui.btnDemandPlaylist, ui.btnMore
                , ui.btnExit, ui.btnSetting, ui.btnPause, ui.btnPlay
                , ui.btnPlayPrev, ui.btnPlayNext, ui.btnOrder, ui.btnRandom))
    {
        connect(button, &CButton::signal_clicked, this, &MainWindow::slot_buttonClicked);
    }

    connect(ui.btnFullScreen, &QPushButton::clicked, [&](){
        g_bFullScreen = !g_bFullScreen;
        m_app.getOption().bFullScreen = g_bFullScreen;

        fixWorkArea(*this);
    });

    ui.btnFullScreen->setParent(this);
#if __android || __ios
    ui.btnFullScreen->setVisible(false);
#endif

    ui.centralWidget->setVisible(false);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    qRegisterMetaType<QVariant>("QVariant");
}

void MainWindow::showLogo()
{
    for (auto widget : SList<QWidget*>(ui.labelLogo, ui.labelLogoTip, ui.labelLogoCompany
                                       , ui.centralWidget, ui.frameDemand, ui.frameDemandLanguage))
    {
        widget->setAttribute(Qt::WA_TranslucentBackground);
    }

    float fFontSizeOffset = 1.0f;
#if __android || __ios
    fFontSizeOffset = 0.94;

    cauto szScreen = QApplication::primaryScreen()->size();
    int nScreenSize = MIN(szScreen.width(), szScreen.height());
    int nLogoWidth = nScreenSize*42/100;

    ui.labelLogo->setScaledContents(true);
    ui.labelLogo->resize(nLogoWidth, nLogoWidth/4);
#endif

    ui.labelLogoTip->setFont(CFont(fFontSizeOffset, g_nDefFontWeight, true));
    ui.labelLogoCompany->setFont(CFont(fFontSizeOffset));

    CDialog::setWidgetTextColor(ui.labelLogoTip, QColor(64, 128, 255));
    CDialog::setWidgetTextColor(ui.labelLogoCompany, QColor(64, 128, 255, 0));

    static QMovie movie(":/img/logo.gif");
    ui.labelLogo->setMovie(&movie);

    g_bFullScreen = m_app.getOption().bFullScreen;
    fixWorkArea(*this);
    this->setVisible(true);

    UINT uDelayTime = 100;
#if !__android
    uDelayTime += 500;
#endif
    __appAsync(uDelayTime, [&](){
        /*#if __windows
            m_app.setForeground();
        #endif*/

        ui.labelLogo->movie()->start();

        __appAsync(100, [&](){
            _updateLogoTip();
        });

        _updateLogoCompany(5, [&](){
            _updateLogoCompany(-5, [&](){
                ui.labelLogoCompany->setText("v"+strutil::toQstr(m_app.appVersion()));
                _updateLogoCompany(5);
            });
        });
    });
}

void MainWindow::_updateLogoTip()
{
    auto labelLogoTip = ui.labelLogoTip;
    labelLogoTip->setText("播放器");

    __appAsync(600, [=](){
        labelLogoTip->setText(labelLogoTip->text() + strutil::toQstr(__CNDot) + "媒体库");

        __appAsync(600, [=](){
            labelLogoTip->setText(labelLogoTip->text() + "  个性化定制");

            __appAsync(2700, [=](){
#define __logoTip "更新媒体库"
                if (-1 == g_nAppDownloadProgress)
                {
                    labelLogoTip->setText(__logoTip);
                }

                timerutil::setTimerEx(300, [=](){
                    if (!labelLogoTip->isVisible())
                    {
                        return false;
                    }

                    _showUpgradeProgress();

                    return true;
                });
            });
        });
    });
}

void MainWindow::_showUpgradeProgress()
{
    if (g_nAppDownloadProgress >= 0)
    {
        QString qsText;
        if (0 == g_nAppDownloadProgress)
        {
            qsText.append("下载更新包···");
        }
        else if (100 == g_nAppDownloadProgress)
        {
            qsText.append("准备安装···");
        }
        else
        {
            qsText.sprintf("下载更新包:  %u%%", (UINT)g_nAppDownloadProgress);
        }
        ui.labelLogoTip->setText(qsText);
    }
    else
    {
        static UINT uDotCount = 0;
        uDotCount++;
        if (uDotCount > 3)
        {
            uDotCount = 0;
        }

        QString qsText(__logoTip);
        for (byte_t uIdx = 1; uIdx <= 3; uIdx++)
        {
            if (uDotCount >= uIdx)
            {
                qsText.append('.');
            }
            else
            {
                qsText.append(' ');
            }
        }
        ui.labelLogoTip->setText(qsText);
    }
}

void MainWindow::_updateLogoCompany(int nAlphaOffset, cfn_void cb)
{
    auto labelLogoCompany = ui.labelLogoCompany;
    auto peCompany = labelLogoCompany->palette();
    auto crCompany = peCompany.color(QPalette::WindowText);
    auto nAlpha = crCompany.alpha();

    timerutil::setTimerEx(40, [=]()mutable{
        nAlpha += nAlphaOffset;
        if (nAlpha < 0 || nAlpha > 255)
        {
            if (cb)
            {
                cb();
            }
            return false;
        }

        crCompany.setAlpha(nAlpha);
        peCompany.setColor(QPalette::WindowText, crCompany);
        labelLogoCompany->setPalette(peCompany);

        return true;
    });
}

void MainWindow::_init()
{
    SList<CLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR
                , ui.labelDemandJP, ui.labelDemandTAI, ui.labelDemandEN, ui.labelDemandEUR};
    for (auto label : lstLabels)
    {
        label->setFont(1, QFont::Weight::DemiBold);
    }
    lstLabels.add(ui.labelSingerImg, ui.labelSingerName, ui.labelAlbumName, ui.labelPlayingfile
                , ui.labelProgress);
    for (auto label : lstLabels)
    {
        connect(label, &CLabel::signal_click, this, &MainWindow::slot_labelClick);
    }

    connect(this, &MainWindow::signal_updatePlayingList, this, &MainWindow::slot_updatePlayingList);
    connect(this, &MainWindow::signal_showPlaying, this, &MainWindow::slot_showPlaying);
    connect(this, &MainWindow::signal_playStoped, this, &MainWindow::slot_playStoped);

//#if __OnlineMediaLib
    connect(this, &MainWindow::signal_updateSingerImg, this, [&](){
        if (m_medialibDlg.isVisible())
        {
            m_medialibDlg.updateSingerImg();
        }
    });

    ui.labelSingerImg->setVisible(false);
    ui.labelSingerImg->setShadow(2);

    ui.labelSingerName->setFont(0.95);

    ui.labelPlayingfile->setFont(0.95);

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

void MainWindow::initBkg()
{
    m_bkgDlg.initBkg(true);
    m_bkgDlg.initBkg(false);
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

    ui.centralWidget->setVisible(true);
    _relayout();

    m_PlayingList.updateList(m_app.getOption().uPlayingItem);

    update();

    (void)startTimer(1000);

#if __windows
    __appAsync(100, [&](){
        m_app.setForeground();
    });
#endif
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
#if __android || __ios
    case QEvent::KeyRelease:
    {
        if (!ui.labelLogo->isVisible())
        {
            static time_t prevTime = 0;

            time_t currTime = time(0);
            if (currTime - prevTime > 3)
            {
#if __android
                m_app.vibrate();
#endif
                prevTime = currTime;
                return true;
            }

            prevTime = currTime;
        }

        this->setVisible(false);
        this->repaint();
    }

    break;
#endif
    case QEvent::Close:
        m_app.quit();

        break;
    case QEvent::Timer:
        _playSingerImg(false);

        _updateProgress();

    break;
    default:
        break;
    }

    return QMainWindow::event(ev);
}

void MainWindow::_relayout()
{
    cauto rc = rect();
    ui.centralWidget->setGeometry(rc); //for iPhoneX

    int cx = rc.width();
    int cy = rc.height();
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

    m_bUseDefaultBkg = false;
    if (!m_app.getOption().bUseThemeColor)
    {
        cauto pmBkg = m_bHScreen?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
        m_bUseDefaultBkg = pmBkg.isNull();
    }

    ui.labelSingerImg->setPixmapRound(m_bUseDefaultBkg?__size(4):__szRound);

    UINT uShadowWidth = m_bUseDefaultBkg?0:1;
    ui.labelDemandHK->setShadow(uShadowWidth);
    ui.labelDemandCN->setShadow(uShadowWidth);
    ui.labelDemandKR->setShadow(uShadowWidth);
    ui.labelDemandJP->setShadow(uShadowWidth);
    ui.labelDemandTAI->setShadow(uShadowWidth);
    ui.labelDemandEN->setShadow(uShadowWidth);
    ui.labelDemandEUR->setShadow(uShadowWidth);

    ui.labelAlbumName->setShadow(uShadowWidth);

    ui.labelDuration->setShadow(uShadowWidth);

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
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        y_frameDemand = __cyIPhoneXBangs-y_frameDemand;
    }
    else if (cy > __size(1920))
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
    int x_btnMore = __size(27);

#if __android || __ios
     if (!m_bHScreen)
     {
         x_btnMore = cx - __size(25) - ui.btnMore->width();
     }

     ui.btnExit->move(-200,-200);
#else
    int x_btnExit = cx - ui.btnExit->width() - (y_frameDemand + __size(12));
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

    _showAlbumName();

    int y_PlayingListMax = 0;

    ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    ui.labelAlbumName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignBottom);

    bool bZoomoutSingerImg = m_bZoomoutSingerImg && ui.labelSingerImg->isVisible();

    auto& rcSingerImg = m_mapWidgetNewPos[ui.labelSingerImg];

    m_PlayingList.setShadow(uShadowWidth);

    ui.labelPlayingfile->setShadow(uShadowWidth);

    if (!m_bUseDefaultBkg)
    {
        int x = ui.barProgress->x();

        int cy_Playingfile = ui.labelPlayingfile->height();
        int y_Playingfile = ui.labelDuration->geometry().bottom() -  cy_Playingfile - __size(2);
        ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x() - x, cy_Playingfile);

        int cx_barProgress = ui.barProgress->width();

        int y_labelAlbumName = 0;
        if (ui.labelAlbumName->isVisible())
        {
#define __cylabelAlbumName __size(80)
            y_labelAlbumName = y_Playingfile - __cylabelAlbumName;

            ui.labelAlbumName->setGeometry(x, y_labelAlbumName, cx_barProgress, __cylabelAlbumName);
        }
        else
        {
            y_labelAlbumName = y_Playingfile - __size(20);
        }

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
                y_SingerImg = ui.frameDemandLanguage->geometry().bottom() + __size(50);
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
        ui.labelSingerImg->setGeometry(rcSingerImg);

        int y_labelSingerName = y_labelAlbumName-ui.labelSingerName->height();

        if (ui.labelSingerImg->isVisible())
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

            y_labelSingerName += __size(10);
        }

        ui.labelSingerName->setGeometry(x_SingerImg+__size(15), y_labelSingerName
                                        , cx_SingerImg-__size(15), ui.labelSingerName->height());
    }
    else
    {
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

//    auto pPixmap = ui.labelSingerImg->pixmap();
//    if (pPixmap && !pPixmap->isNull())
//    {
//        ui.labelSingerImg->setPixmap(*pPixmap);
//    }

#define __CyPlayItem __size(115)
    UINT uRowCount = 0;
    if (m_bHScreen)
    {
        UINT uMargin = __size(45);
        UINT xOffset = uMargin;
        if (cx > __size(1920))
        {
            xOffset = __size(90) * fCXRate;
        }
        else if (cx > __size(1800))
        {
            xOffset = __size(60) * fCXRate;
        }
        int x_PlayingList = ui.barProgress->geometry().right() + xOffset;

        int cx_PlayingList = cx - x_PlayingList - uMargin * fCXRate;
        m_PlayingList.setGeometry(x_PlayingList, uMargin-1, cx_PlayingList, cy-uMargin*2);

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


    if (ui.labelSingerImg->isVisible())
    {
        ui.labelSingerName->setShadow(2);
    }
    else
    {
        ui.labelSingerName->setShadow(1);
    }
}

#define __logoBkgColor QRGB(180, 220, 255)

void MainWindow::_onPaint(CPainter& painter)
{
    cauto rc = this->rect();

    if (ui.labelLogo->isVisible())
    {
        painter.fillRect(rc, QColor(__logoBkgColor));
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

void MainWindow::_updateProgress()
{
    auto& playMgr = m_app.getPlayMgr();
    E_DecodeStatus eDecodeStatus = playMgr.mediaOpaque().decodeStatus();
    _updatePlayPauseButton(E_DecodeStatus::DS_Decoding == eDecodeStatus);

    if (eDecodeStatus != E_DecodeStatus::DS_Decoding)
    {
#if __OnlineMediaLib
        if (eDecodeStatus != E_DecodeStatus::DS_Paused)
        {
            return;
        }
#else
        return;
#endif
    }

    int nProgress = int(playMgr.player().GetClock()/__1e6);
    if (nProgress <= ui.barProgress->maximum())
    {
        UINT bufferValue = 0;
#if __OnlineMediaLib
        bufferValue = UINT(m_app.getPlayMgr().mediaOpaque().size()/1000);

        if (playMgr.mediaOpaque().waitingFlag() && playMgr.player().packetQueueEmpty())
        {
            ui.labelDuration->setText("正在缓冲");
        }
        else
        {
            QString qsDuration = strutil::toQstr(CMedia::genDurationString(m_PlayingInfo.nDuration));
            ui.labelDuration->setText(qsDuration);
        }
#endif
        ui.barProgress->setValue(nProgress, bufferValue);
    }
}

void MainWindow::_updatePlayPauseButton(bool bPlaying)
{
    ui.btnPlay->setVisible(!bPlaying);
    ui.btnPause->setVisible(bPlaying);
}

void MainWindow::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual)
{
    tagPlayingInfo PlayingInfo;
    PlayingInfo.strTitle = PlayItem.GetTitle();

    PlayingInfo.nDuration = PlayItem.duration();

    PlayingInfo.uStreamSize = 0;
#if __OnlineMediaLib
    if (m_app.getPlayMgr().mediaOpaque().isOnline())
    {
        auto nStreamSize = PlayItem.fileSize()/1000;
        if (nStreamSize > 0)
        {
            PlayingInfo.uStreamSize = (UINT)nStreamSize;
        }
    }
#endif

    PlayingInfo.strQuality = PlayItem.qualityString();

    PlayingInfo.strSingerName = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Singer);
    PlayingInfo.uSingerID = PlayItem.GetRelatedMediaSetID(E_MediaSetType::MST_Singer);
    PlayingInfo.strAlbum = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Album);
    PlayingInfo.uRelatedAlbumItemID = PlayItem.GetRelatedMediaID(E_MediaSetType::MST_Album);

    PlayingInfo.strPlaylist = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Playlist);
    PlayingInfo.uRelatedPlayItemID = PlayItem.GetRelatedMediaID(E_MediaSetType::MST_Playlist);

    if (PlayingInfo.uRelatedAlbumItemID != 0)
    {
        PlayingInfo.pRelatedMedia = m_app.getSingerMgr().FindMedia(
                                E_MediaSetType::MST_Album, PlayingInfo.uRelatedAlbumItemID);
    }
    else if (PlayingInfo.uRelatedPlayItemID != 0)
    {
        PlayingInfo.pRelatedMedia = m_app.getPlaylistMgr().FindMedia(
                                E_MediaSetType::MST_Playlist, PlayingInfo.uRelatedPlayItemID);
    }
    else
    {
        PlayingInfo.pRelatedMedia = NULL;
    }

    PlayingInfo.strPath = PlayItem.GetPath();

    QVariant var;
    var.setValue(PlayingInfo);

    emit signal_showPlaying(uPlayingItem, bManual, var);
}

void MainWindow::slot_showPlaying(unsigned int uPlayingItem, bool bManual, QVariant var)
{
    m_uPlaySeq++;

    auto strPrevSinger = m_PlayingInfo.strSingerName;
    m_PlayingInfo = var.value<tagPlayingInfo>();

    m_PlayingList.updatePlayingItem(uPlayingItem, bManual);

    ui.labelPlayingfile->setText(strutil::toQstr(m_PlayingInfo.strTitle));

    if (m_PlayingInfo.strSingerName != strPrevSinger)
    {
        ui.labelSingerName->setText(strutil::toQstr(m_PlayingInfo.strSingerName));

        ui.labelSingerImg->setPixmap(QPixmap());

        ui.labelSingerImg->setVisible(false);
        _playSingerImg(true);
    }

    _updatePlayPauseButton(true);

    ui.barProgress->setValue(0);
    ui.barProgress->setMaximum(m_PlayingInfo.nDuration, m_PlayingInfo.uStreamSize);

#if __OnlineMediaLib
    ui.labelDuration->clear();
#else
    cauto qsDuration = strutil::toQstr(CMedia::genDurationString(m_PlayingInfo.nDuration));
    ui.labelDuration->setText(qsDuration);
#endif

    _relayout();
}

void MainWindow::onPlayStop(bool bCanceled, bool bOpenFail)
{
    emit signal_playStoped(bOpenFail);

    if (!bCanceled)
    {
        /*if (bOpenFail)
        {
            mtutil::usleep(1000);
        }*/

        m_app.getCtrl().callPlayCtrl(E_PlayCtrl::PC_AutoPlayNext);
    }
}

void MainWindow::slot_playStoped(bool bOpenFail)
{
    ui.barProgress->setMaximum(0);

    if (bOpenFail)
    {
        _updatePlayPauseButton(false);
    }
    else
    {
        auto uPlaySeq = m_uPlaySeq;
        __appAsync(2000, [&, uPlaySeq]() {
            if (uPlaySeq == m_uPlaySeq)
            {
                _updatePlayPauseButton(false);
            }
        });
    }
}

void MainWindow::_showAlbumName()
{
    WString strMediaSet;

    auto eDemandMode = m_app.getPlayMgr().demandMode();
    if (E_DemandMode::DM_DemandPlayItem == eDemandMode
            || E_DemandMode::DM_DemandPlaylist == eDemandMode
            || m_PlayingInfo.strAlbum.empty())
    {
        if (!m_PlayingInfo.strPlaylist.empty())
        {
            if (!m_bUseDefaultBkg)
            {
                strMediaSet << L"歌单:  ";
            }
            strMediaSet << m_PlayingInfo.strPlaylist;

            m_PlayingInfo.uRelatedAlbumItemID = 0;
        }
    }
    else if (!m_PlayingInfo.strAlbum.empty())
    {
        if (!m_bUseDefaultBkg)
        {
            strMediaSet << L"专辑:  ";
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

    labelAlbumName.setFont(0.95);

    if (m_bUseDefaultBkg)
    {
        cauto rcAlbumNamePrev = m_mapWidgetNewPos[&labelAlbumName];
        labelAlbumName.adjustSize();
        if (labelAlbumName.width() > rcAlbumNamePrev.width())
        {
            labelAlbumName.adjustFont(0.9f);
            labelAlbumName.adjustSize();

            if (labelAlbumName.width() < rcAlbumNamePrev.width())
            {
                labelAlbumName.move(rcAlbumNamePrev.left(), labelAlbumName.y());
                return;
            }

        }

        int x_labelAlbumName = rcAlbumNamePrev.left() + (rcAlbumNamePrev.width()-labelAlbumName.width())/2;
        labelAlbumName.move(x_labelAlbumName, labelAlbumName.y());
    }
}

#define ___singerImgElapse 8

void MainWindow::_playSingerImg(bool bReset)
{
    if (m_PlayingInfo.strSingerName.empty())
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
        if (uTickCount >= ___singerImgElapse)
        {
            uTickCount = 0;
        }
        else
        {
            uTickCount++;
            return;
        }
    }

    cauto strSingerImg = m_app.getSingerImgMgr().getSingerImg(m_PlayingInfo.strSingerName, uSingerImgIdx);
    if (!strSingerImg.empty())
    {
        QPixmap pm;
        if (pm.load(strutil::toQstr(strSingerImg)))
        {
            ui.labelSingerImg->setPixmap(pm);

            if (!ui.labelSingerImg->isVisible())
            {
                ui.labelSingerImg->setVisible(true);
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
        _updatePlayPauseButton(false);
    }
    else if (button == ui.btnPlay)
    {
        if (E_PlayStatus::PS_Pause == m_app.getPlayMgr().playStatus())
        {
            _updatePlayPauseButton(true);
        }
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Play));
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
        if (m_PlayingInfo.pRelatedMedia)
        {
            m_medialibDlg.showMedia(*m_PlayingInfo.pRelatedMedia);
        }
    }
    else if (label == ui.labelPlayingfile)
    {
        if (m_PlayingInfo.pRelatedMedia)
        {
            m_medialibDlg.showMedia(*m_PlayingInfo.pRelatedMedia);
        }
        else
        {
            (void)m_medialibDlg.showFile(m_PlayingInfo.strPath);
        }
    }
    else if (label == ui.labelProgress)
    {
        if (!m_app.getPlayMgr().mediaOpaque().seekable())
        {
            return;
        }
        if (m_app.getPlayMgr().playStatus() != E_PlayStatus::PS_Play)
        {
            return;
        }
        if (!m_app.getPlayMgr().mediaOpaque().decoderOpened())
        {
            return;
        }

        cauto barProgress = *ui.barProgress;
        auto nMax = barProgress.maximum();
        if (nMax <= 0)
        {
            return;
        }

        auto nSeekPos = pos.x() * nMax / barProgress.width();
        auto nCurrent = barProgress.value();
        if (nSeekPos > nCurrent && barProgress.bufferValue() < barProgress.maxBuffer())
        {
            int nPlayablePos = nMax*barProgress.bufferValue()/barProgress.maxBuffer() - __ReadStreamWaitTime;
            nPlayablePos = MAX(nPlayablePos, nCurrent);
            nSeekPos = MIN(nSeekPos, nPlayablePos);
        }

        m_app.getPlayMgr().player().Seek(nSeekPos);
        //__appAsync(100, [&](){
        //  _updateProgress();
        //});
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
            }
            else
            {
                if (lblLanguage == label)
                {
                    m_eDemandLanguage = eLanguage;
                    lblLanguage->setText(__qsCheck + lblLanguage->text());
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

void MainWindow::drawDefaultBkg(CPainter& painter, cqrc rc, UINT xround, UINT yround)
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

    painter.drawPixmap(rc, *ui.labelBkg->pixmap(), rcSrc, xround, yround);
}
