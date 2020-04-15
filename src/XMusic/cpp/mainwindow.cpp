
#include "widget.cpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMovie>

#include <QScreen>

#include "app.h"

#define __size10 __size(10)

#define __demandplayitemPngCount 4

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
    , m_PlayingList(app, this)
    , m_medialibDlg(*this, app)
    , m_bkgDlg(*this, app)
{
    ui.setupUi(this);

    m_PlayingList.raise();
    //m_PlayingList.setParent(ui.centralWidget);

    ui.labelBkg->setVisible(false);

    ui.btnPause->setVisible(false);

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
    fFontSizeOffset = 0.917;

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
                , ui.labelDemandJP, ui.labelDemandEN, ui.labelDemandEUR};
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

    //ui.labelSingerImg->setVisible(false);

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
    (void)m_pmHDDisk.load(__mediaPng(hddisk));
    (void)m_pmLLDisk.load(__mediaPng(lldisk));

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

        //this->setVisible(false);
    }

    break;
#endif
    case QEvent::Close:
        m_app.quit();

        break;
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

    UINT uShadowWidth = m_bUseDefaultBkg?0:1;
    ui.labelDemandHK->setShadow(uShadowWidth);
    ui.labelDemandCN->setShadow(uShadowWidth);
    ui.labelDemandKR->setShadow(uShadowWidth);
    ui.labelDemandJP->setShadow(uShadowWidth);
    ui.labelDemandEN->setShadow(uShadowWidth);
    ui.labelDemandEUR->setShadow(uShadowWidth);

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

            for (auto pWidget : SList<QWidget*>({ui.labelDuration, ui.progressbar, ui.labelProgress}))
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
        labelAlbumName.setFont(0.95);

        ui.labelAlbumName->setShadow(uShadowWidth);

        labelAlbumName.setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    }

    ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignBottom);

    m_PlayingList.setShadow(uShadowWidth);

    ui.labelPlayingfile->setShadow(uShadowWidth);

    ui.labelSingerImg->setPixmapRound(m_bUseDefaultBkg?__size(4):__szRound);

    //TODO auto pm = (int)m_PlayingInfo.eQuality>=(int)E_MediaQuality::MQ_CD?m_pmHDDisk:m_pmLLDisk;
    //ui.labelSingerImg->setPixmap(pm);

    E_SingerImgPos eSingerImgPos = SIP_Float;
    cauto pmSingerImg = ui.labelSingerImg->pixmap();

    if (!pmSingerImg.isNull())
    {
        eSingerImgPos = m_eSingerImgPos;

        ui.labelSingerImg->setShadow(2);

        ui.labelSingerName->setShadow(2);
    }
    else
    {
        ui.labelSingerName->setShadow(uShadowWidth);
    }

    auto& rcSingerImg = m_mapWidgetNewPos[ui.labelSingerImg];

    int y_PlayingListMax = 0;

    if (!m_bUseDefaultBkg)
    {
        int cx_progressbar = ui.progressbar->width();

        int cy_Playingfile = ui.labelPlayingfile->height();
        int y_Playingfile = ui.labelDuration->geometry().bottom() -  cy_Playingfile - __size(2);

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
        if (SIP_Zoomout == eSingerImgPos)
        {
            cy_SingerImg = 2*(y_Playingfile+cy_Playingfile - (y_labelAlbumName+__cylabelAlbumName/2));
            y_SingerImg = y_Playingfile+cy_Playingfile+1 - cy_SingerImg;

            cx_SingerImg = cy_SingerImg;

            ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);
            labelAlbumName.setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

            auto dx = cx_SingerImg + __size(25);
            x += dx;

            labelAlbumName.setGeometry(x, y_labelAlbumName, cx_progressbar-dx, __cylabelAlbumName);
            ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x()-x, cy_Playingfile);

            ui.labelSingerName->setGeometry(x, y_SingerImg, cx_progressbar-dx, ui.labelSingerName->height());
            y_PlayingListMax = y_SingerImg;
        }
        else
        {
            if (SIP_Float == eSingerImgPos)
            {
                if (m_bHScreen)
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
                    cx_SingerImg = MIN(cx_SingerImg, cy_SingerImg*1.25);
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
            ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x()-x, cy_Playingfile);

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

        rcSingerImg.setRect(x_SingerImg, y_SingerImg, cx_SingerImg, cy_SingerImg);
        ui.labelSingerImg->setGeometry(rcSingerImg);
    }
    else
    {
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
        int x_PlayingList = ui.progressbar->geometry().right() + xOffset;

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

            if (pmSingerImg.isNull() || SIP_Zoomout == eSingerImgPos)
            {
                y_Margin += __size10*2;
            }
            else if (SIP_Dock == eSingerImgPos)
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
       painter.drawPixmapEx(rc, pmBkg, m_dxbkg, m_dybkg);
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

    int nProgress = int(playMgr.mediaOpaque().clock()/__1e6);
    if (nProgress <= ui.progressbar->maximum())
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
            QString qsDuration = strutil::toQstr(CMedia::genDurationString(m_PlayingInfo.uDuration));
            ui.labelDuration->setText(qsDuration);
        }
