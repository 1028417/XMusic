
#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QTimer>

#include <QMovie>

extern const ITxtWriter& g_logWriter;

static CMtxLock<tagPlayingItem> g_mtxPlayingItem;

static Ui::MainWindow ui;

MainWindow::MainWindow(CPlayerView& view) :
    m_view(view),
    m_PlayingList(m_view)
{
    ui.setupUi(this);

    m_mapWidgetPos = {
        {ui.btnSetting, ui.btnSetting->geometry()}
        , {ui.btnPlay, ui.btnPlay->geometry()}
        , {ui.btnPause, ui.btnPause->geometry()}
        , {ui.btnPlayPrev, ui.btnPlayPrev->geometry()}
        , {ui.btnPlayNext, ui.btnPlayNext->geometry()}
        , {ui.btnOrder, ui.btnOrder->geometry()}
        , {ui.btnRandom, ui.btnRandom->geometry()}

        , {ui.labelDuration, ui.labelDuration->geometry()}
        , {ui.progressBar, ui.progressBar->geometry()}
        , {ui.labelPlayProgress, ui.labelPlayProgress->geometry()}

        , {ui.labelPlayingfile, ui.labelPlayingfile->geometry()}
        , {ui.wdgSingerImg, ui.wdgSingerImg->geometry()}
        , {ui.labelSingerName, ui.labelSingerName->geometry()}
        , {ui.labelAlbumName, ui.labelAlbumName->geometry()}
    };

    _init();
}

void MainWindow::_init()
{
    ui.labelLogo->setParent(this);
    ui.labelLogoTip->setParent(this);
    ui.labelLogoCompany->setParent(this);

    ui.wdgSingerImg->setVisible(false);
    ui.btnPause->setVisible(false);
    ui.centralWidget->setVisible(false);

    m_PlayingList.setParent(ui.centralWidget);
    m_PlayingList.raise();

#ifdef __ANDROID__
    ui.btnExit->setVisible(false);
#else
    ui.btnExit->raise();
#endif

    SList<CButton*> lstButtons {ui.btnDemandSinger, ui.btnDemandAlbum, ui.btnDemandAlbumItem
                , ui.btnDemandPlayItem, ui.btnDemandPlaylist
                , ui.btnExit, ui.btnSetting, ui.btnPause, ui.btnPlay
                , ui.btnPlayPrev, ui.btnPlayNext, ui.btnOrder, ui.btnRandom};
    for (auto button : lstButtons)
    {
        button->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        connect(button, SIGNAL(signal_clicked(CButton*)), this, SLOT(slot_buttonClicked(CButton*)));
    }

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::GlobalColor::white);

    SList<QLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK, ui.labelDemandKR, ui.labelDemandJP
        , ui.labelDemandTAI, ui.labelDemandEN, ui.labelDemandEUR};
    for (auto label : lstLabels)
    {
        connect(label, SIGNAL(signal_clicked(CLabel*)), this, SLOT(slot_labelClicked(CLabel*)));
    }
    lstLabels.add({ui.labelPlayingfile, ui.labelSingerName, ui.labelAlbumName, ui.labelDuration});
    for (auto label : lstLabels)
    {
        label->setPalette(pe);
        label->setAttribute(Qt::WA_TranslucentBackground);
    }

    connect(ui.labelPlayProgress, SIGNAL(signal_mousePressEvent(CLabel*, const QPoint&))
        , this, SLOT(slot_progressMousePress(CLabel*, const QPoint&)));

    connect(this, SIGNAL(signal_showPlaying(unsigned int, bool))
            , this, SLOT(slot_showPlaying(unsigned int, bool)));
    connect(this, SIGNAL(signal_playFinish()), this, SLOT(slot_playFinish()));
}

