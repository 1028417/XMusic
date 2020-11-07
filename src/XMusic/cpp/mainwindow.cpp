
#include "app.h"

#include "widget.cpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMovie>

#include <QScreen>

#define __size10 __size(10)

#define __round(f) (int)roundf(f)

#define __demandplayitemPngCount 4

static Ui::MainWindow ui;

static bool g_bFullScreen = true;

QColor g_crLogoBkg(180, 220, 255);

#if __windows
inline static void _fixWorkArea(QWidget& wnd)
{
    const RECT& rcWorkArea = getWorkArea(g_bFullScreen);

    auto cx = rcWorkArea.right-rcWorkArea.left+1;
    auto cy = rcWorkArea.bottom-rcWorkArea.top+1;
    wnd.setGeometry(rcWorkArea.left, rcWorkArea.top, cx, cy);
    ::MoveWindow((HWND)wnd.winId(), rcWorkArea.left, rcWorkArea.top, cx, cy, TRUE);
}
#else
#define _fixWorkArea(wnd)
#endif

void fixWorkArea(QWidget& wnd)
{
    if (__android || __ios || g_bFullScreen)
    {
        wnd.setWindowState(Qt::WindowFullScreen | Qt::WindowActive); // Mac需要WindowActive
    }
    else
    {
        wnd.setWindowState(Qt::WindowMaximized | Qt::WindowActive);
    }

    _fixWorkArea(wnd);
}