#endif
        ui.progressbar->setValue(nProgress, bufferValue);
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
    PlayingInfo.qsTitle = strutil::toQstr(PlayItem.GetTitle());

    PlayingInfo.strPath = PlayItem.GetPath();

    PlayingInfo.uDuration = PlayItem.duration();
    /*if (PlayingInfo.uDuration > __wholeTrackDuration)
    {
        CMediaRes *pMediaRes = __medialib.findSubFile(PlayingInfo.strPath);
        if (pMediaRes && pMediaRes->parent()->dirType() == E_MediaDirType::MDT_Snapshot)
        {
            PlayingInfo.bWholeTrack = true;
        }
    }*/

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

    if (m_app.getPlayMgr().mediaOpaque().isOnline())
    {
        PlayingInfo.eQuality = PlayItem.quality();
        PlayingInfo.qsQuality = mediaQualityString(PlayingInfo.eQuality);
    }

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

    QVariant var;
    var.setValue(PlayingInfo);

    emit signal_showPlaying(uPlayingItem, bManual, var);
}

void MainWindow::slot_showPlaying(unsigned int uPlayingItem, bool bManual, QVariant var)
{
    m_uPlaySeq++;

    auto strPrevSinger = m_PlayingInfo.strSingerName;
    m_PlayingInfo = var.value<tagPlayingInfo>();

    ui.labelPlayingfile->setText(m_PlayingInfo.qsTitle);

    ui.progressbar->setValue(0);
    ui.progressbar->setMaximum(m_PlayingInfo.uDuration, m_PlayingInfo.uStreamSize);

#if __OnlineMediaLib
    ui.labelDuration->clear();
#else
    cauto qsDuration = strutil::toQstr(CMedia::genDurationString(m_PlayingInfo.uDuration));
    ui.labelDuration->setText(qsDuration);
#endif

    _updatePlayPauseButton(true);

    m_PlayingList.updatePlayingItem(uPlayingItem, bManual);

    ui.labelSingerName->setText(strutil::toQstr(m_PlayingInfo.strSingerName));

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
    ui.progressbar->setMaximum(0);

    QString qsDuration = strutil::toQstr(CMedia::genDurationString(m_PlayingInfo.uDuration));
    ui.labelDuration->setText(qsDuration);

    (void)bOpenFail;
    /*if (bOpenFail)
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
    }*/
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

    return strMediaSet;
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

    WString strSingerImg = m_app.getSingerImgMgr().getSingerImg(m_PlayingInfo.strSingerName, uSingerImgIdx);
    if (!strSingerImg->empty())
    {
        QPixmap pm;
        if (pm.load(strSingerImg))
        {
            ui.labelSingerImg->setPixmap(pm);

            //if (!ui.labelSingerImg->isVisible()) ui.labelSingerImg->setVisible(true);

            _relayout();
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
        //if (m_app.getPlayMgr().mediaOpaque().byteRate())
        //{
        //    m_app.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Pause));
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
        m_app.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Play));*/

        if (E_PlayStatus::PS_Stop == m_app.getPlayMgr().playStatus())
        {
            m_app.getCtrl().callPlayCtrl(tagPlayCtrl(m_app.getCtrl().getOption().uPlayingItem));
            return;
        }

        if (m_app.getPlayMgr().player().Resume())
        {
            _updatePlayPauseButton(true);
        }
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
    m_dxbkg = 0;
    m_dybkg = 0;

    _relayout();

    update();
}

void MainWindow::handleTouchMove(const CTouchEvent& te)
{
    m_dxbkg -= te.dx();
    m_dybkg -= te.dy();
    update();
}

static const QString __qsCheck(QChar(L'√'));

void MainWindow::slot_labelClick(CLabel* label, const QPoint& pos)
{
    if (label == ui.labelSingerImg)
    {
        if (!m_bUseDefaultBkg)
        {
            m_eSingerImgPos = E_SingerImgPos((int)m_eSingerImgPos+1);
            if (m_eSingerImgPos > SIP_Zoomout)
            {
                m_eSingerImgPos = SIP_Float;
            }

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
        /*if (m_app.getPlayMgr().playStatus() != E_PlayStatus::PS_Play)
        {
            return;
        }
        if (!m_app.getPlayMgr().mediaOpaque().byteRate())
        {
            return;
        }*/

        cauto progressbar = *ui.progressbar;
        auto nMax = progressbar.maximum();
        if (nMax <= 0)
        {
            return;
        }

        auto nSeekPos = pos.x() * nMax / progressbar.width();
        auto nCurrent = progressbar.value();
        if (nSeekPos > nCurrent && progressbar.bufferValue() < progressbar.maxBuffer())
        {
            int nPlayablePos = nMax*progressbar.bufferValue()/progressbar.maxBuffer() - __ReadStreamWaitTime;
            nPlayablePos = MAX(nPlayablePos, nCurrent);
            nSeekPos = MIN(nSeekPos, nPlayablePos);
        }

        if (m_app.getPlayMgr().player().Seek(nSeekPos))
        {
            __appAsync(100, [&](){
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