void MainWindow::showLogo()
{
    double dbTipFontSize = -0.5;
#ifdef __ANDROID__
    dbTipFontSize -= 1;
#endif
    m_view.setFont(ui.labelLogoTip, dbTipFontSize, true, true);

    QPalette peTip;
    peTip.setColor(QPalette::WindowText, QColor(64, 128, 255));
    ui.labelLogoTip->setPalette(peTip);

    m_view.setFont(ui.labelLogoCompany, 0.5, true);

    QPalette peCompany;
    peCompany.setColor(QPalette::WindowText, QColor(64, 128, 255, 0));
    ui.labelLogoCompany->setPalette(peCompany);

    static QMovie movie(":/img/logo.gif");
    ui.labelLogo->setMovie(&movie);

    QPalette pe;
    pe.setColor(QPalette::Background, QColor(180, 220, 255));
    this->setPalette(pe);

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowState(Qt::WindowFullScreen);
    QMainWindow::show();

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
                m_view.setTimer(330, [&](){
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
                        crCompany.setAlpha(160);
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
                    labelLogoTip->setText(labelLogoTip->text() + "  个性化订制");
                });
            });
        });
    });
}

void MainWindow::show()
{
    m_strHBkgDir = fsutil::workDir() + L"/hbkg/";
    m_strVBkgDir = fsutil::workDir() + L"/vbkg/";

    if (!fsutil::existDir(m_strHBkgDir))
    {
        if (fsutil::createDir(m_strHBkgDir))
        {
            fsutil::copyFile(L"assets:/hbkg/win10.jpg", m_strHBkgDir + L"win10");
        }
    }

    fsutil::findFile(m_strHBkgDir, [&](const tagFileInfo& fileInfo) {
        m_lstHBkg.push_back(fileInfo.m_strName);
        return true;
    });

    if (!fsutil::existDir(m_strVBkgDir))
    {
        if (fsutil::createDir(m_strVBkgDir))
        {
            fsutil::copyFile(L"assets:/vbkg/win10.jpg", m_strVBkgDir + L"win10");
        }
    }

    fsutil::findFile(m_strVBkgDir, [&](const tagFileInfo& fileInfo) {
        m_lstVBkg.push_back(fileInfo.m_strName);
        return true;
    });

    auto& strHBkg = m_view.getDataMgr().getOption().strHBkg;
    if (!strHBkg.empty())
    {
        if (!m_HBkgPixmap.load(m_strHBkgDir + strHBkg))
        {
            strHBkg.clear();
        }
    }
    auto& strVBkg = m_view.getDataMgr().getOption().strVBkg;
    if (!strVBkg.empty())
    {
        if (m_VBkgPixmap.load(m_strVBkgDir + strVBkg))
        {
            strVBkg.clear();
        }
    }

    ui.frameDemand->setAttribute(Qt::WA_TranslucentBackground);
    ui.frameDemandLanguage->setAttribute(Qt::WA_TranslucentBackground);

    m_view.setFont(ui.labelSingerName, 0.2);
    m_view.setFont(ui.labelDuration, -2);

    if (m_view.getDataMgr().getOption().bRandomPlay)
    {
        ui.btnRandom->setVisible(true);
        ui.btnOrder->setVisible(false);
    }
    else
    {
        ui.btnRandom->setVisible(false);
        ui.btnOrder->setVisible(true);
    }

    _relayout();

    ui.labelLogo->movie()->stop();
    ui.labelLogo->setVisible(false);
    ui.labelLogoTip->setVisible(false);
    ui.labelLogoCompany->setVisible(false);

    m_timer = startTimer(1000);

    ui.centralWidget->setVisible(true);
}

void MainWindow::timerEvent(QTimerEvent* ev)
{
    if (ev->timerId() != m_timer)
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
    default:
        break;
    }

    return QMainWindow::event(ev);
}

