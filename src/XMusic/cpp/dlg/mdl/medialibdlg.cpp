
#include "xmusic.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg() : m_lv(*this)
  //, m_wholeTrackDlg(*this)
  //, m_singerImgDlg(*this)
{
}

void CMedialibDlg::init()
{
    m_wholeTrackDlg.init();
    m_singerImgDlg.init();

    m_lv.initpm();

    ui.setupUi(this);

    ui.btnReturn->connect_dlgClose(this);

    ui.btnUpward->onClicked([&]{
        m_lv.upward();
    });

    ui.labelTitle->setFont(__titleFontSize, TD_FontWeight::DemiBold);

    ui.frameFilterLanguage->setAttribute(Qt::WA_TranslucentBackground);

    SList<CLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK
                , ui.labelDemandKR, ui.labelDemandEN, ui.labelDemandEUR};
    for (auto label : lstLabels)
    {
        label->setFont(1.08f, TD_FontWeight::Normal, false, true);

        label->onClicked(this, &CMedialibDlg::slot_labelClick);
    }

    ui.labelSingerImg->setPixmapRound(__szRound);
    ui.labelSingerImg->onClicked([&]{
        auto pSinger = m_lv.currentSinger();
        if (pSinger)
        {
            m_singerImgDlg.show(pSinger->m_uID);
        }
    });

    ui.btnPlay->onClicked(this, &CMedialibDlg::slot_playClick);

    ui.labelPkg->adjustFontWeight(TD_FontWeight::DemiBold);
    ui.labelPkg->setPixmap(m_lv.pmXpk());

    ui.labelPkg->onClicked([&]{
        m_lv.showDir(__xmedialib.xpkRoot());
    });

    m_lv.adjustFontWeight(TD_FontWeight::Normal);
}

void CMedialibDlg::show()
{
    m_lv.showRoot();

    CDialog::show();
}

void CMedialibDlg::showMediaSet(const CMediaSet& MediaSet)
{
    m_lv.showMediaSet((CMediaSet&)MediaSet);

    CDialog::show();
}

void CMedialibDlg::showMediaSet(const IMedia& media)
{
    _relayout(g_app.mainWnd().width(), g_app.mainWnd().height());

    if (!m_lv.hittestMedia(media))
    {
        return;
    }

    CDialog::show();
}

bool CMedialibDlg::showMedia(const IMedia& media)
{
    _relayout(g_app.mainWnd().width(), g_app.mainWnd().height());

    if (media.type() == E_MediaType::MT_MediaRes)
    {
        m_lv.hittestFile((CMediaRes&)media);
    }
    else
    {
        if (!m_lv.hittestMedia(media))
        {
            return false;
        }
    }

    g_app.sync([&]{
        m_wholeTrackDlg.tryShow((IMedia&)media);
    });

    CDialog::show();

    return true;
}

CMediaRes* CMedialibDlg::showLocalFile(cwstr strPath)
{
    _relayout(g_app.mainWnd().width(), g_app.mainWnd().height());

    auto pMediaRes = m_lv.hittestLocalFile(strPath);
    if (pMediaRes)
    {
        CDialog::show();
    }
    return pMediaRes;
}

size_t CMedialibDlg::caleRowCount(int cy)
{
    /*UINT uRowCount = 10;
    if (cy >= __size(__cyBkg))
    {
       uRowCount++;
    }
    else if (cy <= __size(1920))
    {
        uRowCount--;
        if (cy < __size(1800))
        {
            uRowCount = round((float)uRowCount*cy/__size(1800));
        }
    }*/

    return round(11.0f*cy/__size(2160));
}

