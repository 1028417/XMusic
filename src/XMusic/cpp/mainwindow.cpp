
#include "app.h"

#include "widget.cpp"

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMovie>

#include <QScreen>

#define __demandplayitemPngCount 4

Ui::MainWindow ui;

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
    __app.getOption().bFullScreen = g_bFullScreen;

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

MainWindow::MainWindow()
    : m_brBkg(QPixmap(":/img/bkg.jpg"))
    , m_PlayingList(this)
    , m_medialibDlg(*this)
    , m_bkgDlg(*this)
{
    ui.setupUi(this);

    ui.centralWidget->init();

    for (auto button : SList<CButton*>(ui.btnFullScreen, ui.btnExit, ui.btnMore
                , ui.btnDemandSinger, ui.btnDemandAlbum, ui.btnDemandAlbumItem
                , ui.btnDemandPlayItem, ui.btnDemandPlaylist
                , ui.btnSetting, ui.btnOrder, ui.btnRandom
                , ui.btnPause, ui.btnPlay, ui.btnPlayPrev, ui.btnPlayNext))
    {
        connect(button, &CButton::signal_clicked, this, &MainWindow::slot_buttonClicked);
    }

    for (auto widget : SList<QWidget*>(ui.labelLogo, ui.labelLogoTip, ui.labelLogoCompany
                                       , ui.centralWidget, ui.frameDemand, ui.frameDemandLanguage))
    {
        widget->setAttribute(Qt::WA_TranslucentBackground);
    }

    m_PlayingList.raise();

#if __android || __ios
    int nLogoWidth = g_szScreenMin*42/100;
    ui.labelLogo->resize(nLogoWidth, nLogoWidth/4);
    ui.labelLogo->setScaledContents(true);

    ui.btnFullScreen->setVisible(false);
    ui.btnExit->setVisible(false);

#else
    ui.btnFullScreen->setParent(this);

    ui.btnExit->setParent(this);
    ui.btnExit->raise();
#endif

    ui.btnPause->setVisible(false);

    ui.centralWidget->setVisible(false);

    this->setStyleSheet("");

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
}

void MainWindow::showBlank()
{
    g_bFullScreen = __app.getOption().bFullScreen;
    fixWorkArea(*this);

    this->setVisible(true);
}