void MainWindow::switchFullScreen()
{
    g_bFullScreen = !g_bFullScreen;
    m_app.getOption().bFullScreen = g_bFullScreen;

#if __windows
    if (g_bFullScreen)
    {
        ::SetWindowPos(hwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    }
#endif

    fixWorkArea(*this);

#if __windows
    if (g_bFullScreen)
    {
        ::SetWindowPos(hwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    }
#endif
}

MainWindow::MainWindow(CApp& app)
    : m_app(app)
    , m_PlayingList(app, this)
    , m_medialibDlg(*this, app)
    , m_bkgDlg(*this, app)
{
    ui.setupUi(this);

    for (auto pWidget : SList<QWidget*>(
             ui.btnExit, ui.frameDemand, ui.btnMore, ui.btnDemandSinger, ui.btnDemandAlbum
             , ui.btnDemandAlbumItem, ui.btnDemandPlayItem, ui.btnDemandPlaylist
             , ui.frameDemandLanguage, ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR
             , ui.labelDemandJP, ui.labelDemandEN, ui.labelDemandEUR))
    {
        m_mapTopWidgetPos[pWidget] = pWidget->geometry();
    }

    for (auto pWidget : SList<QWidget*>(
             ui.labelPlayingfile, ui.labelSingerImg, ui.labelSingerName, ui.labelAlbumName
             , ui.labelDuration, ui.progressbar, ui.labelProgress, ui.btnPlay, ui.btnPause
             , ui.btnPlayPrev, ui.btnPlayNext, ui.btnSetting, ui.btnOrder, ui.btnRandom))
    {
        m_mapWidgetPos[pWidget] = pWidget->geometry();
    }

    for (auto button : SList<CButton*>(ui.btnFullScreen, ui.btnExit, ui.btnMore
                , ui.btnDemandSinger, ui.btnDemandAlbum, ui.btnDemandAlbumItem
                , ui.btnDemandPlayItem, ui.btnDemandPlaylist
                , ui.btnSetting, ui.btnOrder, ui.btnRandom
                , ui.btnPause, ui.btnPlay, ui.btnPlayPrev, ui.btnPlayNext))
    {
        connect(button, &CButton::signal_clicked, this, &MainWindow::slot_buttonClicked);
    }

    m_PlayingList.raise();

#if __android || __ios
    ui.btnFullScreen->setVisible(false);
    ui.btnExit->setVisible(false);

#else
    ui.btnFullScreen->setParent(this);

    ui.btnExit->setParent(this);
    ui.btnExit->raise();
#endif

#if __android || __ios
#endif

    ui.labelLogo->setParent(this);
    ui.labelLogoTip->setParent(this);
    ui.labelLogoCompany->setParent(this);

    ui.btnPause->setVisible(false);

    ui.labelBkg->setVisible(false);

    ui.centralWidget->setVisible(false);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
}

void MainWindow::showLogo()
{
    for (auto widget : SList<QWidget*>(ui.labelLogo, ui.labelLogoTip, ui.labelLogoCompany
                                       , ui.centralWidget, ui.frameDemand, ui.frameDemandLanguage))
    {
        widget->setAttribute(Qt::WA_TranslucentBackground);
    }

    float fFontSizeOffset = 1.072f;
#if __android || __ios
    fFontSizeOffset = 0.918f;

    cauto szScreen = QApplication::primaryScreen()->size();
    int nScreenSize = MIN(szScreen.width(), szScreen.height());
    int nLogoWidth = nScreenSize*42/100;

    ui.labelLogo->setScaledContents(true);
    ui.labelLogo->resize(nLogoWidth, nLogoWidth/4);
#endif

    ui.labelLogoTip->setFont(CFont(fFontSizeOffset, g_nDefFontWeight, true));
    ui.labelLogoCompany->setFont(CFont(fFontSizeOffset));

    CDialog::setWidgetTextColor(ui.labelLogoTip, QColor(__crLogoText));
    CDialog::setWidgetTextColor(ui.labelLogoCompany, QColor(__crLogoText, 0));

    auto movie = new QMovie(this);
    movie->setFileName(":/img/logo.gif");
    ui.labelLogo->setVisible(false);
    ui.labelLogo->setMovie(movie);

    g_bFullScreen = m_app.getOption().bFullScreen;
    fixWorkArea(*this);
    this->setVisible(true);

    this->repaint();

    UINT uDelayTime = 600;
#if __android
    uDelayTime = 100;
#endif
    CApp::async(uDelayTime, [&](){
        ui.labelLogo->movie()->start();
        ui.labelLogo->setVisible(true);

        _updateLogoCompany(5, [&](){
            _updateLogoCompany(-5, [&](){
                ui.labelLogoCompany->setText(__WS2Q(L"v" + m_app.appVersion()));
                _updateLogoCompany(5);
            });
        });

        CApp::async(50, [&](){
            auto labelLogoTip = ui.labelLogoTip;
            labelLogoTip->setText("播放器");

            CApp::async(600, [=](){
                labelLogoTip->setText(labelLogoTip->text() + WString(__CNDot L"媒体库"));

                CApp::async(600, [=](){
                    labelLogoTip->setText(labelLogoTip->text() + "  个性化定制");

                    CApp::async(2600, [=](){
                        _showUpgradeProgress();
                    });
                });
            });
        });
    });
}

void MainWindow::_updateLogoCompany(int nAlphaOffset, cfn_void cb)
{
    auto labelLogoCompany = ui.labelLogoCompany;
    auto peCompany = labelLogoCompany->palette();
    auto crCompany = peCompany.color(QPalette::WindowText);
    auto nAlpha = crCompany.alpha();

    timerutil::setTimerEx(35, [=]()mutable{
        nAlpha += nAlphaOffset;
        if (nAlpha < 5 || nAlpha > 255)
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

void MainWindow::_showUpgradeProgress()
{
    extern int g_nAppUpgradeProgress;
    if (-1 == g_nAppUpgradeProgress)
    {
#define __logoTip "更新媒体库"
        ui.labelLogoTip->setText(__logoTip);
    }

    UINT uDotCount = 0;

    timerutil::setTimerEx(300, [=]()mutable{
        if (!ui.labelLogoTip->isVisible())
        {
            return false;
        }

        if (g_nAppUpgradeProgress >= 0)
        {
            QString qsText;
            if (0 == g_nAppUpgradeProgress)
            {
                qsText.append("下载升级包...");
            }
            else if (100 == g_nAppUpgradeProgress)
            {
                qsText.append("准备安装...");
            }
            else
            {
                qsText.sprintf("下载升级包:  %u%%", (UINT)g_nAppUpgradeProgress);
            }
            ui.labelLogoTip->setText(qsText);
        }
        else
        {
            uDotCount++;
            if (uDotCount > 3)
            {
                uDotCount = 0;
            }

            QString qsText(__logoTip);
            for (UINT uIdx = 1; uIdx <= 3; uIdx++)
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

        return true;
    });
}

void MainWindow::_init()
{
    qRegisterMetaType<QVariant>("QVariant");

    SList<CLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR
                , ui.labelDemandJP, ui.labelDemandEN, ui.labelDemandEUR};
    for (auto label : lstLabels)
    {
        label->setFont(1.03f, QFont::Weight::DemiBold);
    }
    lstLabels.add(ui.labelSingerImg, ui.labelSingerName, ui.labelAlbumName, ui.labelPlayingfile
                , ui.labelProgress);
    for (auto label : lstLabels)
    {
        connect(label, &CLabel::signal_click, this, &MainWindow::slot_labelClick);
    }

    ui.labelSingerName->setFont(0.95f);

    ui.labelPlayingfile->setFont(0.95f);

    ui.labelDuration->setFont(0.8f);

    m_PlayingList.setFont(0.9f);

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

#if __android
    g_fnAccelerometerNotify = [&](int x, int y, int z){
        if (abs(x) >= abs(y) && abs(x) >= abs(z))
        {
            if (abs(x) >= 20)
            {
                vibrate();
                slot_buttonClicked(ui.btnPlayNext);
            }
        }
    };
#endif
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
    //_relayout();

    m_PlayingList.updateList(m_app.getOption().uPlayingItem);

    (void)startTimer(1000);

    auto nLogoBkgAlpha = g_crLogoBkg.alpha();
    UINT uOffset = 23;
#if __windows || __mac
    if (std::thread::hardware_concurrency() > 4)
    {
        uOffset = 1;
    }
#endif
    UINT uDelayTime = m_app.getOption().bUseBkgColor?50:30;
    timerutil::setTimerEx(uDelayTime, [=]()mutable{
        uOffset+=1;
        nLogoBkgAlpha -= uOffset;
        if (nLogoBkgAlpha <= 0)
        {
            g_crLogoBkg.setAlpha(0);
            update();

#if __windows
            m_app.setForeground();
#endif

            return false;
        }

        g_crLogoBkg.setAlpha(nLogoBkgAlpha);
        update();
        return true;
    });
}

bool MainWindow::event(QEvent *ev)
{
    switch (ev->type())
    {
    case QEvent::Move:
    case QEvent::Resize:
        if (this->windowState() != Qt::WindowMinimized)
        {
            _fixWorkArea(*this);

            _relayout();

            CDialog::resetPos();
        }

        break;
    case QEvent::Paint:
        _onPaint();

        break;
#if __windows || __mac
    case QEvent::MouseButtonDblClick:
        if (!m_PlayingList.geometry().contains(((QMouseEvent*)ev)->pos()))
        {
            switchFullScreen();
        }

        break;
#endif
    case QEvent::KeyRelease:
    {
#if __android || __ios
        if (__android && Qt::Key_Back != ((QKeyEvent*)ev)->key())
        {
            break;
        }

        if (ui.labelLogo->isVisible())
        {
            break;
        }

        static time_t prevTime = 0;
        time_t currTime = time(0);
        if (currTime - prevTime > 3)
        {
#if __android
            vibrate();
#endif
            prevTime = currTime;
            return true;
        }
        prevTime = currTime;

#else
#if __windows //规避qt5.13.2版本windows不停触发
        static time_t prevTime = 0;
        time_t currTime = time(0);
        if (currTime - prevTime == 0)
        {
            break;
        }
        prevTime = currTime;
#endif

        auto key = ((QKeyEvent*)ev)->key();
        if (Qt::Key_Return == key)
        {
            switchFullScreen();
        }
/*        else if (Qt::Key_Escape == key)
        {
#if __mac
            this->setWindowState(Qt::WindowMaximized | Qt::WindowActive); // Mac最小化不了，执行还原
#else
            this->setWindowState(Qt::WindowMinimized); // Qt5.13.2 Windows弹窗后也有bug
#endif
        }*/
        else if (!ui.labelLogo->isVisible())
        {
            // TODO 上下键滚动播放列表、左右键切换背景
            if (Qt::Key_Space == key)
            {
                if (E_PlayStatus::PS_Play == m_app.getPlayMgr().playStatus())
                {
                    slot_buttonClicked(ui.btnPause);
                }
                else
                {
                    slot_buttonClicked(ui.btnPlay);
                }
            }
            else if (Qt::Key_Left == key || Qt::Key_Up == key)
            {
                slot_buttonClicked(ui.btnPlayPrev);
            }
            else if (Qt::Key_Right == key || Qt::Key_Down == key)
            {
                slot_buttonClicked(ui.btnPlayNext);
            }
        }
#endif
    }

    break;
/*    case QEvent::Close:
        m_app.quit();

        break;*/
    case QEvent::Timer:
        _updateProgress();

        _playSingerImg(false);

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
    m_bHLayout = cx > cy; // 橫屏

    int x_Logo = (cx - ui.labelLogo->width())/2;
    int y_Logo = (cy - ui.labelLogo->height())/2;
    if (m_bHLayout)
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
    if (m_bHLayout)
    {
        ui.labelLogoCompany->setAlignment(Qt::AlignmentFlag::AlignBottom | Qt::AlignmentFlag::AlignRight);
    }
    else
    {
        ui.labelLogoCompany->setAlignment(Qt::AlignmentFlag::AlignBottom | Qt::AlignmentFlag::AlignHCenter);
    }

    float fCXRate = 0;
    if (m_bHLayout)
    {
        fCXRate = (float)cx/ui.labelBkg->pixmap()->width();
    }
    else
    {
        fCXRate = (float)cx/1080;
    }
    float fCXRateEx = fCXRate*g_fPixelRatio;

    int cy_bkg = __round(fCXRate * ui.labelBkg->pixmap()->height());
    int dy_bkg = cy - cy_bkg;

    m_bDefaultBkg = false;
    if (!m_app.getOption().bUseBkgColor)
    {
        cauto pmBkg = m_bHLayout?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
        m_bDefaultBkg = pmBkg.isNull();
    }

    UINT uShadowWidth = m_bDefaultBkg?0:1;
    ui.labelDemandHK->setShadow(uShadowWidth);
    ui.labelDemandCN->setShadow(uShadowWidth);
    ui.labelDemandKR->setShadow(uShadowWidth);
    ui.labelDemandJP->setShadow(uShadowWidth);
    ui.labelDemandEN->setShadow(uShadowWidth);
    ui.labelDemandEUR->setShadow(uShadowWidth);

    for (cauto widgetPos : m_mapTopWidgetPos)
    {
        QRect pos = widgetPos.second;
        if (fCXRateEx < 1)
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
        if (fCXRateEx < 1 || NULL == dynamic_cast<QPushButton*>(widgetPos.first))
        {
            width = __round(width * fCXRate);
            height =__round(height * fCXRate);
        }

        /*if (width%2==1)
        {
            width++;
        }
        if (height%2==1)
        {
            height++;
        }*/

        newPos.setRect(__round(fCXRate*pos.center().x()) - width/2
                       , __round(fCXRate*pos.center().y()) - height/2 + dy_bkg, width, height);
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
        if (m_bDefaultBkg)
        {
            y_frameDemand = __size(12);
        }
    }

    int x_frameDemand = 0;
    if (m_bHLayout)
    {
        x_frameDemand = (ui.progressbar->geometry().right()
            + ui.progressbar->x() - ui.frameDemand->width())/2;
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
     if (!m_bHLayout)
     {
         x_btnMore = cx - __size(25) - ui.btnMore->width();
     }
#endif
    ui.btnMore->move(x_btnMore, y_btnMore);

    int x_btnExit = cx - ui.btnExit->width() - (y_frameDemand + __size(12));
    ui.btnExit->move(x_btnExit, y_btnMore);

    if (!m_bDefaultBkg)
    {
        int yOffset = 0;

        if (fCXRateEx <= 1)
        {
#define __offset __size(10.0f)
            yOffset = __round(__offset/fCXRate);

            for (auto pWidget : SList<QWidget*>({ui.labelDuration, ui.progressbar, ui.labelProgress}))
            {
                pWidget->move(pWidget->x(), pWidget->y() - yOffset*2);
            }
        }

#define __dy __size(4)
        int dy =  __round(fCXRate*__dy);
        for (auto pWidget : SList<QWidget*>(ui.btnPlay, ui.btnPause, ui.btnPlayPrev, ui.btnPlayNext
                                            , ui.btnSetting, ui.btnOrder, ui.btnRandom))
        {
            pWidget->move(pWidget->x(), pWidget->y() + dy - yOffset);
        }
    }

    CLabel& labelAlbumName = *ui.labelAlbumName;
    cauto strMediaSet = _genAlbumName();
    if (strMediaSet->empty())
    {
        labelAlbumName.setVisible(false);
    }
    else
    {
        labelAlbumName.setVisible(true);
        labelAlbumName.setText(strMediaSet);
        labelAlbumName.setFont(0.95f);

        labelAlbumName.setShadow(uShadowWidth);
        labelAlbumName.setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    }

    ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);

    m_PlayingList.setShadow(uShadowWidth);

    ui.labelPlayingfile->setShadow(uShadowWidth);
    ui.labelDuration->setShadow(uShadowWidth);

    E_SingerImgPos eSingerImgPos = E_SingerImgPos::SIP_Float;
    cauto pmSingerImg = m_PlayingInfo.bWholeTrack ?
                ((int)m_PlayingInfo.eQuality>=(int)E_MediaQuality::MQ_CD
                 ? m_app.m_pmHDDisk : m_app.m_pmLLDisk)
              : ui.labelSingerImg->pixmap();
    if (m_PlayingInfo.bWholeTrack)
    {
        eSingerImgPos = E_SingerImgPos::SIP_Zoomout;

        ui.labelSingerImg->setPixmapRound(0);
        ui.labelSingerImg->setShadow(0);

        ui.labelSingerName->setShadow(uShadowWidth);
    }
    else
    {
        if (!pmSingerImg.isNull())
        {
            eSingerImgPos = m_eSingerImgPos;

            ui.labelSingerImg->setPixmapRound(m_bDefaultBkg?__size(5):__szRound);
            ui.labelSingerImg->setShadow(2);

            ui.labelSingerName->setShadow(2);
        }
        else
        {
            ui.labelSingerName->setShadow(uShadowWidth);
        }
    }

    auto rcSingerImg = m_mapWidgetNewPos[ui.labelSingerImg];

    int y_PlayingListMax = 0;

    if (!m_bDefaultBkg)
    {
        ui.labelDuration->move(ui.labelDuration->x(), ui.labelDuration->y() - __size(12));
        int cx_progressbar = ui.progressbar->width();

        int cy_Playingfile = ui.labelPlayingfile->height();
        int y_Playingfile = ui.labelDuration->geometry().bottom() -  cy_Playingfile;

#define __cylabelAlbumName __size(70)
        int y_labelAlbumName = 0;
        if (labelAlbumName.isVisible())
        {
            y_labelAlbumName = y_Playingfile - __cylabelAlbumName;
        }
        else
        {
            y_labelAlbumName = y_Playingfile - __size(20);
        }

        int x = ui.progressbar->x();
        int x_SingerImg = x;
        int cx_SingerImg = 0;
        int y_SingerImg = 0;
        int cy_SingerImg = 0;
        if (E_SingerImgPos::SIP_Zoomout == eSingerImgPos)
        {
            int y_SingerName = y_labelAlbumName - cy_Playingfile;
            if (m_PlayingInfo.bWholeTrack)
            {
                cy_SingerImg = __size(70);
                y_SingerImg = y_Playingfile+cy_Playingfile - cy_SingerImg;

            }
            else
            {
                y_SingerImg = y_SingerName + __size(6);
                cy_SingerImg = y_Playingfile+cy_Playingfile - y_SingerImg - __size(4);
            }

            cx_SingerImg = cy_SingerImg;

            auto dx = cx_SingerImg + __size(25);
            x += dx;

            labelAlbumName.setGeometry(x, y_labelAlbumName, cx_progressbar-dx, __cylabelAlbumName);
            labelAlbumName.setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

            ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
            ui.labelSingerName->setGeometry(x, y_SingerName, cx_progressbar-dx, __cylabelAlbumName);

            y_PlayingListMax = y_SingerImg;
        }
        else
        {
            if (E_SingerImgPos::SIP_Float == eSingerImgPos)
            {
                if (m_bHLayout)
                {
                    y_SingerImg = ui.frameDemandLanguage->geometry().bottom() + __size(50);
                }
                else
                {
                    y_SingerImg = cy/2+__size(150);
                }

                cx_SingerImg = cy_SingerImg = y_labelAlbumName-y_SingerImg;

                if (!pmSingerImg.isNull())
                {
                    //cx_SingerImg = cy_SingerImg*1.05; //cy_SingerImg * rcSingerImg.width() / rcSingerImg.height();

                    cx_SingerImg = cy_SingerImg * pmSingerImg.width() / pmSingerImg.height();
                    cx_SingerImg = MIN(cx_SingerImg, cy_SingerImg*1.2f);
                    cx_SingerImg = MIN(cx_SingerImg, ui.progressbar->width());
                }

                x_SingerImg += (cx_progressbar-cx_SingerImg)/2;
            }
            else
            {
                cx_SingerImg = cy_SingerImg = __size(300);
                y_SingerImg = y_labelAlbumName-cy_SingerImg;

                ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);
                labelAlbumName.setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);
            }

            labelAlbumName.setGeometry(x, y_labelAlbumName, cx_progressbar, __cylabelAlbumName);

            int y_labelSingerName = y_labelAlbumName-ui.labelSingerName->height();
            if (!pmSingerImg.isNull())
            {
                y_PlayingListMax = y_SingerImg;
            }
            else
            {
                if (!ui.labelSingerName->text().isEmpty())
                {
                    y_labelSingerName += __size10;
                    y_PlayingListMax = y_labelSingerName;
                }
                else
                {
                    y_PlayingListMax = y_labelAlbumName;
                }
                y_PlayingListMax += __size10;
            }

            ui.labelSingerName->setGeometry(x_SingerImg+__size(15), y_labelSingerName
                                            , cx_SingerImg-__size(15), ui.labelSingerName->height());
        }

        ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x()-x, cy_Playingfile);

        rcSingerImg.setRect(x_SingerImg, y_SingerImg, cx_SingerImg, cy_SingerImg);
        ui.labelSingerImg->setGeometry(rcSingerImg);

        if (m_PlayingInfo.bWholeTrack)
        {
            cauto pm = pmSingerImg.scaledToWidth(rcSingerImg.width(), Qt::SmoothTransformation);
            ui.labelSingerImg->setPixmap(pm);
        }
    }
    else
    {
        if (m_PlayingInfo.bWholeTrack)
        {
            ui.labelSingerImg->clear();
        }

        cauto rcAlbumNamePrev = m_mapWidgetNewPos[&labelAlbumName];
        labelAlbumName.adjustSize();
        do {
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
        } while (0);

        y_PlayingListMax = rcSingerImg.top();
    }

#define __CyPlayItem __size(115)
    UINT uRowCount = 0;
    if (m_bHLayout)
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
        int x_PlayingList = ui.progressbar->geometry().right() + xOffset;

        int cx_PlayingList = cx - x_PlayingList;// - uMargin * fCXRate;
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

            if (pmSingerImg.isNull() || E_SingerImgPos::SIP_Zoomout == eSingerImgPos)
            {
                y_Margin += __size10*2;
            }
            else if (E_SingerImgPos::SIP_Dock == eSingerImgPos)
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

void MainWindow::_onPaint()
{
    CPainter painter(*this);
    cauto rc = this->rect();

    auto nLogoAlpha = g_crLogoBkg.alpha();
    auto nBkgAlpha = 255-nLogoAlpha;

    if (nBkgAlpha > 0)
    {
        if (m_app.getOption().bUseBkgColor)
        {
            painter.fillRect(rc, g_crBkg);
        }
        else
        {
            painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

            bool bHLayout = rc.width() > rc.height();
            cauto pmBkg = bHLayout?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
            if (!pmBkg.isNull())
            {
               painter.drawPixmapEx(rc, pmBkg, m_dxbkg, m_dybkg);

               //auto cx = ui.progressbar->width();
               //auto cy = cx * m_pmDiskFace.height()/m_pmDiskFace.width();
               //cauto rcSingerImg = m_mapWidgetNewPos[ui.labelSingerImg];
               //QRect rcDst(rcSingerImg.x(), rcSingerImg.y(), cx, cy);
               //painter.drawPixmap(rcDst, m_pmDiskFace);
            }
            else
            {
                drawDefaultBkg(painter, rc);
            }
        }
    }

    if (nLogoAlpha > 0)
    {
        painter.fillRect(rc, g_crLogoBkg);
    }
}

void MainWindow::_updateProgress()
{
    auto& playMgr = m_app.getPlayMgr();
    E_DecodeStatus eDecodeStatus = playMgr.mediaOpaque().decodeStatus();
    _updatePlayPauseButton(E_DecodeStatus::DS_Decoding == eDecodeStatus); // for seek

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

#if __OnlineMediaLib
    if (playMgr.mediaOpaque().waitingFlag() && playMgr.player().packetQueueEmpty())
    {
        ui.labelDuration->setText("正在缓冲");
    }
    else
    {
        ui.labelDuration->setText(m_PlayingInfo.qsDuration);
    }

    UINT uBuffer = UINT(m_app.getPlayMgr().mediaOpaque().downloadedSize()/1000);
    if (uBuffer > 0)
    {
        ui.progressbar->setBuffer(uBuffer, m_PlayingInfo.uFileSize);
    }
#endif

    UINT uProgress = UINT(playMgr.mediaOpaque().clock()/__1e6);
    ui.progressbar->setValue(uProgress);
}

void MainWindow::_updatePlayPauseButton(bool bPlaying)
{
    ui.btnPlay->setVisible(!bPlaying);
    ui.btnPause->setVisible(bPlaying);
}

void MainWindow::onPlayingListUpdated(int nPlayingItem, bool bSetActive)
{
    (void)bSetActive;
    m_app.sync([=](){
        m_PlayingList.updateList(nPlayingItem);
    });
}

void MainWindow::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual)
{
    tagPlayingInfo PlayingInfo;
    PlayingInfo.qsTitle = __WS2Q(PlayItem.GetTitle());

    PlayingInfo.strPath = PlayItem.GetPath();

    UINT uDuration = PlayItem.duration();
    PlayingInfo.qsDuration = __WS2Q(CMedia::genDurationString(uDuration));
    if (uDuration > __wholeTrackDuration)
    {
        CMediaRes *pMediaRes = __medialib.findSubFile(PlayingInfo.strPath);
        if (pMediaRes && pMediaRes->parent()->dirType() == E_MediaDirType::MDT_Snapshot)
        {
            PlayingInfo.bWholeTrack = true;
        }
    }

#if __OnlineMediaLib
    auto nFileSize = PlayItem.fileSize();
    if (nFileSize > 0)
    {
        PlayingInfo.uFileSize = UINT(nFileSize/1000);
    }
#endif

    //if (m_app.getPlayMgr().mediaOpaque().isOnline()) // 本地视频文件不能按码率算
    {
        PlayingInfo.eQuality = PlayItem.quality();
        PlayingInfo.qsQuality = mediaQualityString(PlayingInfo.eQuality);
    }

    PlayingInfo.strSingerName = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
    PlayingInfo.uSingerID = PlayItem.GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
    PlayingInfo.strAlbum = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Album);
    PlayingInfo.uRelatedAlbumItemID = PlayItem.GetRelatedMediaID(E_RelatedMediaSet::RMS_Album);

    PlayingInfo.strPlaylist = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Playlist);
    PlayingInfo.uRelatedPlayItemID = PlayItem.GetRelatedMediaID(E_RelatedMediaSet::RMS_Playlist);

    if (PlayingInfo.uRelatedAlbumItemID != 0)
    {
        PlayingInfo.pRelatedMedia = m_app.getSingerMgr().GetMedia(
                                E_MediaSetType::MST_Album, PlayingInfo.uRelatedAlbumItemID);
    }
    else if (PlayingInfo.uRelatedPlayItemID != 0)
    {
        PlayingInfo.pRelatedMedia = m_app.getPlaylistMgr().GetMedia(
                                E_MediaSetType::MST_Playlist, PlayingInfo.uRelatedPlayItemID);
    }
    else
    {
        PlayingInfo.pRelatedMedia = NULL;
    }

    m_app.sync([=](){
        auto strPrevSinger = m_PlayingInfo.strSingerName;
        m_PlayingInfo = PlayingInfo;

        ui.labelPlayingfile->setText(m_PlayingInfo.qsTitle);

        ui.progressbar->setValue(0, uDuration);

    #if __OnlineMediaLib
        ui.labelDuration->clear();
    #else
        ui.labelDuration->setText(m_PlayingInfo.qsDuration);
    #endif

        _updatePlayPauseButton(true);

        m_PlayingList.updatePlayingItem(uPlayingItem, bManual);

        cauto qsSingerName = __WS2Q(m_PlayingInfo.strSingerName);
        /*if (m_PlayingInfo.qsTitle.indexOf(qsSingerName) >= 0)
        {
            ui.labelSingerName->clear();
        }
        else*/
        {
            ui.labelSingerName->setText(qsSingerName);
        }

        if (m_PlayingInfo.strSingerName.empty())
        {
            ui.labelSingerImg->clear();
        }
        else if (m_PlayingInfo.strSingerName != strPrevSinger)
        {
            ui.labelSingerImg->clear();
            _playSingerImg(true);
        }

        _relayout();
    });
}

void MainWindow::onPlayStop(bool bOpenSuccess, bool bPlayFinish)
{
    m_app.sync([=](){
        ui.progressbar->set(0, 0, 0, 0);

        ui.labelDuration->setText(m_PlayingInfo.qsDuration);

        extern UINT g_uPlaySeq;
        if (bPlayFinish)
        {
            m_app.getCtrl().callPlayCmd(E_PlayCmd::PC_AutoPlayNext);

            /*auto uPlaySeq = g_uPlaySeq;
            CApp::async(1000, [=]() {
                if (uPlaySeq == g_uPlaySeq)
                {
                    _updatePlayPauseButton(false);
                }
            });*/
        }
        else if (!bOpenSuccess)
        {
            //_updatePlayPauseButton(false);

            auto uPlaySeq = g_uPlaySeq;
            CApp::async(2000, [=]() {
                if (uPlaySeq == g_uPlaySeq)
                {
                    m_app.getCtrl().callPlayCmd(E_PlayCmd::PC_AutoPlayNext);
                }
            });
        }
    });
}

void MainWindow::onSingerImgDownloaded(cwstr strSingerName, const tagSingerImg& singerImg)
{
    if (m_medialibDlg.isVisible())
    {
        m_app.sync([&, strSingerName](){
            if (m_medialibDlg.isVisible())
            {
                m_medialibDlg.updateSingerImg(strSingerName, singerImg);
            }
        });
    }

    if (singerImg.isSmall() || singerImg.isPiiic())
    {
        return;
    }

    if (m_PlayingInfo.strSingerName == strSingerName && ui.labelSingerImg->pixmap().isNull())
    {
        m_app.sync([&](){
            _playSingerImg();
        });
    }
}

WString MainWindow::_genAlbumName()
{
    WString strMediaSet;

    auto eDemandMode = m_app.getPlayMgr().demandMode();
    if (E_DemandMode::DM_DemandPlayItem == eDemandMode
            || E_DemandMode::DM_DemandPlaylist == eDemandMode
            || m_PlayingInfo.strAlbum.empty())
    {
        if (!m_PlayingInfo.strPlaylist.empty())
        {
            if (!m_bDefaultBkg)
            {
                strMediaSet << L"歌单:  ";
            }
            strMediaSet << m_PlayingInfo.strPlaylist;

            m_PlayingInfo.uRelatedAlbumItemID = 0;
        }
    }
    else if (!m_PlayingInfo.strAlbum.empty())
    {
        if (!m_bDefaultBkg)
        {
            strMediaSet << L"专辑:  ";
        }
        strMediaSet << m_PlayingInfo.strAlbum;

        m_PlayingInfo.uRelatedPlayItemID = 0;
    }

    return strMediaSet;
}

#define ___singerImgElapse 8

static UINT g_uSingerImgIdx = 0;

void MainWindow::_playSingerImg(bool bReset)
{
    if (m_PlayingInfo.strSingerName.empty())
    {
        return;
    }

    static UINT uTickCount = 0;
    if (bReset)
    {
        uTickCount = 0;
        g_uSingerImgIdx = 0;
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

    _playSingerImg();
}

void MainWindow::_playSingerImg()
{
    auto pSingerImg = m_app.getSingerImgMgr().getSingerImg(m_PlayingInfo.strSingerName, g_uSingerImgIdx, true);
    if (NULL == pSingerImg)
    {
        if (g_uSingerImgIdx > 1)
        {
            _playSingerImg(true);
        }

        return;
    }

    if (!pSingerImg->bExist)
    {
        return;
    }

    QPixmap pm;
    (void)pm.load(__WS2Q(m_app.getSingerImgMgr().file(*pSingerImg)));
    ui.labelSingerImg->setPixmap(pm);

    //if (!ui.labelSingerImg->isVisible()) ui.labelSingerImg->setVisible(true);

    _relayout();

    g_uSingerImgIdx++;
    (void)m_app.getSingerImgMgr().getSingerImg(m_PlayingInfo.strSingerName, g_uSingerImgIdx, true);
}

void MainWindow::slot_buttonClicked(CButton* button)
{
    if (button == ui.btnFullScreen)
    {
        switchFullScreen();
    }
    else if (button == ui.btnExit)
    {
        m_app.quit();
    }
    else if (button == ui.btnMore)
    {
        m_medialibDlg.show();
    }
    else if (button == ui.btnSetting)
    {
        m_bkgDlg.show();
    }
    else if (button == ui.btnPause)
    {
        //if (m_app.getPlayMgr().mediaOpaque().byteRate())
        //{
        //    m_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_Pause));
        if (m_app.getPlayMgr().player().Pause())
        {
            _updatePlayPauseButton(false);
        }
    }
    else if (button == ui.btnPlay)
    {
        /*if (E_PlayStatus::PS_Pause == m_app.getPlayMgr().playStatus())
        {
            _updatePlayPauseButton(true);
        }
        m_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_Play));*/

        if (E_PlayStatus::PS_Stop == m_app.getPlayMgr().playStatus())
        {
            m_app.getCtrl().callPlayCmd(tagPlayIndexCmd(m_app.getCtrl().getOption().uPlayingItem));
            return;
        }

        if (m_app.getPlayMgr().player().Resume())
        {
            _updatePlayPauseButton(true);
        }
    }
    else if (button == ui.btnPlayPrev)
    {
        m_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_PlayPrev));
    }
    else if (button == ui.btnPlayNext)
    {
        m_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_PlayNext));
    }
    else if (button == ui.btnRandom || button == ui.btnOrder)
    {
        auto& bRandomPlay = m_app.getOption().bRandomPlay;
        bRandomPlay = !bRandomPlay;

        ui.btnRandom->setVisible(bRandomPlay);
        ui.btnOrder->setVisible(!bRandomPlay);
    }
    else
    {
        _demand(button);
    }
}

