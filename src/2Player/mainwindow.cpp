
#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QTimer>

#include <QMovie>

#include "widget.cpp"

#include "bkgdlg.h"

static CMtxLock<tagPlayingInfo> g_mtxPlayingInfo;

static Ui::MainWindow ui;

MainWindow::MainWindow(CPlayerView& view) :
    m_view(view),
    m_PlayingList(view),
    m_medialibDlg(view),
    m_bkgDlg(view)
{
    ui.setupUi(this);

    m_bkgDlg.setDefaultBkg(*ui.labelBkg->pixmap());

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

        , ui.labelDuration, ui.progressBar, ui.labelPlayProgress

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

    ui.btnPause->setVisible(false);
    ui.centralWidget->setVisible(false);

    m_PlayingList.setParent(ui.centralWidget);
    m_PlayingList.raise();

    ui.btnExit->raise();

    this->setWindowFlags(Qt::FramelessWindowHint);
}

void MainWindow::showLogo()
{
    float fFontSizeOffset = -0.5;
#if __android
    fFontSizeOffset = -2;
#endif
    ui.labelLogoTip->setFont(CFont(fFontSizeOffset, E_FontWeight::FW_Light, true));
    ui.labelLogoCompany->setFont(CFont(fFontSizeOffset/3));

    QPalette peTip;
    peTip.setColor(QPalette::WindowText, QColor(64, 128, 255));
    ui.labelLogoTip->setPalette(peTip);

    QPalette peCompany;
    peCompany.setColor(QPalette::WindowText, QColor(64, 128, 255, 0));
    ui.labelLogoCompany->setPalette(peCompany);

    static QMovie movie(":/img/logo.gif");
    ui.labelLogo->setMovie(&movie);

    QPalette pe;
    pe.setColor(QPalette::Background, QColor(180, 220, 255));
    this->setPalette(pe);

    QMainWindow::showFullScreen();
    this->setWindowState(Qt::WindowFullScreen);

    QTimer::singleShot(800, [&](){
        ui.labelLogo->movie()->start();

        m_view.setTimer(40, [&](){
            auto peCompany = ui.labelLogoCompany->palette();
            auto crCompany = peCompany.color(QPalette::WindowText);

            auto alpha = crCompany.alpha() + 5;
            crCompany.setAlpha(alpha);
            peCompany.setColor(QPalette::WindowText, crCompany);
            ui.labelLogoCompany->setPalette(peCompany);

            if (alpha >= 255)
            {
                m_view.setTimer(500, [&](){
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

        QTimer::singleShot(500, [&](){
            auto labelLogoTip = ui.labelLogoTip;
            labelLogoTip->setText(labelLogoTip->text() + "播放器");

            QTimer::singleShot(500, [=](){
                labelLogoTip->setText(labelLogoTip->text() + " · 媒体库");

                QTimer::singleShot(500, [=](){
                    labelLogoTip->setText(labelLogoTip->text() + "  个性化定制");
                });
            });
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
        label->setFont(0, E_FontWeight::FW_SemiBold);
    }
    lstLabels.add(ui.labelSingerImg, ui.labelSingerName, ui.labelAlbumName, ui.labelPlayingfile
                , ui.labelPlayProgress);
    for (auto label : lstLabels)
    {
        connect(label, &CLabel::signal_click, this, &MainWindow::slot_labelClick);
    }
    lstLabels.add(ui.labelDuration);
    for (auto label : lstLabels)
    {
        label->setTextColor(255,255,255);
    }

    ui.labelSingerName->setShadow(2);

    connect(this, &MainWindow::signal_showPlaying, this, &MainWindow::slot_showPlaying);
    connect(this, &MainWindow::signal_playFinish, this, &MainWindow::slot_playFinish);

    ui.frameDemand->setAttribute(Qt::WA_TranslucentBackground);
    ui.frameDemandLanguage->setAttribute(Qt::WA_TranslucentBackground);

    ui.labelSingerName->setFont(0.5);
    ui.labelPlayingfile->setFont(0.5);
    ui.labelDuration->setFont(-1);

    if (m_view.getOptionMgr().getOption().bRandomPlay)
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

    _relayout();

    ui.labelLogo->movie()->stop();
    ui.labelLogo->setVisible(false);
    ui.labelLogoTip->setVisible(false);
    ui.labelLogoCompany->setVisible(false);

    m_timer = startTimer(1000);

    ui.centralWidget->setVisible(true);
}

bool MainWindow::event(QEvent *ev)
{
    switch (ev->type())
    {
    case QEvent::Move:
    case QEvent::Resize:
    case QEvent::Show:
        _relayout();

        break;
    case QEvent::Timer:
    {
        auto ePlayStatus = m_view.getPlayMgr().GetPlayStatus();
        if (E_PlayStatus::PS_Stop != ePlayStatus)
        {
            _playSingerImg(false);

            if (E_PlayStatus::PS_Play == ePlayStatus)
            {
                uint64_t uClock = m_view.getPlayMgr().getPlayer().getClock();
                if (uClock > 0)
                {
                    int nProgress = uClock / __1e6;
                    if (nProgress <= ui.progressBar->maximum())
                    {
                        ui.progressBar->setValue(nProgress);
                    }
                }
            }
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
        y_Logo -= 60;
    }
    else
    {
        y_Logo -= 100;
    }
    ui.labelLogo->move(x_Logo, y_Logo);

    int x_LogoTip = (cx - ui.labelLogoTip->width())/2;
    int y_LogoTip = ui.labelLogo->geometry().bottom() + 10;
    ui.labelLogoTip->move(x_LogoTip, y_LogoTip);

    ui.labelLogoCompany->adjustSize();
    int y_LogoCompany = cy - 50 - ui.labelLogoCompany->height();
    int x_LogoCompany = 0;
    if (m_bHScreen)
    {
        x_LogoCompany = cx - 50 - ui.labelLogoCompany->width();
    }
    else
    {
        x_LogoCompany = (cx-ui.labelLogoCompany->width())/2;
    }
    ui.labelLogoCompany->move(x_LogoCompany, y_LogoCompany);

    cauto& pmDefaultBkg = m_bkgDlg.defaultBkg();
    float fCXRate = 0;
    if (m_bHScreen)
    {
        fCXRate = (float)cx/pmDefaultBkg.width();
    }
    else
    {
        fCXRate = (float)cx/1080;
    }
    int cy_bkg = fCXRate*pmDefaultBkg.height();
    int dy_bkg = cy - cy_bkg;

    const QPixmap &pmBkg = m_bHScreen?m_bkgDlg.hbkg():m_bkgDlg.vbkg();
    if (!pmBkg.isNull())
    {
        m_bUsingCustomBkg = true;

        ui.labelBkg->setPixmap(pmBkg);

        float fHWRate = (float)pmBkg.height()/pmBkg.width();
        if (fHWRate > (float)cy/cx)
        {
            cy_bkg = fHWRate * cx;
            ui.labelBkg->setGeometry(0, -(cy_bkg-cy)/2, cx, cy_bkg);
        }
        else
        {
            int cx_bkg = (float)cy / fHWRate;
            ui.labelBkg->setGeometry(-(cx_bkg-cx)/2, 0, cx_bkg, cy);
        }
    }
    else
    { 
        m_bUsingCustomBkg = false;

        ui.labelBkg->setPixmap(pmDefaultBkg);

        if (m_bHScreen)
        {
            ui.labelBkg->resize(cx, cy_bkg);
        }
        else
        {
            ui.labelBkg->resize(fCXRate*pmDefaultBkg.width(), cy_bkg);
        }
        ui.labelBkg->move(0, dy_bkg);
    }

    ui.labelDemandCN->setShadow(m_bUsingCustomBkg?1:0);
    ui.labelDemandHK->setShadow(m_bUsingCustomBkg?1:0);
    ui.labelDemandKR->setShadow(m_bUsingCustomBkg?1:0);
    ui.labelDemandJP->setShadow(m_bUsingCustomBkg?1:0);
    ui.labelDemandTAI->setShadow(m_bUsingCustomBkg?1:0);
    ui.labelDemandEN->setShadow(m_bUsingCustomBkg?1:0);
    ui.labelDemandEUR->setShadow(m_bUsingCustomBkg?1:0);

    ui.labelAlbumName->setShadow(m_bUsingCustomBkg?2:0);

    ui.labelDuration->setShadow(m_bUsingCustomBkg?2:0);

    for (cauto& widgetPos : m_mapTopWidgetPos)
    {
        QRect pos = widgetPos.second;
        if (fCXRate < 1)
        {
            pos.setRect(fCXRate*pos.left(), fCXRate*pos.top()
                        , fCXRate*pos.width(), fCXRate*pos.height());
        }
        widgetPos.first->setGeometry(pos);
    }

    for (cauto& widgetPos : m_mapWidgetPos)
    {
        cauto& pos = widgetPos.second;
        auto& newPos = m_mapWidgetNewPos[widgetPos.first];

        if (fCXRate >= 1 && NULL != dynamic_cast<QPushButton*>(widgetPos.first))
        {
            newPos.setRect(fCXRate*pos.center().x()-pos.width()/2
                           , fCXRate*pos.center().y()-pos.height()/2+dy_bkg, pos.width(), pos.height());
        }
        else
        {
            newPos.setRect(fCXRate*pos.left(), fCXRate*pos.top()+dy_bkg
                           , fCXRate*pos.width(), fCXRate*pos.height());
        }
        widgetPos.first->setGeometry(newPos);
    }

    int y_frameDemand = 20;
    if (cy > 1920)
    {
        y_frameDemand = 35;
    }
    if (1920 == cy)
    {
        y_frameDemand = 28;
    }
    else if (cy < 1000)
    {
        if (!m_bUsingCustomBkg)
        {
            y_frameDemand = 12;
        }
    }

    int x_frameDemand = 0;
    if (m_bHScreen)
    {
        x_frameDemand = (ui.progressBar->geometry().right()
            + ui.progressBar->x() - ui.frameDemand->width())/2;
    }
    else
    {        
        x_frameDemand = (cx - ui.frameDemand->width())/2;
        x_frameDemand -= 10;
    }
    ui.frameDemand->move(x_frameDemand, y_frameDemand);

    int x_btnMore = 20;
#if __android
     ui.btnExit->move(-200,-200);

     if (!m_bHScreen)
     {
         x_btnMore = cx - 20 - ui.btnMore->width();
     }
#else
    int x_btnExit = cx - ui.btnExit->width() - (y_frameDemand + 10);
    ui.btnExit->move(x_btnExit, y_frameDemand + 10);
#endif
    ui.btnMore->move(x_btnMore, y_frameDemand+10);

    int y_PlayingListMax = 0;

    ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    ui.labelAlbumName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
    ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignBottom);

    bool bZoomoutSingerImg = m_bZoomoutSingerImg;
    const QPixmap *pmSingerImg = ui.labelSingerImg->pixmap();
    if (NULL != pmSingerImg && !pmSingerImg->isNull())
    {
        ui.wdgSingerImg->setVisible(true);
    }
    else
    {
        ui.wdgSingerImg->setVisible(false);

        bZoomoutSingerImg = false;
    }

    if (m_bUsingCustomBkg)
    {
        int x = ui.progressBar->x();

        int cy_Playingfile = ui.labelPlayingfile->height();
        int y_Playingfile = ui.labelDuration->geometry().bottom() -  cy_Playingfile;
        ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x() - x, cy_Playingfile);

        int cy_labelAlbumName = 80;
        int y_labelAlbumName = y_Playingfile - cy_labelAlbumName;
        int cx_progressBar = ui.progressBar->width();
        ui.labelAlbumName->setGeometry(x, y_labelAlbumName, cx_progressBar, cy_labelAlbumName);

        ui.labelAlbumName->setFont(0.5);

        int y_SingerImg = 0;
        if (bZoomoutSingerImg)
        {
            ui.labelAlbumName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

            ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

            y_SingerImg = y_labelAlbumName-300;
        }
        else
        {
            if (m_bHScreen)
            {
                y_SingerImg = ui.frameDemandLanguage->geometry().bottom() + 50;
            }
            else
            {
                y_SingerImg = cy/2+100;
            }
        }

        int cy_SingerImg = y_labelAlbumName-y_SingerImg;
        cauto& rcSingerImg = m_mapWidgetPos[ui.wdgSingerImg];
        int cx_SingerImg = rcSingerImg.width()*cy_SingerImg/rcSingerImg.height();

        int x_SingerImg = x;
        if (!bZoomoutSingerImg)
        {
            x_SingerImg += (cx_progressBar-cx_SingerImg)/2;
        }

        ui.wdgSingerImg->setGeometry(x_SingerImg, y_SingerImg, cx_SingerImg, cy_SingerImg);

        int y_labelSingerName = y_labelAlbumName-ui.labelSingerName->height();
        ui.labelSingerName->setGeometry(x_SingerImg+15, y_labelSingerName, cx_SingerImg-15, ui.labelSingerName->height());

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

        m_PlayingList.setFont(-1);
        m_PlayingList.setTextColor(255, 255, 255);
        m_PlayingList.setInactiveAlpha(0.4);
        m_PlayingList.setShadow(2);
    }
    else
    {
        m_PlayingList.setFont(-1.5);
        m_PlayingList.setTextColor(255, 255, 255, 160);
        m_PlayingList.setInactiveAlpha(0.33);
        m_PlayingList.setShadow(0);

        ui.labelAlbumName->setFont(0);

        bool bFlag = false;
        if (m_bHScreen)
        {
            bFlag = (cy < 1080 || fCXRate > 1);
        }
        else
        {
            bFlag = cy < 1920;
        }

        if (bFlag)
        {
            int y_Playingfile = ui.wdgSingerImg->geometry().bottom()- ui.labelPlayingfile->height() - 20;
            ui.labelPlayingfile->move(ui.wdgSingerImg->x() + 30, y_Playingfile);

            y_PlayingListMax = ui.wdgSingerImg->y();
        }
        else
        {
            ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignBottom);

            y_PlayingListMax = ui.labelPlayingfile->y();
        }
    }

    UINT uMargin = 0;
    if (m_bUsingCustomBkg)
    {
        uMargin = 2;
    }
    ui.labelSingerImg->setGeometry(uMargin, uMargin, ui.wdgSingerImg->width() - uMargin*2
                                                   , ui.wdgSingerImg->height() - uMargin*2);

    auto pPixmap = ui.labelSingerImg->pixmap();
    if (NULL != pPixmap && !pPixmap->isNull())
    {
        ui.labelSingerImg->setPixmap(*pPixmap);
    }

    _showAlbumName();

    UINT uRowCount = 10;
    if (m_bHScreen)
    {
        UINT uMargin = 45;
        int x_PlayingList = ui.progressBar->geometry().right();
        x_PlayingList += 90 * fCXRate;

        m_PlayingList.setGeometry(x_PlayingList, uMargin-1, cx-x_PlayingList-uMargin*fCXRate, cy-uMargin*2);
    }
    else
    {
        UINT y_Margin = 30;

        int y_frameDemandBottom = ui.frameDemand->geometry().bottom();
        uRowCount = (y_PlayingListMax - y_frameDemandBottom)/100;
        if (uRowCount > 10)
        {
            uRowCount = 10;
            y_Margin += 30;

            if (bZoomoutSingerImg)
            {
                y_Margin += 30;
            }
        }
        else if (uRowCount < 7)
        {
            uRowCount = 7;
            y_Margin -= 10;
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

void MainWindow::refreshPlayingList(int nPlayingItem, bool bSetActive)
{
    (void)bSetActive;

    m_PlayingList.updateList(nPlayingItem);
}

void MainWindow::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual)
{
    PlayItem.AsyncTask();

    g_mtxPlayingInfo.lock([&](tagPlayingInfo& PlayingInfo) {
        PlayingInfo.strTitle = PlayItem.GetTitle();

        PlayingInfo.nDuration = PlayItem.GetDuration();

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

void MainWindow::onPlayFinish()
{
    emit signal_playFinish();

    m_view.getCtrl().callPlayCtrl(E_PlayCtrl::PC_AutoPlayNext);
}

void MainWindow::slot_playFinish()
{
    ui.progressBar->setValue(0);

    _updatePlayPauseButton(false);
}

void MainWindow::slot_showPlaying(unsigned int uPlayingItem, bool bManual)
{
    m_PlayingList.updatePlayingItem(uPlayingItem, bManual);

    g_mtxPlayingInfo.get(m_PlayingInfo);
    _showAlbumName();

    ui.labelPlayingfile->setText(wsutil::toQStr(m_PlayingInfo.strTitle));

    if (m_PlayingInfo.strSinger != m_strSingerName)
    {
        m_strSingerName = m_PlayingInfo.strSinger;

        ui.labelSingerName->setText(wsutil::toQStr(m_strSingerName));

        ui.labelSingerImg->setPixmap(QPixmap());

        if (!m_strSingerName.empty())
        {
            _playSingerImg(true);
        }

        _relayout();
    }

    ui.progressBar->setValue(0);

    if (m_PlayingInfo.nDuration > 0)
    {
        ui.progressBar->setMaximum(m_PlayingInfo.nDuration);
        QString qsDuration = wsutil::toQStr(CMedia::GetDurationString(m_PlayingInfo.nDuration));

        ui.labelDuration->setText(qsDuration);

        _updatePlayPauseButton(true);
    }
    else
    {
        ui.labelDuration->clear();

        _updatePlayPauseButton(false);
    }
}

void MainWindow::_showAlbumName()
{
    auto eDemandMode = m_view.getPlayMgr().demandMode();

    WString strMediaSet;
    if (E_DemandMode::DM_DemandPlayItem == eDemandMode || m_PlayingInfo.strAlbum.empty())
    {
        if (!m_PlayingInfo.strPlaylist.empty())
        {
            if (m_bUsingCustomBkg)
            {
                strMediaSet << L"列表：";
            }
            strMediaSet << m_PlayingInfo.strPlaylist;

            m_PlayingInfo.uRelatedAlbumItemID = 0;
        }
    }
    else if (!m_PlayingInfo.strAlbum.empty())
    {
        if (m_bUsingCustomBkg)
        {
            strMediaSet << L"专辑：";
        }
        strMediaSet << m_PlayingInfo.strAlbum;

        m_PlayingInfo.uRelatedPlayItemID = 0;
    }

    QLabel& labelAlbumName = *ui.labelAlbumName;
    if (strMediaSet.empty())
    {
        labelAlbumName.setVisible(false);
        return;
    }

    labelAlbumName.setVisible(true);
    labelAlbumName.setText(strMediaSet);

    if (!m_bUsingCustomBkg)
    {
        cauto& rcAlbumNamePrev = m_mapWidgetNewPos[&labelAlbumName];
        labelAlbumName.adjustSize();
        if (labelAlbumName.width() < rcAlbumNamePrev.width())
        {
            int x_labelAlbumName = rcAlbumNamePrev.left() + (rcAlbumNamePrev.width()-labelAlbumName.width())/2;
            labelAlbumName.move(x_labelAlbumName, labelAlbumName.y());
        }
        else
        {
            labelAlbumName.move(rcAlbumNamePrev.left(), labelAlbumName.y());
        }
    }
}

void MainWindow::_playSingerImg(bool bReset)
{
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

    if (m_strSingerName.empty())
    {
        return;
    }

    wstring strSingerImg;
    if (m_view.getModel().getSingerImgMgr().getSingerImg(m_strSingerName, uSingerImgIdx, strSingerImg))
    {
        QPixmap pm;
        if (pm.load(wsutil::toQStr(strSingerImg)))
        {
            ui.labelSingerImg->setPixmap(pm);
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
        m_view.getApp().quit();
    }
    else if (button == ui.btnPause)
    {
        m_view.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Pause));

        _updatePlayPauseButton(false);
    }
    else if (button == ui.btnPlay)
    {
        m_view.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Play));

        _updatePlayPauseButton(true);
    }
    else if (button == ui.btnPlayPrev)
    {
        m_view.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_PlayPrev));
    }
    else if (button == ui.btnPlayNext)
    {
        m_view.getCtrl().callPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_PlayNext));
    }
    else if (button == ui.btnRandom || button == ui.btnOrder)
    {
        auto& bRandomPlay = m_view.getOptionMgr().getOption().bRandomPlay;
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
}

static const QString __qsCheck = wsutil::toQStr(L"√");

void MainWindow::slot_labelClick(CLabel* label, const QPoint& pos)
{
    if (label == ui.labelSingerImg)
    {
        if (m_bUsingCustomBkg)
        {
            m_bZoomoutSingerImg = !m_bZoomoutSingerImg;
            this->_relayout();
        }
    }
    else if (label == ui.labelSingerName)
    {
        if (m_PlayingInfo.uSingerID != 0)
        {
            CMediaSet *pMediaSet = m_view.getModel().getSingerMgr().HittestMediaSet(E_MediaSetType::MST_Singer, m_PlayingInfo.uSingerID);
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
            pMedia = m_view.getModel().getSingerMgr().HittestMedia(
                                    E_MediaSetType::MST_Album, m_PlayingInfo.uRelatedAlbumItemID);
        }
        else if (m_PlayingInfo.uRelatedPlayItemID != 0)
        {
            pMedia = m_view.getModel().getPlaylistMgr().HittestMedia(
                                    E_MediaSetType::MST_Playlist, m_PlayingInfo.uRelatedPlayItemID);
        }
        if (pMedia && pMedia->m_pParent)
        {
            m_medialibDlg.showMedia(*pMedia);
        }
    }
    else if (label == ui.labelPlayingfile)
    {
        m_medialibDlg.showFile(m_PlayingInfo.strPath);
    }
    else if (label == ui.labelPlayProgress)
    {
        if (E_PlayStatus::PS_Play == m_view.getPlayMgr().GetPlayStatus())
        {
            if (ui.progressBar->maximum() > 0)
            {
                UINT uPos = pos.x() * ui.progressBar->maximum() /ui.progressBar->width();
                m_view.getPlayMgr().getPlayer().Seek(uPos);

                mtutil::yield();

                ui.progressBar->setValue(uPos);
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
            if (!lblLanguage->text().startsWith(' '))
            {
                lblLanguage->setText("  " + lblLanguage->text().mid(1));

                lblLanguage->setTextColor(255,255,255);
            }
            else
            {
                if (lblLanguage == label)
                {
                    m_eDemandLanguage = eLanguage;

                    lblLanguage->setText(__qsCheck + lblLanguage->text().mid(2));

                    lblLanguage->setTextColor(210,255,220);
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

    m_view.getCtrl().callPlayCtrl(tagPlayCtrl(eDemandMode, m_eDemandLanguage));
}
