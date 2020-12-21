
#include "xmusic.h"

#include "centralwidget.h"

#include "ui_mainwindow.h"
extern Ui::MainWindow ui;

void CCentralWidget::ctor() // 代替构造函数
{
    setAttribute(Qt::WA_TranslucentBackground);

    auto cy = __cyBkg - ui.labelSingerImg->y() + ui.labelSingerImg->x();
    m_fBkgHWRate = (Double_T)cy/__cxBkg;
    auto cyTop = ui.labelSingerImg->x()*2 + ui.frameDemand->height();
    m_fBkgTopReserve = (Double_T)cyTop/(cyTop+cy);

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
}

Double_T CCentralWidget::caleBkgZoomRate(Double_T& cxDst, Double_T cyDst, Double_T& xDst)
{
    if (cxDst > cyDst)
    {
        cxDst = cxDst*9/16;
    }
    else
    {
        cyDst = cyDst*9/16;
    }
    cyDst -= cyDst * m_fBkgTopReserve;

    auto dx = cxDst - cyDst/m_fBkgHWRate;
    if (dx > 0)
    {
        xDst = dx/2;
        cxDst -= dx;
    }
    else
    {
        xDst = 0;
    }

    return (Double_T)cxDst / __cxBkg;
}

void CCentralWidget::relayout(int cx, int cy, bool bDefaultBkg, E_SingerImgPos t_eSingerImgPos
                              , const tagPlayingInfo& PlayingInfo, CPlayingList& PlayingList)
{
    setGeometry(0, 0, cx, cy); //for iPhoneX

    bool bHLayout = cx > cy; // 橫屏

    int x_Logo = (cx - ui.labelLogo->width())/2;
    int y_Logo = (cy - ui.labelLogo->height())/2;
    if (bHLayout)
    {
        y_Logo -= __size(60);
    }
    else
    {
        y_Logo -= __size100;
    }
    ui.labelLogo->move(x_Logo, y_Logo);

    int y_LogoTip = ui.labelLogo->geometry().bottom() + __size(30);
    ui.labelLogoTip->setGeometry(0, y_LogoTip, cx, ui.labelLogoTip->height());

#define __LogoCompanyMargin __size(50)
    int y_LogoCompany = cy - __LogoCompanyMargin - ui.labelLogoCompany->height();
    ui.labelLogoCompany->setGeometry(__LogoCompanyMargin, y_LogoCompany, cx-__LogoCompanyMargin*2, ui.labelLogoCompany->height());
    if (bHLayout)
    {
        ui.labelLogoCompany->setAlignment(Qt::AlignmentFlag::AlignBottom | Qt::AlignmentFlag::AlignRight);
    }
    else
    {
        ui.labelLogoCompany->setAlignment(Qt::AlignmentFlag::AlignBottom | Qt::AlignmentFlag::AlignHCenter);
    }

    Double_T cxDst = cx;
    Double_T xBkgOffset = 0;
    auto fBkgZoomRate = caleBkgZoomRate(cxDst, cy, xBkgOffset);
    auto fBkgZoomRateEx = fBkgZoomRate * g_screen.pixelRatio;

    int cy_bkg = fBkgZoomRate * __cyBkg; //round(fBkgZoomRate * __cyBkg);
    int dy_bkg = cy - cy_bkg;

    UINT uShadowWidth = bDefaultBkg?0:1;
    ui.labelDemandHK->setShadow(uShadowWidth);
    ui.labelDemandCN->setShadow(uShadowWidth);
    ui.labelDemandKR->setShadow(uShadowWidth);
    ui.labelDemandJP->setShadow(uShadowWidth);
    ui.labelDemandEN->setShadow(uShadowWidth);
    ui.labelDemandEUR->setShadow(uShadowWidth);

    for (cauto widgetPos : m_mapTopWidgetPos)
    {
        QRect pos = widgetPos.second;
        if (fBkgZoomRateEx < 1)
        {
            pos.setRect(fBkgZoomRate*pos.left(), fBkgZoomRate*pos.top()
                        , fBkgZoomRate*pos.width(), fBkgZoomRate*pos.height());
        }
        widgetPos.first->setGeometry(pos);
    }

    for (cauto widgetPos : m_mapWidgetPos)
    {
        cauto pos = widgetPos.second;
        auto& newPos = m_mapWidgetNewPos[widgetPos.first];

        int width = pos.width();
        int height = pos.height();
        if (fBkgZoomRateEx < 1 || NULL == dynamic_cast<QPushButton*>(widgetPos.first))
        {
            width = round(width * fBkgZoomRate);
            height = round(height * fBkgZoomRate);
        }

        /*if (width%2==1)
        {
            width++;
        }
        if (height%2==1)
        {
            height++;
        }*/

        newPos.setRect(xBkgOffset + round(fBkgZoomRate*pos.center().x() - width/2.0f)
                       , round(fBkgZoomRate*pos.center().y() - height/2.0f) + dy_bkg, width, height);
        widgetPos.first->setGeometry(newPos);
    }

    if (xBkgOffset != 0)
    {
        {auto& newPos = m_mapWidgetNewPos[ui.progressbar];
        newPos.adjust(-xBkgOffset, 0, xBkgOffset, 0);
        ui.progressbar->setGeometry(newPos);}

        {auto& newPos = m_mapWidgetNewPos[ui.labelProgress];
        newPos.adjust(-xBkgOffset, 0, xBkgOffset, 0);
        ui.labelProgress->setGeometry(newPos);}

        {auto& newPos = m_mapWidgetNewPos[ui.btnSetting];
        newPos.moveLeft(newPos.x()-xBkgOffset);
        ui.btnSetting->setGeometry(newPos);}

        {auto& newPos = m_mapWidgetNewPos[ui.labelPlayingfile];
        newPos.moveLeft(newPos.x()-xBkgOffset);
        ui.labelPlayingfile->setGeometry(newPos);}

        {auto& newPos = m_mapWidgetNewPos[ui.labelDuration];
        newPos.moveLeft(newPos.x()+xBkgOffset);
        ui.labelDuration->setGeometry(newPos);}

        {auto& newPos = m_mapWidgetNewPos[ui.btnRandom];
        newPos.moveLeft(newPos.x()+xBkgOffset);
        ui.btnRandom->setGeometry(newPos);}

        {auto& newPos = m_mapWidgetNewPos[ui.btnOrder];
        newPos.moveLeft(newPos.x()+xBkgOffset);
        ui.btnOrder->setGeometry(newPos);}
    }

    int y_frameDemand = __size(20);
    if (checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
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
        if (bDefaultBkg)
        {
            y_frameDemand = __size(12);
        }
    }

    int x_frameDemand = 0;
    if (bHLayout)
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
     if (!bHLayout)
     {
         x_btnMore = cx - __size(25) - ui.btnMore->width();
     }
#endif
    ui.btnMore->move(x_btnMore, y_btnMore);

    int x_btnExit = cx - ui.btnExit->width() - (y_frameDemand + __size(12));
    ui.btnExit->move(x_btnExit, y_btnMore);

    if (!bDefaultBkg)
    {
#define __dy __size(3)
        int dy =  round(fBkgZoomRate*__dy);

        if (fBkgZoomRateEx <= 1)
        {
#define __offset __size(6)
            int yOffset = round((float)__offset/fBkgZoomRate);
            for (auto pWidget : SList<QWidget*>({ui.labelDuration, ui.progressbar, ui.labelProgress}))
            {
                pWidget->move(pWidget->x(), pWidget->y() - yOffset*2);
            }

            dy -= yOffset;
        }

        for (auto pWidget : SList<QWidget*>(ui.btnPlay, ui.btnPause, ui.btnPlayPrev, ui.btnPlayNext
                                            , ui.btnSetting, ui.btnOrder, ui.btnRandom))
        {
            pWidget->move(pWidget->x(), pWidget->y() + dy);
        }
    }

    CLabel& labelAlbumName = *ui.labelAlbumName;
    WString strMediaSet;
    if (PlayingInfo.pRelatedMediaSet)
    {
        if (!bDefaultBkg)
        {
            /*if (PlayingInfo.uSingerID > 0)
            {
                strMediaSet << PlayingInfo.strSingerName << __CNDot;
            }
            else*/ if (E_MediaSetType::MST_Playlist == PlayingInfo.pRelatedMediaSet->m_eType)
            {
                strMediaSet << L"歌单: ";
            }
            else if (E_MediaSetType::MST_Album == PlayingInfo.pRelatedMediaSet->m_eType)
            {
                strMediaSet << L"专辑: ";
            }
        }

        strMediaSet << PlayingInfo.pRelatedMediaSet->name();

        labelAlbumName.setFont(0.95f);
        labelAlbumName.setShadow(uShadowWidth);
        labelAlbumName.setAlignment(Qt::AlignCenter);
    }
    labelAlbumName.setText(strMediaSet);

    ui.labelSingerName->setAlignment(Qt::AlignCenter);

    E_SingerImgPos eSingerImgPos = E_SingerImgPos::SIP_Float;
    //((bool&)PlayingInfo.bWholeTrack) = true; //测试代码
    bool bSingerImgFlag = PlayingInfo.bWholeTrack || ui.labelSingerImg->pixmap();
    //auto pmSingerImg = ui.labelSingerImg->pixmap();
    if (PlayingInfo.bWholeTrack)
    {
        //pmSingerImg = ((int)PlayingInfo.eQuality>=(int)E_MediaQuality::MQ_CD) ? &__app.m_pmHDDisk : &__app.m_pmLLDisk;
        //ui.labelSingerName->setShadow(uShadowWidth);
        eSingerImgPos = E_SingerImgPos::SIP_Zoomout;
    }
    else
    {
        if (bSingerImgFlag) //pmSingerImg)// && !pmSingerImg.isNull())
        {
            eSingerImgPos = t_eSingerImgPos;

            ui.labelSingerImg->setPixmapRound(bDefaultBkg?__size(5):__szRound);
            ui.labelSingerImg->setShadow(2);

            ui.labelSingerName->setShadow(2);
        }
        else
        {
            ui.labelSingerName->setShadow(uShadowWidth);
        }
    }

    auto rcSingerImg = m_mapWidgetNewPos[ui.labelSingerImg];

    PlayingList.setShadow(uShadowWidth);
    ui.labelPlayingfile->setShadow(uShadowWidth);
    ui.labelDuration->setShadow(uShadowWidth);

    int y_PlayingListMax = 0;

    int x = ui.progressbar->x();
    int cx_progressbar = ui.progressbar->width();

    int cy_Playingfile = ui.labelPlayingfile->height();
    int y_Playingfile = ui.labelDuration->geometry().bottom() -  cy_Playingfile;

#define __cylabelAlbumName __size(70)
    if (!bDefaultBkg)
    {
        int y_labelAlbumName = 0;
        if (!labelAlbumName.text().isEmpty())
        {
            y_labelAlbumName = y_Playingfile - __cylabelAlbumName;
        }
        else
        {
            y_labelAlbumName = y_Playingfile - __size(20);
        }

        int x_SingerImg = x;
        int cx_SingerImg = 0;
        int y_SingerImg = 0;
        int cy_SingerImg = 0;
        if (E_SingerImgPos::SIP_Zoomout == eSingerImgPos)
        {
            int y_SingerName = y_labelAlbumName - cy_Playingfile;
            if (PlayingInfo.bWholeTrack)
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

            ui.labelSingerName->setGeometry(x, y_SingerName, cx_progressbar-dx, __cylabelAlbumName);
            ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignTop);

            y_PlayingListMax = y_SingerImg - __size10;
        }
        else
        {
            if (E_SingerImgPos::SIP_Float == eSingerImgPos)
            {
                if (bHLayout)
                {
                    y_SingerImg = ui.frameDemandLanguage->geometry().bottom() + __size(50);
                }
                else
                {
                    y_SingerImg = cy/2+__size(150);
                }

                cx_SingerImg = cy_SingerImg = y_labelAlbumName-y_SingerImg;

                auto cyPm = ui.labelSingerImg->pixmapHeight();
                if (cyPm > 0)
                {
                    //cx_SingerImg = cy_SingerImg*1.05; //cy_SingerImg * rcSingerImg->width() / rcSingerImg->height();
                    cx_SingerImg = cy_SingerImg * ui.labelSingerImg->pixmapWidth() / cyPm;
                    cx_SingerImg = MIN(cx_SingerImg, cy_SingerImg*1.2f);
                    cx_SingerImg = MIN(cx_SingerImg, cx_progressbar);
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
            if (bSingerImgFlag) //pmSingerImg)// && !pmSingerImg.isNull())
            {
                y_PlayingListMax = y_SingerImg - __size10;
            }
            else
            {
                if (PlayingInfo.uSingerID > 0)
                {
                    y_labelSingerName += __size10;
                    y_PlayingListMax = y_labelSingerName;
                }
                else if (PlayingInfo.pRelatedMediaSet)
                {
                    y_PlayingListMax = y_labelAlbumName;
                }
                else
                {
                    y_PlayingListMax = y_Playingfile;
                }
            }

            ui.labelSingerName->setGeometry(x_SingerImg+__size(15), y_labelSingerName
                                            , cx_SingerImg-__size(15), ui.labelSingerName->height());
        }

        ui.labelPlayingfile->setGeometry(x, y_Playingfile, ui.labelDuration->x()-x, cy_Playingfile);

        rcSingerImg.setRect(x_SingerImg, y_SingerImg, cx_SingerImg, cy_SingerImg);
        ui.labelSingerImg->setGeometry(rcSingerImg);

        if (PlayingInfo.bWholeTrack)
        {
            cauto pm = ((int)PlayingInfo.eQuality>=(int)E_MediaQuality::MQ_CD) ? __app.m_pmHDDisk : __app.m_pmLLDisk;
            ui.labelSingerImg->setPixmap(pm); //pmSingerImg);
            ui.labelSingerImg->setPixmapRound(0);
            ui.labelSingerImg->setShadow(0);
        }
    }
    else
    {
        if (PlayingInfo.bWholeTrack)
        {
            ui.labelSingerImg->clear();
        }

        if (!bSingerImgFlag) //NULL == pmSingerImg)// || pmSingerImg->isNull())
        {
            auto y = y_Playingfile;
            if (PlayingInfo.pRelatedMediaSet)
            {
                y -= __cylabelAlbumName;
                labelAlbumName.setGeometry(x, y, cx_progressbar, __cylabelAlbumName);
                labelAlbumName.setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignVCenter);
            }
            y_PlayingListMax = y;

            if (PlayingInfo.uSingerID > 0)
            {
                y -= ui.labelPlayingfile->height();
                ui.labelSingerName->setGeometry(x, y, cx_progressbar, __cylabelAlbumName);
                ui.labelSingerName->setAlignment(Qt::AlignmentFlag::AlignHCenter | Qt::AlignmentFlag::AlignTop);
                y_PlayingListMax = y - __size10;
            }
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
                        break;
                    }
                }

                int x_labelAlbumName = rcAlbumNamePrev.left() + (rcAlbumNamePrev.width()-labelAlbumName.width())/2;
                labelAlbumName.move(x_labelAlbumName, labelAlbumName.y());
            } while (0);

            y_PlayingListMax = rcSingerImg.y() - __size10;
        }
    }