void MainWindow::_relayout()
{
    int cx = this->width();
    int cy = this->height();
    bool bHScreen = cx > cy; // 橫屏

    int x_Logo = (cx - ui.labelLogo->width())/2-1;

    int y_Logo = (cy - ui.labelLogo->height())/2;
    if (bHScreen)
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

    int x_LogoCompany = cx - 50 - ui.labelLogoCompany->width();
    int y_LogoCompany = cy - 50 - ui.labelLogoCompany->height();
    ui.labelLogoCompany->move(x_LogoCompany, y_LogoCompany);

    static const QPixmap bkgPixmapPrev = *ui.labelBkg->pixmap();
    float fCXRate = 0;
    if (bHScreen)
    {
        fCXRate = (float)cx/bkgPixmapPrev.width();
    }
    else
    {
        fCXRate = (float)cx/1080;
    }
    int cy_bkg = fCXRate*bkgPixmapPrev.height();
    int dy_bkg = cy - cy_bkg;

    QPixmap *pBkgPixmap = NULL;
    if (bHScreen)
    {
        if (!m_HBkgPixmap.isNull())
        {
            pBkgPixmap = &m_HBkgPixmap;
        }
    }
    else
    {
        if (!m_VBkgPixmap.isNull())
        {
            pBkgPixmap = &m_VBkgPixmap;
        }
    }
    if (NULL != pBkgPixmap)
    {
        m_bUsingCustomBkg = true;

        ui.labelBkg->setPixmap(*pBkgPixmap);

        float fHWRate = (float)pBkgPixmap->height()/pBkgPixmap->width();
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

        ui.labelBkg->setPixmap(bkgPixmapPrev);

        if (bHScreen)
        {
            ui.labelBkg->resize(cx, cy_bkg);
        }
        else
        {
            ui.labelBkg->resize(fCXRate*bkgPixmapPrev.width(), cy_bkg);
        }
        ui.labelBkg->move(0, dy_bkg);
    }

    for (cauto& widgetPos : m_mapWidgetPos)
    {
        cauto& pos = widgetPos.second;
        auto& newPos = m_mapWidgetNewPos[widgetPos.first];

        QPushButton *btn = dynamic_cast<QPushButton*>(widgetPos.first);
        if (NULL != btn)
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
    if (cy < 1000 && !m_bUsingCustomBkg)
    {
        y_frameDemand = 12;
    }

    int x_btnExit = cx - ui.btnExit->width() - (y_frameDemand + 10);
    ui.btnExit->move(x_btnExit, y_frameDemand + 10);

    int x_frameDemand = 0;
    if (bHScreen)
    {
        x_frameDemand = (ui.progressBar->geometry().right()
            + ui.progressBar->x() - ui.frameDemand->width())/2;

    }
    else
    {
#ifdef __ANDROID__
        x_frameDemand = (cx - ui.frameDemand->width())/2;
#else
        x_frameDemand = (x_btnExit - ui.frameDemand->width())/2;
#endif
    }
    x_frameDemand-=11;
    ui.frameDemand->move(x_frameDemand, y_frameDemand);

//    QAbstractButton* lpButton[] = {
//        ui.btnDemandSinger, ui.btnDemandAlbum, ui.btnDemandAlbumItem
//        , ui.btnDemandPlayItem, ui.btnDemandPlaylist};
//    UINT uButtonCount = sizeof(lpButton)/sizeof(lpButton[0]);
//    int nButtonSize = ui.frameDemand->height();
//    int cx_offset = (ui.frameDemand->width() - uButtonCount*nButtonSize) / (uButtonCount-1);
//    for (UINT uIdx = 0; uIdx < uButtonCount; uIdx++)
//    {
//        lpButton[uIdx]->setGeometry(uIdx * (cx_offset + nButtonSize), 0, nButtonSize, nButtonSize);
//    }

    int y_frameDemandBottom = ui.frameDemand->geometry().bottom();

    int y_PlayingListMax = 0;


    bool bFlag = bHScreen && fCXRate>1 && cy <= 1080;

    if (m_bUsingCustomBkg)
    {
        int x = ui.progressBar->x();
        int cy_Playingfile = ui.labelPlayingfile->height();
        int y_Playingfile = ui.labelDuration->geometry().bottom() -  cy_Playingfile;
        ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x() - x, cy_Playingfile);
        ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);

        int cy_AlbumName = 60;//ui.labelAlbumName->height();
        int y_AlbumName = y_Playingfile - cy_AlbumName;
        int cx_progressBar = ui.progressBar->width();
        ui.labelAlbumName->setGeometry(x, y_AlbumName, cx_progressBar, cy_AlbumName);

        ui.labelSingerName->setGeometry(x, y_AlbumName-ui.labelSingerName->height()
                                         , cx_progressBar, ui.labelSingerName->height());

        int y_SingerImg = 0;
        if (bHScreen)
        {
            y_SingerImg = ui.frameDemandLanguage->geometry().bottom() + 60;
        }
        else
        {
            y_SingerImg = y_AlbumName - 500;

            y_PlayingListMax = y_SingerImg;
        }
        int cy_SingerImg = y_AlbumName-y_SingerImg;
        cauto& rcSingerImg = m_mapWidgetPos[ui.wdgSingerImg];
        int cx_SingerImg = rcSingerImg.width()*cy_SingerImg/rcSingerImg.height();
        ui.wdgSingerImg->setGeometry(x + (cx_progressBar-cx_SingerImg)/2, y_SingerImg, cx_SingerImg, cy_SingerImg);


        m_PlayingList.setFont(m_view.genFont(-1), QColor(255, 255, 255, 255));
    }
    else
    {
        if (bFlag)
        {
            int x_Playingfile = ui.wdgSingerImg->x() + 30;
            int y_Playingfile = ui.wdgSingerImg->geometry().bottom()- ui.labelPlayingfile->height() - 30;
            ui.labelPlayingfile->move(x_Playingfile, y_Playingfile);

            ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignBottom);
        }
        else
        {
            ui.labelPlayingfile->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignBottom);
        }

        y_PlayingListMax = ui.labelPlayingfile->y();

        m_PlayingList.setFont(m_view.genFont(-1.5), QColor(255, 255, 255, 160));
    }

    if (ui.frameSingerImg->isVisible())
    {
        auto pPixmap = ui.labelSingerImg->pixmap();
        if (NULL != pPixmap && !pPixmap->isNull())
        {
            _showSingerImg(*pPixmap);
        }
    }

    UINT uMinRowHeight = 95;
    if (bHScreen)
    {
        UINT uMargin = 45;
        int x_PlayingList = ui.progressBar->geometry().right();
        x_PlayingList += 100 * fCXRate;

        m_PlayingList.setGeometry(x_PlayingList, uMargin-1, cx-x_PlayingList-uMargin, cy-uMargin*2);

        if (cy > 1080)
        {
            uMinRowHeight = 105;
        }
     }
    else
    {
        UINT y_Margin = 30;

        int nRows = (y_PlayingListMax - y_frameDemandBottom)/uMinRowHeight;
        if (nRows > 10)
        {
            uMinRowHeight += 10;
            y_Margin += 10;
        }
        else if (nRows < 7)
        {
            uMinRowHeight -= 10;
            y_Margin -= 10;
        }

        UINT x_Margin = ui.frameDemand->x();
        int y_PlayingList = y_frameDemandBottom + y_Margin;
        int cy_PlayingList = y_PlayingListMax - y_Margin - y_PlayingList;
        m_PlayingList.setGeometry(x_Margin, y_PlayingList, cx-x_Margin*2, cy_PlayingList);

        if (cy_PlayingList < 900)
        {
            uMinRowHeight = 85;
        }
        else if (cy_PlayingList < 1080)
        {
            uMinRowHeight = 90;
        }
    }
    m_PlayingList.setMinRowHeight(uMinRowHeight);

    _showAlbumName(m_PlayingItem);
}