void CMedialibDlg::_relayout(int cx, int cy)
{
    m_uRowCount = caleRowCount(cy);
    int sz = cy/(1.1f+m_uRowCount);

    int cyMargin = sz/4;
    int cxMargin = cyMargin;
    if (cx > __size(1080))
    {
        cxMargin += __size(6);
    }

    auto szBtn = sz-cyMargin*2;
    QRect rcReturn(cxMargin, cyMargin, szBtn, szBtn);
    UINT cyBangs = checkIPhoneXBangs(cx, cy);
    if (cyBangs)
    {
        rcReturn.moveTop(cyBangs - cyMargin);
    }
    else
    {
        UINT cyStatusBar = checkAndroidStatusBar();
        if (cyStatusBar)
        {
            rcReturn.moveTop(cyMargin + cyStatusBar);
        }
    }
    ui.btnReturn->setGeometry(rcReturn);

    int xUpward = rcReturn.right() + cxMargin*.6f;
    ui.btnUpward->setGeometry(xUpward, rcReturn.top(), szBtn, szBtn);

    auto& frameFilterLanguage = *ui.frameFilterLanguage;
    frameFilterLanguage.move(cx-frameFilterLanguage.width()-cxMargin
                                    , rcReturn.center().y()-frameFilterLanguage.height()/2);

    int xBtn = cx - __lvRowMargin  - szBtn;
    ui.btnPlay->setGeometry(xBtn + __playIconOffset, rcReturn.top(), szBtn, szBtn);

    int __cyOffset = __size(18);
#if __android || __ios
    if (!m_bHLayout)
    {
        __cyOffset -= __size(6);
    }
#endif
#define __cxOffset __size(200)
    ui.labelPkg->setGeometry(xBtn-__cyOffset-__cxOffset, rcReturn.top()-__cyOffset/2
                           , __cxOffset+szBtn+__cyOffset, szBtn+__cyOffset);

    bool bShowXpkBtn = false;
    bool bShowXpkRoot = false;
    if (__xmedialib.xpkRoot().count())
    {
        if (m_bHLayout)
        {
            bShowXpkBtn = m_lv.currentMediaSet() == NULL && m_lv.currentDir() == NULL;
        }
        else
        {
            bShowXpkRoot = true;
        }
    }
    ui.labelPkg->setVisible(bShowXpkBtn);
    m_lv.resetRootItem(bShowXpkRoot);

    _relayoutTitle();

    int yLv = rcReturn.bottom() + cyMargin;
    m_lv.setGeometry(0, yLv, cx, cy-yLv);

    m_singerImgDlg.relayoutTitle(rcReturn);
}

void CMedialibDlg::_relayoutTitle()
{
    cauto rcReturn = ui.btnReturn->geometry();
    int cxMargin = rcReturn.x();

    auto rc = ui.btnPlay->geometry();
    if (ui.labelSingerImg->isVisible())
    {
#define __szOffset __size(6)
        if (ui.btnPlay->isVisible())
        {
            rc.moveLeft(rc.x() - cxMargin - rc.width() - __szOffset);
        }
        else
        {
            rc.moveLeft(rc.x() - __szOffset);
        }
        rc.adjust(-__szOffset, -__szOffset, __szOffset, __szOffset);
        ui.labelSingerImg->setGeometry(rc);
    }

     ui.btnUpward->setVisible(m_lv.currentMediaSet() || m_lv.currentDir());
#if __android || __ios
    if (width() <= __size(1080))
    {
        ui.btnUpward->setVisible(false);
    }
#endif

    cauto rcUpward = ui.btnUpward->geometry();
    int x_title = cxMargin + (ui.btnUpward->isVisible() ? rcUpward.right() : rcReturn.right());
    int cx_title = rc.x()-cxMargin-x_title;
    int cyMargin = m_lv.y()-rcReturn.bottom();
    ui.labelTitle->setGeometry(x_title, rcReturn.top()-cyMargin
                               , cx_title, rcReturn.height() + cyMargin*2);

    m_wholeTrackDlg.relayoutTitle(rcReturn, rcUpward, ui.btnPlay->geometry(), m_lv.geometry());
}

