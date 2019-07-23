/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.4.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>
#include <button.h>
#include <label.h>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *action;
    QWidget *centralWidget;
    QLabel *labelBkg;
    QProgressBar *progressBar;
    CButton *btnPlay;
    CButton *btnPause;
    CButton *btnPlayPrev;
    CButton *btnPlayNext;
    QLabel *labelAlbumName;
    QLabel *labelDuration;
    QLabel *labelSingerName;
    CButton *btnOrder;
    CButton *btnRandom;
    CLabel *labelPlayProgress;
    QLabel *labelPlayingfile;
    CButton *btnSetting;
    QWidget *wdgSingerImg;
    QFrame *frameSingerImg;
    QLabel *labelSingerImg;
    QFrame *frameDemand;
    CButton *btnDemandAlbum;
    CButton *btnDemandAlbumItem;
    CButton *btnDemandPlaylist;
    CButton *btnDemandSinger;
    CButton *btnDemandPlayItem;
    QFrame *frameDemandLanguage;
    CLabel *labelDemandCN;
    CLabel *labelDemandHK;
    CLabel *labelDemandKR;
    CLabel *labelDemandJP;
    CLabel *labelDemandTAI;
    CLabel *labelDemandEN;
    CLabel *labelDemandEUR;
    QLabel *labelLogo;
    QLabel *labelLogoCompany;
    QLabel *labelLogoTip;
    CButton *btnExit;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1920, 2340);
        MainWindow->setStyleSheet(QStringLiteral(""));
        action = new QAction(MainWindow);
        action->setObjectName(QStringLiteral("action"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        labelBkg = new QLabel(centralWidget);
        labelBkg->setObjectName(QStringLiteral("labelBkg"));
        labelBkg->setGeometry(QRect(0, 0, 1920, 2340));
        labelBkg->setPixmap(QPixmap(QString::fromUtf8(":/img/bkg.jpg")));
        labelBkg->setScaledContents(true);
        progressBar = new QProgressBar(centralWidget);
        progressBar->setObjectName(QStringLiteral("progressBar"));
        progressBar->setGeometry(QRect(40, 2170, 990, 6));
        progressBar->setStyleSheet(QLatin1String("color: rgb(200, 200, 255);\n"
"background-color: rgb(255, 255, 255);\n"
"border-color: rgb(255, 255, 255);\n"
"gridline-color: rgb(255, 255, 255);"));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        btnPlay = new CButton(centralWidget);
        btnPlay->setObjectName(QStringLiteral("btnPlay"));
        btnPlay->setGeometry(QRect(480, 2200, 110, 110));
        btnPlay->setAutoFillBackground(false);
        btnPlay->setStyleSheet(QStringLiteral("border-image: url(:/img/btnPlay.png);"));
        btnPlay->setIconSize(QSize(120, 120));
        btnPlay->setFlat(false);
        btnPause = new CButton(centralWidget);
        btnPause->setObjectName(QStringLiteral("btnPause"));
        btnPause->setGeometry(QRect(480, 2200, 110, 110));
        btnPause->setAutoFillBackground(false);
        btnPause->setStyleSheet(QStringLiteral("border-image: url(:/img/btnPause.png);"));
        btnPause->setIconSize(QSize(120, 120));
        btnPause->setFlat(false);
        btnPlayPrev = new CButton(centralWidget);
        btnPlayPrev->setObjectName(QStringLiteral("btnPlayPrev"));
        btnPlayPrev->setGeometry(QRect(310, 2210, 90, 90));
        btnPlayPrev->setAutoFillBackground(false);
        btnPlayPrev->setStyleSheet(QStringLiteral("border-image: url(:/img/btnPlayPrev.png);"));
        btnPlayPrev->setIconSize(QSize(100, 100));
        btnPlayPrev->setFlat(false);
        btnPlayNext = new CButton(centralWidget);
        btnPlayNext->setObjectName(QStringLiteral("btnPlayNext"));
        btnPlayNext->setGeometry(QRect(670, 2210, 90, 90));
        btnPlayNext->setAutoFillBackground(false);
        btnPlayNext->setStyleSheet(QStringLiteral("border-image: url(:/img/btnPlayNext.png);"));
        btnPlayNext->setIconSize(QSize(100, 100));
        btnPlayNext->setFlat(false);
        labelAlbumName = new QLabel(centralWidget);
        labelAlbumName->setObjectName(QStringLiteral("labelAlbumName"));
        labelAlbumName->setGeometry(QRect(670, 1680, 200, 70));
        labelAlbumName->setScaledContents(false);
        labelAlbumName->setAlignment(Qt::AlignCenter);
        labelAlbumName->setWordWrap(false);
        labelDuration = new QLabel(centralWidget);
        labelDuration->setObjectName(QStringLiteral("labelDuration"));
        labelDuration->setGeometry(QRect(930, 2110, 100, 50));
        labelDuration->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);
        labelSingerName = new QLabel(centralWidget);
        labelSingerName->setObjectName(QStringLiteral("labelSingerName"));
        labelSingerName->setGeometry(QRect(54, 2070, 600, 70));
        labelSingerName->setAlignment(Qt::AlignCenter);
        btnOrder = new CButton(centralWidget);
        btnOrder->setObjectName(QStringLiteral("btnOrder"));
        btnOrder->setGeometry(QRect(975, 2225, 60, 60));
        btnOrder->setAutoFillBackground(false);
        btnOrder->setStyleSheet(QStringLiteral("border-image: url(:/img/btnOrder.png);"));
        btnOrder->setFlat(false);
        btnRandom = new CButton(centralWidget);
        btnRandom->setObjectName(QStringLiteral("btnRandom"));
        btnRandom->setGeometry(QRect(975, 2225, 60, 60));
        btnRandom->setAutoFillBackground(false);
        btnRandom->setStyleSheet(QStringLiteral("border-image: url(:/img/btnRandom.png);"));
        btnRandom->setFlat(false);
        labelPlayProgress = new CLabel(centralWidget);
        labelPlayProgress->setObjectName(QStringLiteral("labelPlayProgress"));
        labelPlayProgress->setGeometry(QRect(40, 2158, 990, 30));
        labelPlayProgress->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        labelPlayProgress->setWordWrap(true);
        labelPlayingfile = new QLabel(centralWidget);
        labelPlayingfile->setObjectName(QStringLiteral("labelPlayingfile"));
        labelPlayingfile->setGeometry(QRect(40, 1495, 1000, 46));
        labelPlayingfile->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        labelPlayingfile->setWordWrap(false);
        btnSetting = new CButton(centralWidget);
        btnSetting->setObjectName(QStringLiteral("btnSetting"));
        btnSetting->setGeometry(QRect(40, 2225, 60, 60));
        btnSetting->setAutoFillBackground(false);
        btnSetting->setStyleSheet(QStringLiteral("border-image: url(:/img/btnSetting.png);"));
        btnSetting->setFlat(false);
        wdgSingerImg = new QWidget(centralWidget);
        wdgSingerImg->setObjectName(QStringLiteral("wdgSingerImg"));
        wdgSingerImg->setGeometry(QRect(41, 1563, 628, 577));
        wdgSingerImg->setAutoFillBackground(false);
        wdgSingerImg->setStyleSheet(QLatin1String("border-image: url(:/img/defaultSingerImg.jpg);\n"
"border-image-repeat: stretched"));
        frameSingerImg = new QFrame(wdgSingerImg);
        frameSingerImg->setObjectName(QStringLiteral("frameSingerImg"));
        frameSingerImg->setGeometry(QRect(160, 80, 391, 321));
        frameSingerImg->setStyleSheet(QStringLiteral(""));
        frameSingerImg->setFrameShape(QFrame::NoFrame);
        frameSingerImg->setFrameShadow(QFrame::Plain);
        frameSingerImg->setLineWidth(0);
        labelSingerImg = new QLabel(frameSingerImg);
        labelSingerImg->setObjectName(QStringLiteral("labelSingerImg"));
        labelSingerImg->setGeometry(QRect(140, 40, 211, 181));
        labelSingerImg->setScaledContents(true);
        labelSingerImg->setWordWrap(false);
        frameDemand = new QFrame(centralWidget);
        frameDemand->setObjectName(QStringLiteral("frameDemand"));
        frameDemand->setGeometry(QRect(0, 0, 800, 160));
        frameDemand->setFrameShape(QFrame::NoFrame);
        frameDemand->setFrameShadow(QFrame::Raised);
        btnDemandAlbum = new CButton(frameDemand);
        btnDemandAlbum->setObjectName(QStringLiteral("btnDemandAlbum"));
        btnDemandAlbum->setGeometry(QRect(180, 0, 110, 110));
        QFont font;
        font.setPointSize(16);
        font.setBold(false);
        font.setWeight(50);
        btnDemandAlbum->setFont(font);
        btnDemandAlbum->setAutoFillBackground(false);
        btnDemandAlbum->setStyleSheet(QStringLiteral("border-image: url(:/img/album.png);"));
        btnDemandAlbum->setIconSize(QSize(130, 130));
        btnDemandAlbum->setFlat(false);
        btnDemandAlbumItem = new CButton(frameDemand);
        btnDemandAlbumItem->setObjectName(QStringLiteral("btnDemandAlbumItem"));
        btnDemandAlbumItem->setGeometry(QRect(350, 0, 110, 110));
        btnDemandAlbumItem->setFont(font);
        btnDemandAlbumItem->setAutoFillBackground(false);
        btnDemandAlbumItem->setStyleSheet(QStringLiteral("border-image: url(:/img/albumitem.png);"));
        btnDemandAlbumItem->setIconSize(QSize(130, 130));
        btnDemandAlbumItem->setFlat(false);
        btnDemandPlaylist = new CButton(frameDemand);
        btnDemandPlaylist->setObjectName(QStringLiteral("btnDemandPlaylist"));
        btnDemandPlaylist->setGeometry(QRect(690, 0, 110, 110));
        btnDemandPlaylist->setFont(font);
        btnDemandPlaylist->setAutoFillBackground(false);
        btnDemandPlaylist->setStyleSheet(QStringLiteral("border-image: url(:/img/playlist.png);"));
        btnDemandPlaylist->setIconSize(QSize(130, 130));
        btnDemandPlaylist->setFlat(false);
        btnDemandSinger = new CButton(frameDemand);
        btnDemandSinger->setObjectName(QStringLiteral("btnDemandSinger"));
        btnDemandSinger->setGeometry(QRect(10, 0, 110, 110));
        btnDemandSinger->setFont(font);
        btnDemandSinger->setAutoFillBackground(false);
        btnDemandSinger->setStyleSheet(QStringLiteral("border-image: url(:/img/singerdefault.png);"));
        btnDemandSinger->setIconSize(QSize(130, 130));
        btnDemandSinger->setFlat(false);
        btnDemandPlayItem = new CButton(frameDemand);
        btnDemandPlayItem->setObjectName(QStringLiteral("btnDemandPlayItem"));
        btnDemandPlayItem->setGeometry(QRect(520, 0, 110, 110));
        btnDemandPlayItem->setFont(font);
        btnDemandPlayItem->setAutoFillBackground(false);
        btnDemandPlayItem->setStyleSheet(QStringLiteral("border-image: url(:/img/playitem.png);"));
        btnDemandPlayItem->setIconSize(QSize(130, 130));
        btnDemandPlayItem->setFlat(false);
        frameDemandLanguage = new QFrame(frameDemand);
        frameDemandLanguage->setObjectName(QStringLiteral("frameDemandLanguage"));
        frameDemandLanguage->setGeometry(QRect(0, 120, 990, 50));
        frameDemandLanguage->setFrameShape(QFrame::NoFrame);
        frameDemandLanguage->setFrameShadow(QFrame::Raised);
        labelDemandCN = new CLabel(frameDemandLanguage);
        labelDemandCN->setObjectName(QStringLiteral("labelDemandCN"));
        labelDemandCN->setGeometry(QRect(0, 0, 110, 50));
        labelDemandCN->setAlignment(Qt::AlignCenter);
        labelDemandCN->setWordWrap(true);
        labelDemandHK = new CLabel(frameDemandLanguage);
        labelDemandHK->setObjectName(QStringLiteral("labelDemandHK"));
        labelDemandHK->setGeometry(QRect(110, 0, 110, 50));
        labelDemandHK->setAlignment(Qt::AlignCenter);
        labelDemandHK->setWordWrap(true);
        labelDemandKR = new CLabel(frameDemandLanguage);
        labelDemandKR->setObjectName(QStringLiteral("labelDemandKR"));
        labelDemandKR->setGeometry(QRect(220, 0, 110, 50));
        labelDemandKR->setAlignment(Qt::AlignCenter);
        labelDemandKR->setWordWrap(true);
        labelDemandJP = new CLabel(frameDemandLanguage);
        labelDemandJP->setObjectName(QStringLiteral("labelDemandJP"));
        labelDemandJP->setGeometry(QRect(330, 0, 110, 50));
        labelDemandJP->setAlignment(Qt::AlignCenter);
        labelDemandJP->setWordWrap(true);
        labelDemandTAI = new CLabel(frameDemandLanguage);
        labelDemandTAI->setObjectName(QStringLiteral("labelDemandTAI"));
        labelDemandTAI->setGeometry(QRect(440, 0, 110, 50));
        labelDemandTAI->setAlignment(Qt::AlignCenter);
        labelDemandTAI->setWordWrap(true);
        labelDemandEN = new CLabel(frameDemandLanguage);
        labelDemandEN->setObjectName(QStringLiteral("labelDemandEN"));
        labelDemandEN->setGeometry(QRect(550, 0, 110, 50));
        labelDemandEN->setAlignment(Qt::AlignCenter);
        labelDemandEN->setWordWrap(true);
        labelDemandEUR = new CLabel(frameDemandLanguage);
        labelDemandEUR->setObjectName(QStringLiteral("labelDemandEUR"));
        labelDemandEUR->setGeometry(QRect(660, 0, 140, 50));
        labelDemandEUR->setAlignment(Qt::AlignCenter);
        labelDemandEUR->setWordWrap(true);
        labelLogo = new QLabel(centralWidget);
        labelLogo->setObjectName(QStringLiteral("labelLogo"));
        labelLogo->setGeometry(QRect(120, 230, 520, 100));
        QFont font1;
        font1.setPointSize(48);
        labelLogo->setFont(font1);
        labelLogo->setStyleSheet(QStringLiteral(""));
        labelLogo->setAlignment(Qt::AlignCenter);
        labelLogoCompany = new QLabel(centralWidget);
        labelLogoCompany->setObjectName(QStringLiteral("labelLogoCompany"));
        labelLogoCompany->setGeometry(QRect(670, 270, 281, 60));
        QFont font2;
        font2.setPointSize(20);
        labelLogoCompany->setFont(font2);
        labelLogoCompany->setStyleSheet(QStringLiteral(""));
        labelLogoCompany->setAlignment(Qt::AlignBottom|Qt::AlignRight|Qt::AlignTrailing);
        labelLogoTip = new QLabel(centralWidget);
        labelLogoTip->setObjectName(QStringLiteral("labelLogoTip"));
        labelLogoTip->setGeometry(QRect(140, 360, 491, 60));
        labelLogoTip->setFont(font2);
        labelLogoTip->setStyleSheet(QStringLiteral(""));
        labelLogoTip->setAlignment(Qt::AlignBottom|Qt::AlignHCenter);
        btnExit = new CButton(centralWidget);
        btnExit->setObjectName(QStringLiteral("btnExit"));
        btnExit->setGeometry(QRect(860, 10, 90, 90));
        btnExit->setAutoFillBackground(false);
        btnExit->setStyleSheet(QStringLiteral("border-image: url(:/img/btnExit.png);"));
        btnExit->setIconSize(QSize(100, 100));
        btnExit->setFlat(false);
        MainWindow->setCentralWidget(centralWidget);
        labelBkg->raise();
        progressBar->raise();
        btnPlay->raise();
        btnPause->raise();
        btnPlayPrev->raise();
        btnPlayNext->raise();
        labelAlbumName->raise();
        labelDuration->raise();
        btnOrder->raise();
        btnRandom->raise();
        labelPlayProgress->raise();
        labelPlayingfile->raise();
        btnSetting->raise();
        wdgSingerImg->raise();
        labelSingerName->raise();
        frameDemand->raise();
        labelLogo->raise();
        labelLogoCompany->raise();
        labelLogoTip->raise();
        btnExit->raise();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "XMusic", 0));
        action->setText(QApplication::translate("MainWindow", "\344\270\223\350\276\221", 0));
        labelBkg->setText(QString());
        progressBar->setFormat(QString());
        btnPlay->setText(QString());
        btnPause->setText(QString());
        btnPlayPrev->setText(QString());
        btnPlayNext->setText(QString());
        labelAlbumName->setText(QString());
        labelDuration->setText(QString());
        labelSingerName->setText(QString());
        btnOrder->setText(QString());
        btnRandom->setText(QString());
        labelPlayProgress->setText(QString());
        labelPlayingfile->setText(QString());
        btnSetting->setText(QString());
        labelSingerImg->setText(QString());
        btnDemandAlbum->setText(QString());
        btnDemandAlbumItem->setText(QString());
        btnDemandPlaylist->setText(QString());
        btnDemandSinger->setText(QString());
        btnDemandPlayItem->setText(QString());
        labelDemandCN->setText(QApplication::translate("MainWindow", "  \345\233\275\350\257\255", 0));
        labelDemandHK->setText(QApplication::translate("MainWindow", "  \347\262\244\350\257\255", 0));
        labelDemandKR->setText(QApplication::translate("MainWindow", "  \351\237\251\350\257\255", 0));
        labelDemandJP->setText(QApplication::translate("MainWindow", "  \346\227\245\350\257\255", 0));
        labelDemandTAI->setText(QApplication::translate("MainWindow", "  \346\263\260\350\257\255", 0));
        labelDemandEN->setText(QApplication::translate("MainWindow", "  \350\213\261\346\226\207", 0));
        labelDemandEUR->setText(QApplication::translate("MainWindow", "  \345\260\217\350\257\255\347\247\215", 0));
        labelLogo->setText(QApplication::translate("MainWindow", "X Music", 0));
        labelLogoCompany->setText(QApplication::translate("MainWindow", "@MusicroSoft", 0));
        labelLogoTip->setText(QString());
        btnExit->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