void MainWindow::showLogo()
{
    float fFontSizeOffset = 1.072f;
#if __android || __ios
    fFontSizeOffset = 0.918f;
#endif
    ui.labelLogoTip->setFont(CFont(fFontSizeOffset, g_nDefFontWeight, true));
    ui.labelLogoCompany->setFont(CFont(fFontSizeOffset));

    CDialog::setWidgetTextColor(ui.labelLogoTip, QColor(__crLogoText));
    CDialog::setWidgetTextColor(ui.labelLogoCompany, QColor(__crLogoText, 0));

    auto movie = new QMovie(this);
    movie->setFileName(":/img/logo.gif");
    ui.labelLogo->setMovie(movie);
    ui.labelLogo->movie()->start();
    ui.labelLogo->setParent(this);
    ui.labelLogo->setVisible(true);

    async(30, [&](){
        auto labelLogoTip = ui.labelLogoTip;
        labelLogoTip->setParent(this);
        labelLogoTip->setVisible(true);
        labelLogoTip->setText("播放器");

        async(600, [=](){
            labelLogoTip->setText(labelLogoTip->text() + WString(__CNDot L"媒体库"));

            async(600, [=](){
                labelLogoTip->setText(labelLogoTip->text() + "  个性化定制");

                async(2600, [=](){
                    _showUpgradeProgress();
                });
            });
        });
    });

    ui.labelLogoCompany->setParent(this);
    ui.labelLogoCompany->setVisible(true);

    _updateLogoCompany(5, [&](){
        _updateLogoCompany(-5, [&](){
            ui.labelLogoCompany->setText(__WS2Q(L"v" + __app.appVersion()));
            _updateLogoCompany(5);
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

    if (__app.getOption().bRandomPlay)
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
    _relayout();

    m_PlayingList.updateList(__app.getOption().uPlayingItem);

    (void)startTimer(1000);

    auto nLogoBkgAlpha = g_crLogoBkg.alpha();
    UINT uOffset = 23;
#if __windows || __mac
    if (std::thread::hardware_concurrency() > 4)
    {
        uOffset = 1;
    }
#endif
    UINT uDelayTime = __app.getOption().bUseBkgColor?50:30;
    timerutil::setTimerEx(uDelayTime, [=]()mutable{
        uOffset+=1;
        nLogoBkgAlpha -= uOffset;
        if (nLogoBkgAlpha <= 0)
        {
            g_crLogoBkg.setAlpha(0);
            update();
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
        /*else if (Qt::Key_Escape == key)
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
                if (E_PlayStatus::PS_Play == __app.getPlayMgr().playStatus())
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
        __app.quit();

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
    int cx = width();
    int cy = height();
    m_bHLayout = cx > cy; // 橫屏

    m_bDefaultBkg = false;
    if (!__app.getOption().bUseBkgColor)
    {
        cauto pmBkg = m_bHLayout?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
        m_bDefaultBkg = pmBkg.isNull();
    }

    ui.centralWidget->relayout(cx, cy, m_bDefaultBkg, m_eSingerImgPos, m_PlayingInfo, m_PlayingList);
}

void MainWindow::_onPaint()
{
    CPainter painter(*this);
    cauto rc = this->rect();

    auto nLogoAlpha = g_crLogoBkg.alpha();
    auto nBkgAlpha = 255-nLogoAlpha;
    if (nBkgAlpha > 0)
    {
        if (__app.getOption().bUseBkgColor)
        {
            painter.fillRect(rc, g_crBkg);
        }
        else
        {
            painter.setRenderHints(QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);

            cauto pmBkg = rc.width()>rc.height() ?m_bkgDlg.hbkg() :m_bkgDlg.vbkg();
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
                drawDefaultBkg(painter, rc, 0, 0, !ui.labelSingerImg->pixmap().isNull());
            }
        }
    }

    if (nLogoAlpha > 0)
    {
        painter.fillRect(rc, g_crLogoBkg);
    }
}

void MainWindow::drawDefaultBkg(CPainter& painter, cqrc rc, UINT xround, UINT yround, bool bDrawDisk)
{
    int cxDst = rc.width();
    int cyDst = rc.height();
    int xDst = 0;
    auto fBkgZoomRate = ui.centralWidget->caleBkgZoomRate(cxDst, cyDst, xDst);

    auto cySrc = cyDst/fBkgZoomRate;
    QRect rcSrc(0, __cyBkg-cySrc, 10, cySrc);
    painter.drawPixmap(rc, m_brBkg, rcSrc, xround, yround);

    if (!bDrawDisk)
    {
        painter.setOpacity(0.06f);
    }

    rcSrc.setWidth(__cxBkg);
    QRect rcDst(rc.x()+xDst, rc.y(), cxDst, cyDst);
    painter.drawPixmap(rcDst, m_brBkg, rcSrc);

    if (!bDrawDisk)
    {
        painter.setOpacity(1.0f);
    }
}

void MainWindow::_updateProgress()
{
    auto& mediaOpaque = __app.getPlayMgr().mediaOpaque();
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
    if (mediaOpaque.waitingFlag() && __app.getPlayMgr().player().packetQueueEmpty())
    {
        ui.labelDuration->setText("正在缓冲");
    }
    else
    {
        ui.labelDuration->setText(m_PlayingInfo.qsDuration);
    }

    UINT uBuffer = UINT(mediaOpaque.downloadedSize()/1000);
    if (uBuffer > 0)
    {
        ui.progressbar->setBuffer(uBuffer, m_PlayingInfo.uFileSize);
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
    __app.sync([=](){
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
        auto pMediaRes = __medialib.subFile(PlayingInfo.strPath);
        if (pMediaRes && !pMediaRes->isLocal())
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

    // TODO 获取音频流码率 if (!__app.getPlayMgr().mediaOpaque().isVideo()) // 本地视频文件不显示码率
    PlayingInfo.eQuality = PlayItem.quality();
    PlayingInfo.qsQuality = mediaQualityString(PlayingInfo.eQuality);

    auto uAlbumItemID = PlayItem.GetRelatedMediaID(E_RelatedMediaSet::RMS_Album);
    if (uAlbumItemID > 0)
    {
        PlayingInfo.pRelatedMedia = __app.getSingerMgr().GetMedia(uAlbumItemID);
        PlayingInfo.uSingerID = PlayItem.GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
        PlayingInfo.strSingerName = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
    }
    else
    {
        auto uPlayItemID = PlayItem.GetRelatedMediaID(E_RelatedMediaSet::RMS_Playlist);
        if (uPlayItemID > 0)
        {
            PlayingInfo.pRelatedMedia = __app.getPlaylistMgr().GetMedia(uPlayItemID);
        }
        else
        {
            auto pSinger = __app.getSingerMgr().checkSingerDir(PlayingInfo.strPath, false);
            if (pSinger)
            {
                PlayingInfo.uSingerID = pSinger->m_uID;
                PlayingInfo.strSingerName = pSinger->m_strName;
                PlayingInfo.pRelatedMedia = __medialib.subFile(PlayingInfo.strPath);
            }
        }
    }
    if (PlayingInfo.pRelatedMedia)
    {
        PlayingInfo.pRelatedMediaSet = PlayingInfo.pRelatedMedia->mediaSet();
    }

    __app.sync([=](){
        auto uPrevSingerID = m_PlayingInfo.uSingerID;
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
        ui.labelSingerName->setText(qsSingerName);

        if (m_PlayingInfo.uSingerID != uPrevSingerID)
        {
            ui.labelSingerImg->clear();
            update();
            _playSingerImg(true);
        }

        _relayout();
    });
}

void MainWindow::onPlayStop(bool bOpenSuccess, bool bPlayFinish)
{
    __app.sync([=](){
        ui.progressbar->set(0, 0, 0, 0);

        ui.labelDuration->setText(m_PlayingInfo.qsDuration);

        extern UINT g_uPlaySeq;
        if (bPlayFinish)
        {
            __app.getCtrl().callPlayCmd(E_PlayCmd::PC_AutoPlayNext);

            /*auto uPlaySeq = g_uPlaySeq;
            async(1000, [=]() {
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
            async(2000, [=]() {
                if (uPlaySeq == g_uPlaySeq)
                {
                    __app.getCtrl().callPlayCmd(E_PlayCmd::PC_AutoPlayNext);
                }
            });
        }
    });
}

void MainWindow::onSingerImgDownloaded(cwstr strSingerName, const tagSingerImg& singerImg)
{
    if (m_medialibDlg.isVisible())
    {
        __app.sync([&, strSingerName](){
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
        __app.sync([&](){
            _playSingerImg();
        });
    }
}

#define ___singerImgElapse 8

static UINT g_uSingerImgIdx = 0;

void MainWindow::_playSingerImg(bool bReset)
{
    if (0 == m_PlayingInfo.uSingerID)
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
    auto pSingerImg = __app.getSingerImgMgr().getSingerImg(m_PlayingInfo.strSingerName, g_uSingerImgIdx, true);
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
    (void)pm.load(__WS2Q(__app.getSingerImgMgr().file(*pSingerImg)));
    ui.labelSingerImg->setPixmap(pm);
    update();

    //if (!ui.labelSingerImg->isVisible()) ui.labelSingerImg->setVisible(true);

    _relayout();

    g_uSingerImgIdx++;
    (void)__app.getSingerImgMgr().getSingerImg(m_PlayingInfo.strSingerName, g_uSingerImgIdx, true);
}

void MainWindow::slot_buttonClicked(CButton* button)
{
    if (button == ui.btnFullScreen)
    {
        switchFullScreen();
    }
    else if (button == ui.btnExit)
    {
        __app.quit();
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
        //if (__app.getPlayMgr().mediaOpaque().byteRate())
        //{
        //    __app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_Pause));
        if (__app.getPlayMgr().player().Pause())
        {
            _updatePlayPauseButton(false);
        }
    }
    else if (button == ui.btnPlay)
    {
        /*if (E_PlayStatus::PS_Pause == __app.getPlayMgr().playStatus())
        {
            _updatePlayPauseButton(true);
        }
        __app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_Play));*/

        if (E_PlayStatus::PS_Stop == __app.getPlayMgr().playStatus())
        {
            __app.getCtrl().callPlayCmd(tagPlayIndexCmd(__app.getCtrl().getOption().uPlayingItem));
            return;
        }

        if (__app.getPlayMgr().player().Resume())
        {
            _updatePlayPauseButton(true);
        }
    }
    else if (button == ui.btnPlayPrev)
    {
        __app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_PlayPrev));
    }
    else if (button == ui.btnPlayNext)
    {
        __app.getCtrl().callPlayCmd(tagPlayCmd(E_PlayCmd::PC_PlayNext));
    }
    else if (button == ui.btnRandom || button == ui.btnOrder)
    {
        auto& bRandomPlay = __app.getOption().bRandomPlay;
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
        if (m_PlayingInfo.uSingerID != 0)
        {
            CMediaSet *pMediaSet = __app.getSingerMgr().GetSubSet(E_MediaSetType::MST_Singer, m_PlayingInfo.uSingerID);
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
        if (__app.getPlayMgr().player().Seek(uSeekPos))
        {
            async(100, [&](){
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
            cauto text = lbl->text();
            if (text.startsWith(__qsCheck))
            {
                lbl->setText(text.mid(__qsCheck.length()));
            }
            else
            {
                if (lbl == label)
                {
                    m_eDemandLanguage = eLanguage;
                    lbl->setText(__qsCheck + text);
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

    __app.getCtrl().callPlayCmd(tagDemandCmd(eDemandMode, m_eDemandLanguage));
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
        bTouchSingerImg = !m_bDefaultBkg && !ui.labelSingerImg->pixmap().isNull()
                && ui.labelSingerImg->geometry().contains(te.x(), te.y());
    }
    else if (E_TouchEventType::TET_TouchMove == type)
    {
        if (m_bDefaultBkg || __app.getOption().bUseBkgColor)
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
