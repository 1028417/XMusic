
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(QWidget& parent) : CDialog(parent)
  , m_lv(*this, m_OuterDir)
  , m_wholeTrackDlg(*this)
  , m_singerImgDlg(*this)
{
}

void CMedialibDlg::preinit()
{
    m_lv.initpm();
}

void CMedialibDlg::init()
{
    ui.setupUi(this);

    ui.labelTitle->setFont(__titleFontSize, QFont::Weight::DemiBold);

    m_lv.setFont(1.0f, QFont::Weight::Normal);

    ui.labelSingerImg->setPixmapRound(__szRound);
    connect(ui.labelSingerImg, &CLabel::signal_click, [&](){
        auto pSinger = m_lv.currentSinger();
        if (pSinger)
        {
            m_singerImgDlg.show(pSinger->m_strName);
        }
    });

    ui.frameFilterLanguage->setAttribute(Qt::WA_TranslucentBackground);

    SList<CLabel*> lstLabels {ui.labelDemandCN, ui.labelDemandHK
                , ui.labelDemandKR, ui.labelDemandEN, ui.labelDemandEUR};
    for (auto label : lstLabels)
    {
        label->setFont(1.08f, QFont::Weight::Normal, false, true);

        connect(label, &CLabel::signal_click, this, &CMedialibDlg::slot_labelClick);
    }

    connect(ui.btnReturn, &CButton::signal_clicked, this, &QDialog::close);

    connect(ui.btnUpward, &CButton::signal_clicked, [&](){
        m_lv.upward();
    });

    connect(ui.btnPlay, &CButton::signal_clicked, [&](){
        m_lv.dselectItem();

        CMediaSet *pMediaSet = m_lv.currentMediaSet();
        if (pMediaSet)
        {
            __app.getCtrl().callPlayCmd(tagAssignMediaSetCmd(*pMediaSet));
            return;
        }

        CPath *pDir = m_lv.currentDir();
        if (pDir)
        {
            TD_IMediaList paMedias;
            pDir->files()([&](XFile& file){
                paMedias.add((CMediaRes&)file);
            });

            if (paMedias)
            {
                __app.getCtrl().callPlayCmd(tagAssignMediaCmd(paMedias));
            }
        }
    });
}

void CMedialibDlg::_show()
{
    CDialog::show([&](){
        m_lv.cleanup();

        m_OuterDir.clear();
    });
}

void CMedialibDlg::show()
{
    m_lv.showRoot();

    _show();
}

void CMedialibDlg::showMediaSet(CMediaSet& MediaSet)
{
    m_lv.showMediaSet(MediaSet);

    _show();
}

bool CMedialibDlg::showMedia(IMedia& media)
{
    if (!m_lv.hittestMedia(media))
    {
        if (media.type() != E_MediaType::MT_MediaRes)
        {
            return false;
        }

        m_lv.hittestFile((CMediaRes&)media);
    }

    CApp::async([&]() {
        m_wholeTrackDlg.tryShow(media);
    });

    _show();

    return true;
}

CMediaRes* CMedialibDlg::showMediaRes(cwstr strPath)
{
    CMediaRes *pMediaRes = m_OuterDir.subFile(strPath);
    if (NULL == pMediaRes)
    {
        pMediaRes = __medialib.subFile(strPath);
        if(NULL == pMediaRes)
        {
            return NULL;
        }
    }

    m_lv.hittestFile(*pMediaRes);

    _show();

    return pMediaRes;
}

