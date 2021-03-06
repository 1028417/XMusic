
#include "xmusic.h"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMovie>

#define __demandplayitemPngCount 4

Ui::MainWindow ui;

bool g_bHLayout = false;

QColor g_crLogoBkg(180, 220, 255);

#if __windows
static void _fixWorkArea(QWidget& wnd)
{
    const RECT& rcWorkArea = getWorkArea(g_bFullScreen);

    auto cx = rcWorkArea.right-rcWorkArea.left+1;
    auto cy = rcWorkArea.bottom-rcWorkArea.top+1;
    wnd.setGeometry(rcWorkArea.left, rcWorkArea.top, cx, cy);
    ::MoveWindow((HWND)wnd.winId(), rcWorkArea.left, rcWorkArea.top, cx, cy, TRUE);
}

#elif __android
inline static void _androidFullScreen()
{
    /*//java构造函数全屏效果更好，所以统一用jni控制
    fullScreen(g_bFullScreen);

    if (!g_bFullScreen)
    {
        showTransparentStatusBar(true); // 不全屏就显示安卓透明状态栏（半沉浸）
    }*/

    fullScreenex(g_bFullScreen);// 合并成一次调用
}
#endif

static void _fixScreen(QWidget& wnd)
{
#if __android
    //wnd.setWindowState((g_bFullScreen?Qt::WindowFullScreen:Qt::WindowMaximized));// | Qt::WindowActive);

    wnd.setWindowState(Qt::WindowMaximized);// | Qt::WindowActive);

    return;
#endif

    if (__ios || g_bFullScreen)
    {
        wnd.setWindowState(Qt::WindowFullScreen | Qt::WindowActive); // Mac需要WindowActive
    }
    else
    {
        wnd.setWindowState(Qt::WindowMaximized);// | Qt::WindowActive);
    }

#if __windows
    _fixWorkArea(wnd);
#endif
}