void MainWindow::_updatePlayPauseButton(bool bPlaying)
{
    ui.btnPlay->setVisible(!bPlaying);
    ui.btnPause->setVisible(bPlaying);
}

void MainWindow::refreshPlayingList(int nPlayingItem, bool bSetActive)
{
    (void)bSetActive;

    cauto& PlayingItems = m_view.getPlayMgr().getPlayingItems();
    PairList<UINT, QString> plTitle;
    PlayingItems([&](const CPlayItem& PlayItem){
        plTitle.add(PlayItem.m_uID, wsutil::toQStr(PlayItem.GetTitle()));
    });
    m_PlayingList.updateList(plTitle, nPlayingItem);
}

void MainWindow::onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual)
{
    PlayItem.AsyncTask();

    g_mtxPlayingItem.lock([&](tagPlayingItem& PlayingItem){
        PlayingItem.strTitle = PlayItem.GetTitle();

        PlayingItem.strSinger = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Singer);
        PlayingItem.strAlbum = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Album);

        PlayingItem.strPlaylist = PlayItem.GetRelatedMediaSetName(E_MediaSetType::MST_Playlist);

        PlayingItem.nDuration = PlayItem.GetDuration();
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

    g_mtxPlayingItem.get(m_PlayingItem);
    _showAlbumName(m_PlayingItem);

    ui.labelPlayingfile->setText(wsutil::toQStr(m_PlayingItem.strTitle));
    UINT uLen = wsutil::toStr(m_PlayingItem.strTitle).size();
    float fFontSizeOffset = 0;
    if (uLen > 60)
    {
        fFontSizeOffset = -2.5;
    }
    else if (uLen > 55)
    {
        fFontSizeOffset = -2;
    }
    else if (uLen > 50)
    {
        fFontSizeOffset = -1;
    }
    else if (uLen > 45)
    {
        fFontSizeOffset = -0.5;
    }
    else if (uLen > 40)
    {
        fFontSizeOffset = 0;
    }
    else
    {
        if (!m_bUsingCustomBkg)
        {
            fFontSizeOffset -= 0.5;
        }
        else
        {
            fFontSizeOffset = 0.5;
        }
    }
    m_view.setFont(ui.labelPlayingfile, fFontSizeOffset);

    if (m_PlayingItem.strSinger != m_strSingerName)
    {
        m_strSingerName = m_PlayingItem.strSinger;

        ui.labelSingerName->setText(wsutil::toQStr(m_strSingerName));

        ui.frameSingerImg->setVisible(false);

        if (m_strSingerName.empty())
        {
            ui.wdgSingerImg->setVisible(false);
        }
        else
        {
            ui.wdgSingerImg->setVisible(true);

            _playSingerImg(true);
        }
    }

    ui.progressBar->setValue(0);

    if (m_PlayingItem.nDuration > 0)
    {
        ui.progressBar->setMaximum(m_PlayingItem.nDuration);
        QString qsDuration = wsutil::toQStr(CMedia::GetDurationString(m_PlayingItem.nDuration));

        ui.labelDuration->setText(qsDuration);

        _updatePlayPauseButton(true);
    }
    else
    {
        ui.labelDuration->clear();

        _updatePlayPauseButton(false);
    }
}