void MainWindow::updateBkg()
{
    m_dxbkg = 0;
    m_dybkg = 0;

    _relayout();

    //update();
    this->repaint();
}

static const QString __qsCheck(QChar(L'√'));

void MainWindow::slot_labelClick(CLabel* label, const QPoint& pos)
{
    if (label == ui.labelSingerImg)
    {
        if (!m_bDefaultBkg)
        {
            if (m_PlayingInfo.bWholeTrack)
            {
                slot_labelClick(ui.labelPlayingfile, pos);
                return;
            }

            m_eSingerImgPos = E_SingerImgPos((int)m_eSingerImgPos+1);
            if (m_eSingerImgPos > E_SingerImgPos::SIP_Zoomout)
            {
                m_eSingerImgPos = E_SingerImgPos::SIP_Float;
            }

            _relayout();
        }
    }
    else if (label == ui.labelSingerName)
    {
        if (m_PlayingInfo.uSingerID != 0)// && !ui.labelSingerName->text().isEmpty())
        {
            CMediaSet *pMediaSet = m_app.getSingerMgr().GetSubSet(E_MediaSetType::MST_Singer, m_PlayingInfo.uSingerID);
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
            m_medialibDlg.showMediaRes(m_PlayingInfo.strPath);
        }
    }
    else if (label == ui.labelProgress)
    {
        cauto progressbar = *ui.progressbar;
        UINT uMax = progressbar.maximum();
        if (0 == uMax)
        {
            return; // 播放停止
        }

        UINT uSeekPos = uMax*pos.x()/progressbar.width();
        UINT uValue = progressbar.value();
        if (uSeekPos > uValue)
        {
            UINT uBuffer = progressbar.buffer();
            UINT uMaxBuffer = progressbar.maxBuffer();
            if (uBuffer < uMaxBuffer)
            {
                int nPlayablePos = (int)uMax*uBuffer/uMaxBuffer - __ReadStreamWaitTime;
                nPlayablePos = MAX(nPlayablePos, (int)uValue);
                uSeekPos = MIN(uSeekPos, (UINT)nPlayablePos);
            }
        }

        // TODO 增加按文件位置seek
        if (m_app.getPlayMgr().player().Seek(uSeekPos))
        {
            CApp::async(100, [&](){
              _updateProgress();
            });
        }
    }
    else
    {
        m_eDemandLanguage = E_LanguageType::LT_None;

        PairList<CLabel*, E_LanguageType> plLabels {
            {ui.labelDemandCN, E_LanguageType::LT_CN}
            , {ui.labelDemandHK, E_LanguageType::LT_HK}
            , {ui.labelDemandKR, E_LanguageType::LT_KR}
            , {ui.labelDemandJP, E_LanguageType::LT_JP}
            , {ui.labelDemandEN, E_LanguageType::LT_EN}
            , {ui.labelDemandEUR, E_LanguageType::LT_EUR}};
        plLabels([&](CLabel* lbl, E_LanguageType eLanguage) {
            if (lbl->text().startsWith(__qsCheck))
            {
                lbl->setText(lbl->text().mid(__qsCheck.length()));
            }
            else
            {
                if (lbl == label)
                {
                    m_eDemandLanguage = eLanguage;
                    lbl->setText(__qsCheck + lbl->text());
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

        static UINT uIdx = 0;
        uIdx++;
        if (uIdx >= __demandplayitemPngCount)
        {
            uIdx = 0;
        }
        WString strStyle;
        strStyle << L"border-image: url(:/img/medialib/demandplayitem" << uIdx << L".png)";

        ui.btnDemandPlayItem->setStyleSheet(strStyle);
    }
    else if (btnDemand == ui.btnDemandPlaylist)
    {
        eDemandMode = E_DemandMode::DM_DemandPlaylist;
    }
    else
    {
        return;
    }

    m_app.getCtrl().callPlayCmd(tagDemandCmd(eDemandMode, m_eDemandLanguage));
}

void MainWindow::drawDefaultBkg(CPainter& painter, cqrc rc, UINT xround, UINT yround)
{
    cauto pm = *ui.labelBkg->pixmap();
    QRect rcSrc = pm.rect();

    if (rc.height()>rc.width())
    {
        rcSrc.setRight(1080);
    }
    rcSrc.setTop(rcSrc.height()-rcSrc.width()*rc.height()/rc.width());

    painter.drawPixmap(rc, pm, rcSrc, xround, yround);
}

void MainWindow::_handleTouchEnd(const CTouchEvent& te)
{
    if (g_crLogoBkg.alpha() > 0)
    {
        return;
    }

    if (te.dt() < __fastTouchDt)
    {
        auto dx = te.dx();
        auto dy = te.dy();
        if (abs(dx) > abs(dy))
        {
            if (dx >= 3)
            {
                m_bkgDlg.switchBkg(m_bHLayout, false);
            }
            else if (dx <= -3)
            {
                m_bkgDlg.switchBkg(m_bHLayout, true);
            }
        }
        else
        {
            if (dy >= 3)
            {
                m_bkgDlg.switchBkg(m_bHLayout, false);
            }
            else if (dy <= -3)
            {
                m_bkgDlg.switchBkg(m_bHLayout, true);
            }
        }
    }
}

void MainWindow::handleTouchEvent(E_TouchEventType type, const CTouchEvent& te)
{
    static bool bTouchSingerImg = false;
    if (E_TouchEventType::TET_TouchBegin == type)
    {
        bTouchSingerImg = !m_bDefaultBkg && !ui.labelSingerImg->pixmap().isNull() && ui.labelSingerImg->geometry().contains(te.x(), te.y());
    }
    else if (E_TouchEventType::TET_TouchMove == type)
    {
        if (m_bDefaultBkg || m_app.getOption().bUseBkgColor)
        {
            return;
        }

        if (g_crLogoBkg.alpha() > 0)
        {
            return;
        }

        mtutil::yield();
        m_dxbkg -= te.dx();
        m_dybkg -= te.dy();
        update();
    }
    else if (E_TouchEventType::TET_TouchEnd == type)
    {
        if (bTouchSingerImg)
        {
            bTouchSingerImg = false;
            return;
        }

        _handleTouchEnd(te);
    }
}

#if __windows
static bool _cmdShell(const string& strCmd, bool bBlock = true)
{
    STARTUPINFOA si;
    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput=GetStdHandle(STD_INPUT_HANDLE);
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi;
    memset(&pi, 0, sizeof(pi));
    if(!CreateProcessA(NULL, (char*)strCmd.c_str(), NULL, NULL, FALSE
                      , CREATE_NO_WINDOW, NULL, NULL, &si, &pi))
    {
        return false;
    }

    if (bBlock)
    {
        WaitForSingleObject(pi.hProcess,INFINITE);
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return true;
}
#endif

bool MainWindow::installApp(const CByteBuffer& bbfData)
{
#if __android
    cauto strApkFile = fsutil::workDir() + "/upgrade.apk";
    if (!OFStream::writefilex(strApkFile, true, bbfData))
    {
        g_logger << "save appPackage fail: " >> strApkFile;
        return false;
    }

    installApk(QString::fromStdString(strApkFile));

#elif __mac
    cauto strUpgradeFile = fsutil::workDir() + "/upgrade.zip";
    if (!OFStream::writefilex(strUpgradeFile, true, bbfData))
    {
        g_logger << "save appPackage fail: " >> strUpgradeFile;
        return false;
    }

    cauto strUpgradeDir = fsutil::workDir() + "/XMusic.app";

#define system(x) system((x).c_str())
    (void)system("rm -rf " + strUpgradeDir);

    auto nRet = system("tar -xf " + strUpgradeFile);
    if (nRet)
    {
        g_logger << "unpack app fail: " >> nRet;
        return false;
    }
    (void)system("rm -f " + strUpgradeFile);

    auto strAppDir = QApplication::applicationDirPath().toStdString();
    strAppDir = fsutil::GetParentDir(strAppDir);
    strAppDir = fsutil::GetParentDir(strAppDir);
    strutil::replace(strAppDir, " ", "\\ ");

    cauto strBakDir = strAppDir + ".bak";
    (void)system("rm -rf " + strBakDir);

    nRet = system("mv " + strAppDir + " " + strBakDir);
    if (nRet)
    {
        g_logger << "backupApp fail: " >> nRet;
        return false;
    }

    nRet = system("mv " + strUpgradeDir + " " + strAppDir);
    if (nRet)
    {
        g_logger << "upgradeApp fail: " >> nRet;
        return false;
    }

    (void)system("rm -rf " + strBakDir);

    g_logger << "appUpgrade success, restart: " >> strAppDir;
    g_lf.unlock();

    nRet = system("open -n " + strAppDir);
    if (nRet)
    {
        g_logger << "restartApp fail: " >> nRet;
    }

#elif __windows
    IFBuffer ifbData(bbfData);
    CZipFile zipFile(ifbData);
    if (!zipFile)
    {
        g_logger >> "invalid appData";
        return false;
    }

    cauto strParentDir = fsutil::GetParentDir(fsutil::getModuleDir()) + __cPathSeparator;

    string strTempDir = strParentDir + "Upgrade";
    cauto strCmd = "cmd /C rd /S /Q \"" + strTempDir + "\"";
    if (!_cmdShell(strCmd))
    {
        g_logger << "cmdShell fail: " >> strCmd;
        return false;
    }

    if (!fsutil::createDir(strTempDir))
    {
        g_logger << "createDir fail: " >> strTempDir;
        return false;
    }

    strTempDir.push_back(__cPathSeparator);
    for (cauto unzdir : zipFile.unzdirList())
    {
        cauto strSubDir = strTempDir + unzdir.strPath;
        if (!fsutil::createDir(strSubDir))
        {
            g_logger << "createDir fail: " >> strSubDir;
            return false;
        }
    }

#define __StartupFile "XMusicStartup.exe"
    string strStartupFile;
    for (cauto pr : zipFile.unzfileMap())
    {
        cauto strSubFile = strTempDir + pr.first;
        if (strutil::endWith(strSubFile, __StartupFile))
        {
            strStartupFile = strSubFile;
        }

        if (zipFile.unzip(pr.second, strSubFile) < 0)
        {
            g_logger << "unzip fail: " >> strSubFile;
            return false;
        }
    }

    if (strStartupFile.empty())
    {
        g_logger >> "StartupFile not found";
        return false;
    }

    if (!fsutil::copyFile(strStartupFile, strParentDir + __StartupFile))
    {
        g_logger >> "copy StartupFile fail";
        return false;
    }

    if (!_cmdShell("\"" + strParentDir + __StartupFile "\" -upg", false))
    {
        g_logger >> "shell StartupFile fail";
    }
#endif
    return true;
}