size_t CMedialibDlg::caleRowCount(int cy)
{
    /*UINT uRowCount = 10;
    if (cy >= __size(2340))
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
    if (cx > 1080)
    {
        cxMargin += __size(6);
    }

    auto szBtn = sz-cyMargin*2;
    QRect rcReturn(cxMargin, cyMargin, szBtn, szBtn);
    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }
    ui.btnReturn->setGeometry(rcReturn);

    QRect rcUpward(rcReturn.right() + cxMargin/2, rcReturn.top(), szBtn, szBtn);
    ui.btnUpward->setGeometry(rcUpward);

    auto& frameFilterLanguage = *ui.frameFilterLanguage;
    frameFilterLanguage.move(cx-frameFilterLanguage.width()-cxMargin
                                    , rcReturn.center().y()-frameFilterLanguage.height()/2);

    int x_btnPlay = cx - __lvRowMargin + __playIconOffset - szBtn;
    QRect rcPlay(x_btnPlay, rcReturn.top(), szBtn, szBtn);
    ui.btnPlay->setGeometry(rcPlay);

    _relayoutTitle();

    int y_MedialibView = rcReturn.bottom() + rcReturn.top();
    QRect rcLv(0, y_MedialibView, cx, cy-y_MedialibView);
    m_lv.setGeometry(rcLv);

    m_wholeTrackDlg.relayout(rcReturn, rcUpward, ui.labelTitle->geometry(), rcPlay, rcLv);
    m_singerImgDlg.relayout(rcReturn);
}

void CMedialibDlg::_relayoutTitle() const
{
    int cxMargin = ui.btnReturn->x();

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

    auto pButton = ui.btnUpward->isVisible() ? ui.btnUpward : ui.btnReturn;
    int x_title = pButton->geometry().right() + cxMargin;
    int cx_title = rc.x()-cxMargin-x_title;
    ui.labelTitle->setGeometry(x_title, 0, cx_title, rc.bottom() + rc.top());
}

void CMedialibDlg::updateHead(const WString& strTitle)
{
    bool bShowUpwardButton = false;
    bool bShowPlayButton = false;
    int nElidedFlag = -1;
    auto pMediaSet = m_lv.currentMediaSet();
    if (pMediaSet)
    {
        bShowUpwardButton = true;

        if (E_MediaSetType::MST_Singer==pMediaSet->m_eType
                || E_MediaSetType::MST_Album==pMediaSet->m_eType
                || E_MediaSetType::MST_Playlist==pMediaSet->m_eType)
        {
            bShowPlayButton = true;
        }
        else if (E_MediaSetType::MST_SnapshotMediaDir == pMediaSet->m_eType)
        {
            nElidedFlag = Qt::TextWordWrap | Qt::TextHideMnemonic;
        }
    }
    else
    {
        auto pDir = m_lv.currentDir();
        if (pDir)
        {
            bShowUpwardButton = true;
            bShowPlayButton = pDir->files();
            nElidedFlag = Qt::TextWordWrap | Qt::TextHideMnemonic;
        }
    }

#if __android || __ios
    if (width() <= 1080)
    {
        bShowUpwardButton = false;
    }
#endif

    ui.btnUpward->setVisible(bShowUpwardButton);

    ui.frameFilterLanguage->setVisible(&__app.getPlaylistMgr() == pMediaSet);

    ui.labelTitle->setText(strTitle, nElidedFlag);

    auto pSinger = m_lv.currentSinger();
    do {
        if (pSinger)
        {
            bShowPlayButton = true;

            cauto pm = m_lv.genSingerHead(pSinger->m_uID, pSinger->m_strName);
            if (&pm != &m_lv.m_pmDefaultSinger)
            {
                ui.labelSingerImg->setPixmap(pm);
                ui.labelSingerImg->setVisible(true);
                break;
            }

            __app.getSingerImgMgr().downloadSingerHead({pSinger->m_strName});
        }

        ui.labelSingerImg->clear();
        ui.labelSingerImg->setVisible(false);
    } while(0);

    ui.btnPlay->setVisible(bShowPlayButton);

    _relayoutTitle();
}

void CMedialibDlg::updateSingerImg(cwstr strSingerName, const tagSingerImg& singerImg)
{    
    if (singerImg.isSmall())
    {
        m_lv.update();
    }

    auto pSinger = m_lv.currentSinger();
    if (pSinger && pSinger->m_strName == strSingerName)
    {
        if (singerImg.isSmall())
        {
            ui.labelSingerImg->setPixmap(m_lv.genSingerHead(pSinger->m_uID, strSingerName));            
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
        , {ui.labelDemandEUR, E_LanguageType::LT_EUR}
    };
    auto uLanguage = (UINT)mapLabels[label];

    cauto paMediaSet = m_lv.currentSubSets();
    //paMediaSet.get((UINT)m_MedialibView.scrollPos(), [&](const CMediaSet& MediaSet){
    //    if (MediaSet.property().language() != uLanguage) {
            int nItem = paMediaSet.find([&](const CMediaSet& MediaSet){
                if ((UINT)E_LanguageType::LT_EUR == uLanguage)
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