void CMedialibDlg::updateHead(const WString& strTitle)
{
    int nElidedFlag = -1;
    bool bShowPlayButton = false;

    CPath *pDir = NULL;
    auto pMediaSet = m_lv.currentMediaSet();
    if (pMediaSet)
    {
        if (E_MediaSetType::MST_Singer == pMediaSet->m_eType
                || E_MediaSetType::MST_Album == pMediaSet->m_eType
                || E_MediaSetType::MST_Playlist == pMediaSet->m_eType)
        {
            bShowPlayButton = true;
        }
        else if (E_MediaSetType::MST_SnapshotDir == pMediaSet->m_eType)
        {
            nElidedFlag = Qt::TextWordWrap | Qt::TextHideMnemonic;

            for (auto pSubFile : ((CSnapshotDir*)pMediaSet)->files())
            {
                if (((CSnapshotMedia*)pSubFile)->available)
                {
                    bShowPlayButton = true;
                    break;
                }
            }
        }
    }
    else
    {
        pDir = m_lv.currentDir();
        if (pDir)
        {
            if (_snapshotDir(*pDir))
            {
                for (auto pSubFile : pDir->files())
                {
                    if (((CSnapshotMedia*)pSubFile)->available)
                    {
                        bShowPlayButton = true;
                        break;
                    }
                }
            }
            else
            {
                bShowPlayButton = pDir->files();
            }

            nElidedFlag = Qt::TextWordWrap | Qt::TextHideMnemonic;
        }
    }

    ui.labelTitle->setText(strTitle, nElidedFlag);

    ui.frameFilterLanguage->setVisible(&g_app.getPlaylistMgr() == pMediaSet);

    auto pSinger = m_lv.currentSinger();
    do {
        if (pSinger)
        {
            auto& brSingerHead = m_lv.genSingerHead(pSinger->m_uID);
            if (&brSingerHead != &m_lv.m_brNullSingerHead)
            {
                ui.labelSingerImg->setPixmap(brSingerHead);
                ui.labelSingerImg->setVisible(true);
                break;
            }

            g_app.getSingerImgMgr().downloadSingerHead({pSinger->m_uID});
        }

        ui.labelSingerImg->clear();
        ui.labelSingerImg->setVisible(false);
    } while(0);

    ui.btnPlay->setVisible(bShowPlayButton);

    bool bShowXpkBtn = m_bHLayout && NULL == pMediaSet && NULL == pDir && __xmedialib.xpkRoot().count();
    ui.labelPkg->setVisible(bShowXpkBtn);

    _relayoutTitle();
}

void CMedialibDlg::updateSingerImg(CSinger& singer, const tagSingerImg& singerImg)
{    
    if (singerImg.isSmall())
    {
        m_lv.update();
    }

    if (m_lv.currentSinger() == &singer)
    {
        if (singerImg.isSmall())
        {
            ui.labelSingerImg->setPixmap(m_lv.genSingerHead(singer.m_uID));
            ui.labelSingerImg->setVisible(true);
            _relayoutTitle();
        }
        else
        {
            if (m_singerImgDlg.isVisible())
            {
                m_singerImgDlg.updateSingerImg();
            }
        }
    }
}

void CMedialibDlg::slot_labelClick(CLabel *label, const QPoint&)
{
    static map<CLabel*, E_LanguageType> mapLabels {
        {ui.labelDemandCN, E_LanguageType::LT_CN}
        , {ui.labelDemandHK, E_LanguageType::LT_HK}
        , {ui.labelDemandKR, E_LanguageType::LT_KR}
        , {ui.labelDemandEN, E_LanguageType::LT_EN}
        , {ui.labelDemandEUR, E_LanguageType::LT_Other}
    };
    auto uLanguage = (UINT)mapLabels[label];

    cauto paMediaSet = m_lv.currentSubSets();
    //paMediaSet.get((UINT)m_MedialibView.scrollPos(), [&](const CMediaSet& MediaSet){
    //    if (MediaSet.property().language() != uLanguage) {
            int nItem = paMediaSet.find([&](const CMediaSet& MediaSet){
                if ((UINT)E_LanguageType::LT_Other == uLanguage)
                {
                    if (MediaSet.property().language() & (UINT)E_LanguageType::LT_JP)
                    {
                        return true;
                    }
                }

                return bool(MediaSet.property().language() & uLanguage);
            });
            if (nItem >= 0)
            {
                m_lv.scrollToItem(nItem);
            }
    //    }
    //});
}

void CMedialibDlg::slot_playClick()
{
    m_lv.dselectItem();

    CMediaSet *pMediaSet = m_lv.currentMediaSet();
    if (pMediaSet)
    {
        g_app.getCtrl().callPlayCmd(tagAssignMediaSetCmd(*pMediaSet));
        return;
    }

    auto pDir = m_lv.currentDir();
    if (NULL == pDir)
    {
        return;
    }

    TD_IMediaList paMedias;
    if (_snapshotDir(*pDir))
    {
        for (auto pSubFile : pDir->files())
        {
            auto pSnapshotMedia = (CSnapshotMedia*)pSubFile;
            if (pSnapshotMedia->available)
            {
                paMedias.add(pSnapshotMedia);
            }
        }
    }
    else
    {
        TD_MediaResList paMediasRes(pDir->files());
        paMedias.add(paMediasRes);
    }

    if (paMedias)
    {
        g_app.getCtrl().callPlayCmd(tagAssignMediaCmd(paMedias));
    }
}