#define __CyPlayItem __size(115)
    UINT uRowCount = 0;
    if (bHLayout)
    {
        UINT uMargin = ui.progressbar->x();
        int x_PlayingList = ui.progressbar->geometry().right() + uMargin*1.5f;
        if (cx > __size(1920))
        {
            x_PlayingList += uMargin;
        }
        PlayingList.setGeometry(x_PlayingList, uMargin-1, cx-x_PlayingList, cy-uMargin*2);

        uRowCount = cy/__CyPlayItem;
        uRowCount = MAX(uRowCount,7);
        uRowCount = MIN(uRowCount,10);
    }
    else
    {
        UINT y_Margin = __size(30);

        int y_PlayingList = ui.frameDemand->geometry().bottom() - __size10;
        uRowCount = (y_PlayingListMax - y_PlayingList - y_Margin*2)/__CyPlayItem;
        if (uRowCount > 10)
        {
            uRowCount = 10;
            y_Margin += __size10;

            if (!bSingerImgFlag //NULL == pmSingerImg //|| pmSingerImg->isNull()
                    || E_SingerImgPos::SIP_Zoomout == eSingerImgPos)
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

        y_PlayingList += y_Margin;
        int cy_PlayingList = y_PlayingListMax - y_Margin - y_PlayingList;
        UINT x_Margin = ui.frameDemand->x();
        PlayingList.setGeometry(x_Margin, y_PlayingList, cx-x_Margin*2, cy_PlayingList);
    }
    PlayingList.setPageRowCount(uRowCount);
}