void MainWindow::switchFullScreen()
{
#if __ios
    return;
#endif

    g_bFullScreen = !g_bFullScreen;
    m_opt.bFullScreen = g_bFullScreen;

#if __windows
    if (g_bFullScreen)
    {
        ::SetWindowPos(hwnd(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    }
#endif

    _fixScreen(*this);

#if __android
    _androidFullScreen();

    //async([&]{
        _relayout();
    //});
#endif

#if __windows
    if (g_bFullScreen)
    {
        ::SetWindowPos(hwnd(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE);
    }
#endif
}

MainWindow::MainWindow() :
    QMainWindow(NULL, Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint)
    , m_opt(g_app.getOption())
{
    //this->setStyleSheet("");

    //qRegisterMetaType<QVariant>("QVariant");
}

void MainWindow::exec()
{
#if __android
    _androidFullScreen();
#else
    _fixScreen(*this);
    this->setVisible(true); //必须在前面？？不然ole异常？？
#endif
}

void MainWindow::preinit(XThread& thr) // showlogo之后工作线程调用
{
    QPixmap pmBkg(":/img/bkg.jpg");
    m_brBkg.setTexture(pmBkg.copy(0, 0, 10, pmBkg.height()));

#define __cyCDCover 825
    auto rc = pmBkg.rect();
    rc.setTop(rc.bottom() - __cyCDCover);
    QPixmap&& pm = pmBkg.copy(rc);
    m_pmCDCover.swap(pm);

    mtutil::concurrence([&]{
        m_bkgDlg.preinitBkg(thr, true);
    }, [&]{
        m_bkgDlg.preinitBkg(thr, false);
    });

    while (m_uShowLogoState < 3)
    {
        if (!thr.usleep(100))
        {
            break;
        }
    }
}

#define __cxLoginTip __size(600)

void MainWindow::_ctor()
{
#if !__android
    m_labelLoginTip.resize(__cxLoginTip, __size(60));
    m_labelLoginTip.setFont(1.03f, TD_FontWeight::Normal);
#endif

    ui.setupUi(this);

    ui.centralWidget->ctor();

    this->regUISlot(&MainWindow::slot_buttonClicked, &CButton::signal_clicked, {
                        ui.btnFullScreen, ui.btnExit, ui.btnMore
                        , ui.btnDemandSinger, ui.btnDemandAlbum, ui.btnDemandAlbumItem
                        , ui.btnDemandPlayItem, ui.btnDemandPlaylist
                        , ui.btnSetting, ui.btnOrder, ui.btnRandom
                        , ui.btnPause, ui.btnPlay, ui.btnPlayPrev, ui.btnPlayNext
                    });

    for (auto widget : list<QWidget*>({ ui.labelLogo, ui.labelLogoTip, ui.labelLogoCompany
                                      , ui.centralWidget, ui.frameDemand, ui.frameDemandLanguage}))
    {
        widget->setAttribute(Qt::WA_TranslucentBackground);
    }

    ui.btnFullScreen->setParent(this);
    ui.btnFullScreen->setVisible(true);

#if __android || __ios
    int nLogoWidth = g_screen.nMinSide*42/100;
    ui.labelLogo->resize(nLogoWidth, nLogoWidth/4);
    ui.labelLogo->setScaledContents(true);

    ui.btnExit->setVisible(false);
#else
    ui.btnExit->setParent(this);
    ui.btnExit->setVisible(true);
#endif

    ui.centralWidget->setVisible(false);
    //m_PlayingList.setVisible(false);
}

void MainWindow::_init()
{
    _ctor();

    SList<CLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR
                , ui.labelDemandJP, ui.labelDemandEN, ui.labelDemandOther};
    for (auto label : lstLabels)
    {
        label->setFont(1.03f, TD_FontWeight::DemiBold);
    }
    lstLabels.add(ui.labelSingerImg, ui.labelSingerName, ui.labelAlbumName, ui.labelPlayingfile);
    this->regUISlot(&MainWindow::slot_labelClick, &CLabel::signal_clicked, lstLabels);

    ui.labelProgress->onTouch(this, &MainWindow::slot_progressTouch);

    m_PlayingList.setFont(0.9f);

    ui.labelSingerName->setFont(0.95f);
    ui.labelAlbumName->setFont(0.95f);
    ui.labelPlayingfile->setFont(0.95f);

    ui.labelDuration->setFont(0.8f);

    ui.btnPause->setVisible(false);

    if (m_opt.bRandomPlay)
    {
        ui.btnRandom->setVisible(true);
        ui.btnOrder->setVisible(false);
    }
    else
    {
        ui.btnRandom->setVisible(false);
        ui.btnOrder->setVisible(true);
    }

    m_medialibDlg.init();
    m_bkgDlg.init();
}

void MainWindow::showLogo() // TODO 广告
{
/*#if __android
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0)) // Qt5.7以上
    async([]{
        hideSplashScreen(); //启动页延时关闭防止闪烁
    });
#endif
#endif*/

    _init();

    auto movie = new QMovie(this);
    movie->setFileName(":/img/logo.gif");
    ui.labelLogo->setMovie(movie);
    ui.labelLogo->movie()->start();
    ui.labelLogo->setParent(this);
    ui.labelLogo->setVisible(true);

#if __android
    _fixScreen(*this);
    this->setVisible(true);
#else
    _relayout();
#endif
}

void MainWindow::startLogo()
{
    float fFontSizeOffset = 1.072f;
#if __android || __ios
    fFontSizeOffset = 0.918f;
#endif
    CFont font(fFontSizeOffset);
    ui.labelLogoCompany->setFont(font);
    font.setItalic(true);
    ui.labelLogoTip->setFont(font);

    CDialog::setWidgetTextColor(ui.labelLogoTip, QColor(__crLogoText));
    CDialog::setWidgetTextColor(ui.labelLogoCompany, QColor(__crLogoText, 0));

    auto& labelLogoTip = *ui.labelLogoTip;
    labelLogoTip.setParent(this);
    labelLogoTip.setVisible(true);

    labelLogoTip.setText("播放器");

    async(500, [&]{
        labelLogoTip.setText(labelLogoTip.text() + WString(__CNDot L"媒体库"));
        async(500, [&]{
            labelLogoTip.setText(labelLogoTip.text() + "  个性化定制");
            async(2000, [&]{
                _showUpgradeProgress();
            });
        });
    });

    auto& labelLogoCompany = *ui.labelLogoCompany;
    labelLogoCompany.setParent(this);
    labelLogoCompany.setVisible(true);
    _updateLogoCompany(5, [&]{
        _updateLogoCompany(-5, [&]{
            labelLogoCompany.setText(__WS2Q(L"v" + g_app.appVer()));
            _updateLogoCompany(5, [&]{
                m_uShowLogoState++;
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

    timerutil::setTimerEx(25, [=]()mutable{
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

#define __logoTip "更新媒体库"

void MainWindow::_showUpgradeProgress()
{
    cauto nAppUpgradeProgress = g_app.getMdlMgr().appUpgradeProgress();
    if (-1 == nAppUpgradeProgress)
    {
        ui.labelLogoTip->setText(__logoTip);
    }

    UINT uDotCount = 0;
    timerutil::setTimerEx(200, [&, uDotCount]()mutable{
        if (m_bStopLogo)
        {
            return false;
        }

        if (nAppUpgradeProgress >= 0)
        {
            QString qsText;
            if (100 == nAppUpgradeProgress)
            {
                qsText.append("准备安装...");
            }
            else
            {
                //if (0 == g_nAppUpgradeProgress) qsText.append("下载升级包..."); else
                qsText.sprintf("下载升级包:  %u%%", (UINT)nAppUpgradeProgress);
            }
            ui.labelLogoTip->setText(qsText);
        }
        else
        {
            if (++uDotCount > 3)
            {
                uDotCount = 0;
                m_uShowLogoState++;
            }

            QString qsText(__logoTip);
            for (UINT uIdx = 1; uIdx <= 3; uIdx++)
            {
                if (uDotCount >= uIdx)
                {
                    qsText.append(__wcDot);
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

void MainWindow::stopLogo()
{
    m_bStopLogo = true;

    ui.labelLogoTip->setText(WString(L"播发器" __CNDot L"媒体库 个性化定制"));
}

void MainWindow::show()
{
    m_bStopLogo = true;

    ui.labelLogo->movie()->stop();
    delete ui.labelLogo->movie();
    ui.labelLogo->clear();
    ui.labelLogo->setVisible(false);

    ui.labelLogoTip->setVisible(false);

    ui.labelLogoCompany->setVisible(false);

    ui.centralWidget->setVisible(true);

    m_PlayingList.setParent(this);
    m_PlayingList.setVisible(true);
    m_PlayingList.raise();
    ui.btnFullScreen->raise();
    ui.btnExit->raise();

    m_PlayingList.init();

    updateDemandButton();

    ui.labelDemandCN->setEnabled(_checkDemandLanguage(E_LanguageType::LT_CN));
    ui.labelDemandHK->setEnabled(_checkDemandLanguage(E_LanguageType::LT_HK));
    ui.labelDemandKR->setEnabled(_checkDemandLanguage(E_LanguageType::LT_KR));
    ui.labelDemandJP->setEnabled(_checkDemandLanguage(E_LanguageType::LT_JP));
    ui.labelDemandEN->setEnabled(_checkDemandLanguage(E_LanguageType::LT_EN));
    ui.labelDemandOther->setEnabled(_checkDemandLanguage(E_LanguageType::LT_Other));

    _relayout();

    auto nLogoBkgAlpha = g_crLogoBkg.alpha();
    UINT uOffset = 23;
#if __windows || __mac
    uOffset = 20;
    if (std::thread::hardware_concurrency() > 4)
    {
        uOffset = 1;
    }
#endif
    UINT uDelayTime = m_opt.bUseBkgColor?50:30;
    timerutil::setTimerEx(uDelayTime, [=]()mutable{
        uOffset+=1;
        nLogoBkgAlpha -= uOffset;
        if (nLogoBkgAlpha <= 0)
        {
            g_crLogoBkg.setAlpha(0);
            update();

            (void)startTimer(1000);

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
            return false;
        }

        g_crLogoBkg.setAlpha(nLogoBkgAlpha);
        update();
        return true;
    });
}

void MainWindow::showLoginLabel(cwstr strUser)
{
    cauto qsTip = "登录成功！Hi~ " + __WS2Q(strUser);
#if __android
    vibrate();
    showToast(qsTip, true);

#else
    QWidget *parent = dynamic_cast<CDialog*>(CDialog::frontDlg());
    if (NULL == parent)
    {
        parent = this;
    }
    m_labelLoginTip.show(*parent, qsTip, 3000);
#endif
}

void MainWindow::quit(cfn_void cb)
{
    int nAlpha = 255;
    UINT uOffset = 23;
#if __windows || __mac
    uOffset = 20;
    if (std::thread::hardware_concurrency() > 4)
    {
        uOffset = 1;
    }
#endif
    UINT uDelayTime = m_opt.bUseBkgColor?50:30;
    timerutil::setTimerEx(uDelayTime, [=]()mutable{
        uOffset+=1;
        nAlpha -= uOffset;
        if (nAlpha <= 0)
        {
            this->setVisible(false);
            cb();
            return false;
        }

        this->setWindowOpacity(nAlpha/255.0f);
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
#if __windows
            _fixWorkArea(*this);
#endif
            _relayout();

            CDialog::resetPos();
        }

        break;
    case QEvent::Paint:
        _onPaint();

        break;
    case QEvent::MouseButtonDblClick:
        if (!m_PlayingList.isVisible() || !m_PlayingList.geometry().contains(((QMouseEvent*)ev)->pos()))
        {
            switchFullScreen();
        }

        break;
    case QEvent::KeyRelease:
    {
#if __android || __ios
#if __android
        if (Qt::Key_Back != ((QKeyEvent*)ev)->key())
        {
            break;
        }
#endif

        if (NULL == ui.centralWidget || !ui.centralWidget->isVisible())
        {
            break;
        }

        static time_t prevTime = 0;
        time_t currTime = time(0);
        if (currTime - prevTime > 2)
        {
#if __android
            vibrate();
            showToast("再按一次退出程序", false);
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
        /*else if (Qt::Key_Escape == key)
        {
#if __mac
            this->setWindowState(Qt::WindowMaximized | Qt::WindowActive); // Mac最小化不了，执行还原
#else
            this->setWindowState(Qt::WindowMinimized); // Qt5.13.2 Windows弹窗后也有bug
#endif
        }*/
        else if (m_PlayingList.isVisible())
        {
            // TODO 上下键滚动播放列表、左右键切换背景
            if (Qt::Key_Space == key)
            {
                if (E_PlayStatus::PS_Play == g_app.getPlayMgr().playStatus())
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
    /*case QEvent::Close:
        g_app.quit();
        return true;
        break;*/
    case QEvent::Timer:
        _updateProgress();

        if (m_PlayingInfo.pSinger)
        {
            _playSingerImg(m_PlayingInfo.pSinger->m_uID, false);
        }

        break;
    default:
        break;
    }

    return QMainWindow::event(ev);
}

void MainWindow::_relayout()
{
    int cx = width();
    int cy = height();
    g_bHLayout = cx > cy; // 橫屏

#if !__android
    m_labelLoginTip.move((cx - __cxLoginTip)/2, cy - __size(300));
#endif

    if (NULL == ui.centralWidget) return;

    m_bDefaultBkg = false;
    if (!m_opt.bUseBkgColor)
    {
        cauto pmBkg = g_bHLayout?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
        m_bDefaultBkg = pmBkg.isNull();
    }

    auto eSingerImgPos = (E_SingerImgPos)(g_bHLayout?m_opt.uHSingerImgPos:m_opt.uVSingerImgPos);
    ui.centralWidget->relayout(cx, cy, m_bDefaultBkg, eSingerImgPos, m_PlayingInfo, m_PlayingList);
}

void MainWindow::_onPaint()
{
    CPainter painter(*this);
    cauto rc = this->rect();

    auto nLogoAlpha = g_crLogoBkg.alpha();
    auto nBkgAlpha = 255-nLogoAlpha;
    if (nBkgAlpha > 0)
    {
        if (m_opt.bUseBkgColor)
        {
            painter.fillRect(rc, g_crBkg);
        }
        else
        {
            painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

            /*cauto pmBkg = g_bHLayout ?m_bkgDlg.hbkg() :m_bkgDlg.vbkg();
            if (!pmBkg.isNull())
            {
                auto& prBkgOffset = g_bHLayout?m_opt.prHBkgOffset:m_opt.prVBkgOffset;
                painter.drawImgEx(rc, pmBkg, prBkgOffset.first, prBkgOffset.second);

                //auto cx = ui.progressbar->width();
                //auto cy = cx * m_pmDiskFace.height()/m_pmDiskFace.width();
                //cauto rcSingerImg = m_mapWidgetNewPos[ui.labelSingerImg];
                //QRect rcDst(rcSingerImg.x(), rcSingerImg.y(), cx, cy);
                //painter.drawImg(rcDst, m_pmDiskFace);
            }*/
            if (!drawBkg(g_bHLayout, painter, rc))
            {
                drawDefaultBkg(painter, rc, 0, ui.labelSingerImg->pixmap()?1.0f:0.06f);
            }
        }
    }

    if (nLogoAlpha > 0)
    {
        painter.fillRect(rc, g_crLogoBkg);
    }
}

bool MainWindow::drawBkg(bool bHLayout, CPainter& painter, cqrc rc) const
{
    if (m_opt.bUseBkgColor)
    {
        return false;
    }

    cauto pmBkg = bHLayout ?m_bkgDlg.hbkg() :m_bkgDlg.vbkg();
    if (pmBkg.isNull())
    {
        return false;
    }

    auto& prBkgOffset = bHLayout?m_opt.prHBkgOffset:m_opt.prVBkgOffset;
    painter.drawImgEx(rc, pmBkg, prBkgOffset.first, prBkgOffset.second);
    return true;
}

void MainWindow::drawDefaultBkg(CPainter& painter, cqrc rc, UINT szRound, float fCDCover)
{
    Double_T cxDst = rc.width();
    int cyDst = rc.height();
    Double_T xDst = 0;
    auto fBkgZoomRate = ui.centralWidget->caleBkgZoomRate(cxDst, cyDst, xDst);

    auto cySrc = cyDst/fBkgZoomRate;
    QRect rcSrc(0, round(__cyBkg-cySrc), 10, round(cySrc));
    painter.drawBrush(rc, m_brBkg, rcSrc, szRound);

    painter.setOpacity(fCDCover);

    auto cyCDCover = m_pmCDCover.height()*fBkgZoomRate;
    QRect rcCDCover(round(rc.x()+xDst), round(rc.y()+cyDst-cyCDCover), round(cxDst), round(cyCDCover));
    painter.drawImg(rcCDCover, m_pmCDCover);

    painter.setOpacity(1);
}

void MainWindow::_updateProgress()
{
    auto& mediaOpaque = g_app.getPlayMgr().mediaOpaque();
    E_DecodeStatus eDecodeStatus = mediaOpaque.decodeStatus();
    _updatePlayPauseButton(E_DecodeStatus::DS_Decoding == eDecodeStatus); // for see
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
    if (mediaOpaque.waitingFlag() && g_app.getPlayMgr().player().packetQueueEmpty())
    {
        ui.labelDuration->setText("正在缓冲");
    }
    else
    {
        ui.labelDuration->setText(m_PlayingInfo.qsDuration);
    }

    auto uReadableSize = mediaOpaque.readableSize();
    if (uReadableSize > 0)
    {
        ui.progressbar->setBuffer(UINT(uReadableSize/1000), m_PlayingInfo.uFileSize);
    }
#endif

    UINT uProgress = UINT(mediaOpaque.clock()/__1e6);
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
    m_PlayingList.updateList(nPlayingItem);
}

extern UINT g_uPlaySeq;
static UINT m_uPlaySeq = 0;

void MainWindow::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, CMedia *pMedia, CMediaRes *pMediaRes, bool bManual)
{
    m_uPlaySeq = g_uPlaySeq;

    tagPlayingInfo PlayingInfo;
    cauto strPath = PlayingInfo.strPath = PlayItem.GetPath();

#if __OnlineMediaLib
    PlayingInfo.uFileSize = PlayItem.fileSize()/1000;
#endif

    auto uDuration = PlayItem.duration();
    PlayingInfo.qsDuration = __WS2Q(IMedia::genDurationString(uDuration));

    E_TrackType eTrackType = E_TrackType::TT_Single;
    if (pMedia)
    {
        PlayingInfo.pIMedia = pMedia;

        auto pMediaSet = pMedia->mediaSet();
        PlayingInfo.strMediaSet = pMediaSet->m_strName;

        if (E_MediaSetType::MST_Album == pMediaSet->type())
        {
            PlayingInfo.pSinger = (CSinger*)pMediaSet->m_pParent;
        }

        PlayingInfo.pXpkMediaRes = __xmedialib.getXpkMediaRes(L"", *pMedia);
    }
    else if (pMediaRes)
    {
        PlayingInfo.pIMedia = pMediaRes;

        auto pSnapshotDir = _snapshotDir(*pMediaRes);
        if (pSnapshotDir)
        {
            PlayingInfo.pSinger = pSnapshotDir->singer();
            if (PlayingInfo.pSinger)
            {
                PlayingInfo.strMediaSet = m_medialibDlg.genAttachTitle(*pSnapshotDir);
            }
            eTrackType = ((CSnapshotMedia*)pMediaRes)->trackType();

            PlayingInfo.pXpkMediaRes = __xmedialib.getXpkMediaRes(pSnapshotDir->catName(), *pMediaRes);
        }
    }
    else
    {
        PlayingInfo.pXpkMediaRes = __xmedialib.xpkRoot().subFile(strPath);
    }

    bool bAbsPath = PlayItem.isAbsPath();

    cauto strTitle = PlayItem.GetTitle();
    cauto qsTitle = bAbsPath ? __WS2Q(strTitle) : CFileTitle::genDisplayTitle_r(
                                   strTitle, PlayingInfo.pSinger);

    auto eQuality = PlayItem.quality();
    //if (g_app.getPlayMgr().mediaOpaque().isVideo()) // 获取本地视频文件音频流码率

    g_app.sync([=]{
        m_PlayingList.updatePlayingItem(uPlayingItem, bManual, qsTitle, uDuration, eQuality);
        ui.labelPlayingfile->setText(qsTitle);

        _onPlay(PlayingInfo, uDuration, eTrackType);
    });
}

void MainWindow::_onPlay(const tagPlayingInfo& PlayingInfo, UINT uDuration, E_TrackType eTrackType)
{
    bool bSingerChanged = (PlayingInfo.pSinger != m_PlayingInfo.pSinger);
    m_PlayingInfo = PlayingInfo;

    if (E_TrackType::TT_Single != eTrackType)
    {
        cauto pmIcon = E_TrackType::TT_HDWhole == eTrackType? g_app.m_pmHDDisk:g_app.m_pmSQDisk;
        ui.labelPlayingfile->setPixmap(pmIcon);
    }
    else if (m_PlayingInfo.pXpkMediaRes)
    {
        ui.labelPlayingfile->setPixmap(m_medialibDlg.pmXpk());
    }
    else
    {
        ui.labelPlayingfile->clearPixmap();
    }

    ui.progressbar->setValue(0, uDuration);

/*#if __OnlineMediaLib
    ui.labelDuration->clear();
#else*/
    ui.labelDuration->setText(m_PlayingInfo.qsDuration);
//#endif

    _updatePlayPauseButton(true);

    if (bSingerChanged)
    {
        ui.labelSingerImg->clear();
        update();

        if (m_PlayingInfo.pSinger)
        {
            _playSingerImg(m_PlayingInfo.pSinger->m_uID, true);

            ui.labelSingerName->setText(__WS2Q(m_PlayingInfo.pSinger->m_strName));
        }
        else
        {
            ui.labelSingerName->clear();
        }
    }

    _relayout();
}

void MainWindow::onPlayStop(bool bOpenSuccess, bool bPlayFinish)
{
    g_app.sync([=]{
        ui.progressbar->set(0, 0, 0, 0);
        ui.labelDuration->clear();
/*#if __OnlineMediaLib
        ui.labelDuration->setText(m_PlayingInfo.qsDuration); //防止还显示正在缓冲
#endif*/
    });

    if (m_uPlaySeq != g_uPlaySeq)
    {
        return;
    }

    if (bPlayFinish)
    {
        // sync或async安卓切后台响应不来
        g_app.getCtrl().callPlayCmd(E_PlayCmd::PC_AutoPlayNext);
    }
    else if (!bOpenSuccess)
    {
        __usleep(2000);
        if (m_uPlaySeq != g_uPlaySeq)
        {
            return;
        }
        g_app.getCtrl().callPlayCmd(E_PlayCmd::PC_AutoPlayNext);
    }
}

void MainWindow::onSingerImgDownloaded(CSinger& singer, const tagSingerImg& singerImg)
{
    if (m_medialibDlg.isVisible())
    {
        g_app.sync([&]{
            if (m_medialibDlg.isVisible())
            {
                m_medialibDlg.updateSingerImg(singer, singerImg);
            }
        });
    }

    if (singerImg.isSmall() || singerImg.isPiiic())
    {
        return;
    }

    if (m_PlayingInfo.pSinger == &singer && !ui.labelSingerImg->pixmap())
    {
        g_app.sync([=]{
            if (m_PlayingInfo.pSinger == &singer)
            {
             _playSingerImg(singer.m_uID);
            }
        });
    }
}

bool MainWindow::installApp(const CByteBuffer& bbfUpgradeFile)
{
    return ::installApp(bbfUpgradeFile);
}

bool MainWindow::installApp(const string& strUpgradeFile)
{
    return ::installApp(strUpgradeFile);
}

#define ___singerImgElapse 8

void MainWindow::_playSingerImg(UINT uSingerID, bool bReset)
{
    static UINT uTickCount = 0;
    if (bReset)
    {
        uTickCount = 0;
        m_uSingerImgIdx = 0;
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

    _playSingerImg(uSingerID);
}

void MainWindow::_playSingerImg(UINT uSingerID)
{
    auto pSingerImg = g_app.getSingerImgMgr().getSingerImg(uSingerID, m_uSingerImgIdx, true);
    if (NULL == pSingerImg)
    {
        if (m_uSingerImgIdx > 1)
        {
            _playSingerImg(uSingerID, true);
        }
        return;
    }

    if (!pSingerImg->bExist)
    {
        return;
    }

    cauto qsFile = __WS2Q(g_app.getSingerImgMgr().file(*pSingerImg));
    QPixmap pm(qsFile);

#define __szSingerImg 700
    auto cx = pm.width();
    auto cy = pm.height();
    if (cx > cy)
    {
        if (cy > __szSingerImg)
        {
            QPixmap&& temp = pm.scaledToHeight(__szSingerImg, Qt::SmoothTransformation);
            pm.swap(temp);
        }

        auto cxMax = cy * __hPiiicRate;
        if (cx > cxMax)
        {
            QPixmap&& temp = pm.copy((cx-cxMax)/2, 0, cxMax, cy);
            pm.swap(temp);
        }
    }
    else
    {
        if (cx > __szSingerImg)
        {
            QPixmap&& temp = pm.scaledToWidth(__szSingerImg, Qt::SmoothTransformation);
            pm.swap(temp);
        }

        auto cyMax = cx * __vPiiicRate;
        if (cy > cyMax)
        {
            QPixmap&& temp = pm.copy(0, (cy-cyMax)/2, cx, cyMax);
            pm.swap(temp);
        }
    }

    ui.labelSingerImg->setPixmap(pm);
    update();
    //if (!ui.labelSingerImg->isVisible()) ui.labelSingerImg->setVisible(true);

    _relayout();

#if __android
    if (!g_bAndroidSDPermission)
    {
        return;
    }
#endif

    m_uSingerImgIdx++;
    (void) g_app.getSingerImgMgr().getSingerImg(uSingerID, m_uSingerImgIdx, true);
}

void MainWindow::slot_buttonClicked(CButton* button)
{
    if (button == ui.btnFullScreen)
    {
        switchFullScreen();
    }
    else if (button == ui.btnExit)
    {
        g_app.quit();
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
        //if (g_app.getPlayMgr().mediaOpaque().byteRate())
        //{
        //    g_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_Pause));
        if (g_app.getPlayMgr().player().Pause())
        {
            _updatePlayPauseButton(false);
        }
    }
    else if (button == ui.btnPlay)
    {
        /*if (E_PlayStatus::PS_Pause == g_app.getPlayMgr().playStatus())
        {
            _updatePlayPauseButton(true);
        }
        g_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_Play));*/

        if (E_PlayStatus::PS_Stop == g_app.getPlayMgr().playStatus())
        {
            g_app.getCtrl().callPlayCmd(tagPlayIndexCmd(m_opt.uPlayingItem));
            return;
        }

        if (g_app.getPlayMgr().player().Resume())
        {
            _updatePlayPauseButton(true);
        }
    }
    else if (button == ui.btnPlayPrev)
    {
        g_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_PlayPrev));
    }
    else if (button == ui.btnPlayNext)
    {
        g_app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_PlayNext));
    }
    else if (button == ui.btnRandom || button == ui.btnOrder)
    {
        auto& bRandomPlay = m_opt.bRandomPlay;
        bRandomPlay = !bRandomPlay;

        ui.btnRandom->setVisible(bRandomPlay);
        ui.btnOrder->setVisible(!bRandomPlay);
    }
    else
    {
        _demand(button);
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

    g_app.getCtrl().callPlayCmd(tagDemandCmd(eDemandMode, m_eDemandLanguage));
}

void MainWindow::updateBkg()
{
    auto& prBkgOffset = g_bHLayout?m_opt.prHBkgOffset:m_opt.prVBkgOffset;
    prBkgOffset.first = 0;
    prBkgOffset.second = 0;

    _relayout();

    //update();
    this->repaint();
}

void MainWindow::handleTouchEvent(const CTouchEvent& te)
{
    if (g_crLogoBkg.alpha() > 0)
    {
        return;
    }

    static bool bTouchSingerImg = false;
    auto teType = te.type();
    if (E_TouchEventType::TET_TouchBegin == teType)
    {
        bTouchSingerImg = !m_bDefaultBkg && ui.labelSingerImg->pixmap()
                && ui.labelSingerImg->geometry().contains(te.x(), te.y());
        return;
    }

    if (E_TouchEventType::TET_TouchMove == teType)
    {
        if (m_bDefaultBkg || m_opt.bUseBkgColor)
        {
            return;
        }

        __yield();
        auto& prBkgOffset = g_bHLayout?m_opt.prHBkgOffset:m_opt.prVBkgOffset;
        prBkgOffset.first -= te.dx();
        prBkgOffset.second -= te.dy();
        update();
    }
    else if (E_TouchEventType::TET_TouchEnd == teType)
    {
        if (bTouchSingerImg)
        {
            bTouchSingerImg = false;
            return;
        }

        if (te.dt() < __fastTouchDt)// && !m_opt.bUseBkgColor)
        {
            auto dx = te.dx();
            auto dy = te.dy();
            if (abs(dx) > abs(dy))
            {
                if (dx >= 3)
                {
                    m_bkgDlg.switchBkg(g_bHLayout, false);
                }
                else if (dx <= -3)
                {
                    m_bkgDlg.switchBkg(g_bHLayout, true);
                }
            }
            else
            {
                if (dy >= 3)
                {
                    m_bkgDlg.switchBkg(g_bHLayout, false);
                }
                else if (dy <= -3)
                {
                    m_bkgDlg.switchBkg(g_bHLayout, true);
                }
            }
        }
    }
}

void MainWindow::slot_progressTouch(int x, int y)
{
    if (!g_app.getModel().getUserMgr().isVip())
    {
        return;
    }

    cauto progressbar = *ui.progressbar;
    UINT uMax = progressbar.maximum();
    if (0 == uMax)
    {
        return;
    }

    auto& player = g_app.getPlayMgr().player();
    /*if (player.sampleRate() == 0) //!player.isOpen())
    {
        return;
    }*/

    (void)y;
    UINT uSeekPos = uMax*x/progressbar.width();
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

    if (player.Seek(uSeekPos))
    {
        async(100, [&]{
          _updateProgress();
        });
    }
}

void MainWindow::slot_labelClick(CLabel* label, const QPoint& pos)
{
    if (label == ui.labelSingerImg)
    {
        if (!m_bDefaultBkg)
        {
            auto& eSingerImgPos = (E_SingerImgPos&)(g_bHLayout?m_opt.uHSingerImgPos:m_opt.uVSingerImgPos);
            eSingerImgPos = E_SingerImgPos((int)eSingerImgPos+1);
            if (eSingerImgPos > E_SingerImgPos::SIP_Zoomout)
            {
                eSingerImgPos = E_SingerImgPos::SIP_Float;
            }

            _relayout();
        }
    }
    else if (label == ui.labelSingerName)
    {
        if (m_PlayingInfo.pSinger)
        {
            m_medialibDlg.showMediaSet(*m_PlayingInfo.pSinger);
        }
    }
    else if (label == ui.labelAlbumName)
    {
        if (m_PlayingInfo.pIMedia)
        {
            m_medialibDlg.showMediaSet(*m_PlayingInfo.pIMedia);
        }
    }
    else if (label == ui.labelPlayingfile)
    {
        if (m_PlayingInfo.pXpkMediaRes)
        {
            m_medialibDlg.showMedia(*m_PlayingInfo.pXpkMediaRes);
            return;
        }

        if (m_PlayingInfo.pIMedia)
        {
            (void)m_medialibDlg.showMedia(*m_PlayingInfo.pIMedia);
            return;
        }

        m_medialibDlg.showLocalFile(m_PlayingInfo.strPath);
    }
    else
    {
#define __wcCheck L'√'
        m_eDemandLanguage = E_LanguageType::LT_None;

        PairList<CLabel*, E_LanguageType> plLabels {
            {ui.labelDemandCN, E_LanguageType::LT_CN}
            , {ui.labelDemandHK, E_LanguageType::LT_HK}
            , {ui.labelDemandKR, E_LanguageType::LT_KR}
            , {ui.labelDemandJP, E_LanguageType::LT_JP}
            , {ui.labelDemandEN, E_LanguageType::LT_EN}
            , {ui.labelDemandOther, E_LanguageType::LT_Other}};
        plLabels([&](CLabel* lbl, E_LanguageType eLanguage) {
            cauto text = lbl->text();
            if (text.startsWith(__wcCheck))
            {
                lbl->setText(text.mid(1));
            }
            else
            {
                if (lbl == label)
                {
                    m_eDemandLanguage = eLanguage;
                    lbl->setText(QChar(__wcCheck) + text);
                }
            }
        });

        updateDemandButton();
    }
}

void MainWindow::updateDemandButton()
{
    bool bEnable = _checkSingerDemandable();
    ui.btnDemandSinger->setEnabled(bEnable);

    bEnable = _checkAlbumDemandable();
    ui.btnDemandAlbum->setEnabled(bEnable);

    bEnable = _checkPlaylistDemandable();
    ui.btnDemandPlaylist->setEnabled(bEnable);
    ui.btnDemandPlayItem->setEnabled(bEnable);
}

void MainWindow::checkDemandable()
{
    auto& PlayMgr = g_app.getPlayMgr();

    for (cauto eLanguage : {
        E_LanguageType::LT_CN,
        E_LanguageType::LT_HK,
        E_LanguageType::LT_KR,
        E_LanguageType::LT_JP,
        E_LanguageType::LT_EN,
        E_LanguageType::LT_Other})
    {
        set<E_DemandMode> setDemandMode;
        if (PlayMgr.checkDemandable(E_DemandMode::DM_DemandSinger, eLanguage))
        {
            setDemandMode.insert(E_DemandMode::DM_DemandSinger);
            m_bSingerDemandable = true;
        }
        if (PlayMgr.checkDemandable(E_DemandMode::DM_DemandAlbum, eLanguage))
        {
            m_bSingerDemandable = true;
            setDemandMode.insert(E_DemandMode::DM_DemandAlbum);
            m_bAlbumDemandable = true;
        }
        if (PlayMgr.checkDemandable(E_DemandMode::DM_DemandPlaylist, eLanguage))
        {
            setDemandMode.insert(E_DemandMode::DM_DemandPlaylist);
            m_bPlaylistDemandable = true;
            //setDemandMode.insert(E_DemandMode::DM_DemandPlayItem);
        }

        if (!setDemandMode.empty())
        {
            m_mapDemandLanguage[eLanguage].swap(setDemandMode);
        }
    }
}