void MainWindow::_showAlbumName(tagPlayingItem& PlayingItem)
{
    WString strMediaSet;
    if (E_DemandMode::DM_DemandPlaylist == m_eDemandMode || E_DemandMode::DM_DemandPlayItem == m_eDemandMode
            || PlayingItem.strAlbum.empty())
    {
        if (!PlayingItem.strPlaylist.empty())
        {
            if (m_bUsingCustomBkg)
            {
                strMediaSet << L"列表：";
            }
            strMediaSet << PlayingItem.strPlaylist;
        }
    }
    else if (!PlayingItem.strAlbum.empty())
    {
        if (m_bUsingCustomBkg)
        {
            strMediaSet << L"专辑：";
        }
        strMediaSet << PlayingItem.strAlbum;
    }

    QLabel& labelAlbumName = *ui.labelAlbumName;
    labelAlbumName.setText(wsutil::toQStr(strMediaSet));

    if (!m_bUsingCustomBkg)
    {
        uint uLen = wsutil::toStr(*strMediaSet).size();
        float fFontSizeOffset = 0;
        if (uLen > 20)
        {
            fFontSizeOffset = -1.5;
        }
        else if (uLen > 15)
        {
            fFontSizeOffset = -1;
        }
        else if (uLen > 10)
        {
            fFontSizeOffset = -0.5;
        }
        m_view.setFont(&labelAlbumName, fFontSizeOffset);

        cauto& rcAlbumNamePrev = m_mapWidgetNewPos[&labelAlbumName];
        labelAlbumName.adjustSize();
        if (labelAlbumName.width() < rcAlbumNamePrev.width())
        {
            labelAlbumName.move(rcAlbumNamePrev.left() + (rcAlbumNamePrev.width()-labelAlbumName.width())/2, labelAlbumName.y());
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
        QPixmap pixmap;
        if (pixmap.load(wsutil::toQStr(strSingerImg)))
        {
            _showSingerImg(pixmap);
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

void MainWindow::_showSingerImg(const QPixmap& pixmap)
{
    int cx_target = ui.wdgSingerImg->width();
    int cy_target = ui.wdgSingerImg->height();
    float fWHRate = (float)pixmap.width()/pixmap.height();
    if (fWHRate > (float)cx_target/cy_target)
    {
        int width = cy_target*fWHRate;
        ui.labelSingerImg->setGeometry((cx_target-width)/2, 0, width, cy_target);
    }
    else
    {
        int height = cx_target/fWHRate;
        ui.labelSingerImg->setGeometry(0, (cy_target-height)/2, cx_target, height);
    }
    ui.labelSingerImg->setPixmap(pixmap);

    ui.frameSingerImg->setVisible(true);

    UINT uMargin = 0;
    if (m_bUsingCustomBkg)
    {
        uMargin = 2;
    }
    ui.frameSingerImg->setGeometry(uMargin, uMargin
        , ui.wdgSingerImg->width() - uMargin*2
        , ui.wdgSingerImg->height() - uMargin*2);
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
        auto& bRandomPlay = m_view.getDataMgr().getOption().bRandomPlay;
        bRandomPlay = !bRandomPlay;

        ui.btnRandom->setVisible(bRandomPlay);
        ui.btnOrder->setVisible(!bRandomPlay);
    }
    else if (button == ui.btnSetting)
    {
        bool bHScreen = this->width() > this->height(); // 橫屏
        cauto& strBkgDir = bHScreen?m_strHBkgDir:m_strVBkgDir;
        const list<wstring>& lstBkg = bHScreen?m_lstHBkg:m_lstVBkg;
        auto& strCurrBkg = bHScreen? m_view.getDataMgr().getOption().strHBkg : m_view.getDataMgr().getOption().strVBkg;
        QPixmap& bkgPixmap = bHScreen? m_HBkgPixmap:m_VBkgPixmap;

        for (cauto& strBkg : lstBkg)
        {
            if (strCurrBkg.empty())
            {
                strCurrBkg = strBkg;

                (void)bkgPixmap.load(strBkgDir + strCurrBkg);

                break;
            }

            if (strBkg == strCurrBkg)
            {
                strCurrBkg.clear();
                bkgPixmap = QPixmap();
            }
        }

        _relayout();
    }
    else
    {
        _demand(button);
    }
}

static const QString __qsCheck = wsutil::toQStr(L"√");

void MainWindow::slot_labelClicked(CLabel* label)
{
    m_eDemandLanguage = E_LanguageType::LT_None;

    PairList<E_LanguageType, QLabel*> plLabels {{E_LanguageType::LT_CN, ui.labelDemandCN}
        , {E_LanguageType::LT_HK, ui.labelDemandHK}
        , {E_LanguageType::LT_KR, ui.labelDemandKR}
        , {E_LanguageType::LT_JP, ui.labelDemandJP}
        , {E_LanguageType::LT_TAI, ui.labelDemandTAI}
        , {E_LanguageType::LT_EN, ui.labelDemandEN}
        , {E_LanguageType::LT_EUR, ui.labelDemandEUR}};
    plLabels([&](E_LanguageType eLanguage, QLabel* lblLanguage){
        if (!lblLanguage->text().startsWith(' '))
        {
            lblLanguage->setText("  " + lblLanguage->text().mid(1));
        }
        else
        {
            if (lblLanguage == label)
            {
                lblLanguage->setText(__qsCheck + lblLanguage->text().mid(2));
                m_eDemandLanguage = eLanguage;
            }
        }
    });
}

void MainWindow::_demand(CButton* btnDemand)
{
    if (btnDemand == ui.btnDemandSinger)
    {
        m_eDemandMode = E_DemandMode::DM_DemandSinger;
    }
    else if (btnDemand == ui.btnDemandAlbum)
    {
        m_eDemandMode = E_DemandMode::DM_DemandAlbum;
    }
    else if (btnDemand == ui.btnDemandAlbumItem)
    {
        m_eDemandMode = E_DemandMode::DM_DemandAlbumItem;
    }
    else if (btnDemand == ui.btnDemandPlayItem)
    {
        m_eDemandMode = E_DemandMode::DM_DemandPlayItem;
    }
    else if (btnDemand == ui.btnDemandPlaylist)
    {
        m_eDemandMode = E_DemandMode::DM_DemandPlaylist;
    }
    else
    {
        return;
    }

    m_view.getCtrl().callPlayCtrl(tagPlayCtrl(m_eDemandMode, m_eDemandLanguage));
}

void MainWindow::slot_progressMousePress(CLabel* label, const QPoint& pos)
{
    if (label == ui.labelPlayProgress)
    {
        if (E_PlayStatus::PS_Play == m_view.getPlayMgr().GetPlayStatus())
        {
            if (ui.progressBar->maximum() > 0)
            {
                UINT uPos = pos.x() * ui.progressBar->maximum() /ui.progressBar->width();
                m_view.getPlayMgr().getPlayer().Seek(uPos);

                ui.progressBar->setValue(uPos);
                mtutil::yield();
            }
        }
    }
}
