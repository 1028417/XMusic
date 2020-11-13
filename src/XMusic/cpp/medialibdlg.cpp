
#include "app.h"

#include "medialibdlg.h"
#include "ui_medialibdlg.h"

static Ui::MedialibDlg ui;

CMedialibDlg::CMedialibDlg(QWidget& parent, class CApp& app) : CDialog(parent)
  , m_app(app)
  , m_lv(*this, app, m_OuterDir)
  , m_wholeTrackDlg(*this, app)
  , m_singerImgDlg(*this, app)
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
            m_app.getCtrl().callPlayCmd(tagAssignMediaSetCmd(*pMediaSet));
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
                m_app.getCtrl().callPlayCmd(tagAssignMediaCmd(paMedias));
            }
        }
    });

    m_lv.init();
}

/*this->setGeometry(m_app.mainWnd().geometry());
_resizeTitle();
//CApp::async([&](){
//    _resizeTitle();
//});*/

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

void CMedialibDlg::showMedia(const CMedia& media)
{
    m_lv.hittestMedia(media);

    _show();
}

bool CMedialibDlg::showMediaRes(cwstr strPath)
{
    CMediaRes *pMediaRes = __medialib.subFile(strPath);
    if (NULL == pMediaRes)
    {
        pMediaRes = m_OuterDir.subFile(strPath);
        if(NULL == pMediaRes)
        {
            return false;
        }
    }
    m_lv.hittestFile(*pMediaRes);

    CApp::async([&, pMediaRes]() {
        tryShowWholeTrack(*pMediaRes);
    });

    CDialog::show();

    return true;
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
    int cxMargin = sz/4;
//    if (cy < cx)
//    {
//#define __szOffset __size(6)
//        cxMargin += __szOffset;
//        yReturn -= __szOffset;
//    }
    QRect rcReturn(cxMargin, cxMargin, sz-cxMargin*2, sz-cxMargin*2);

    if (CApp::checkIPhoneXBangs(cx, cy)) // 针对全面屏刘海作偏移
    {
        rcReturn.moveTop(__cyIPhoneXBangs - rcReturn.top());
    }

    ui.btnReturn->setGeometry(rcReturn);

    auto szBtn = rcReturn.width();
    ui.btnUpward->setGeometry(rcReturn.right() + cxMargin/2, rcReturn.top(), szBtn, szBtn);

    auto& frameFilterLanguage = *ui.frameFilterLanguage;
    frameFilterLanguage.move(cx-frameFilterLanguage.width()-cxMargin
                                    , rcReturn.center().y()-frameFilterLanguage.height()/2);

    int x_btnPlay = cx - __lvRowMargin + __playIconOffset - szBtn;
    QRect rc(x_btnPlay, rcReturn.top(), szBtn, szBtn);
    ui.btnPlay->setGeometry(rc);

    rc.moveLeft(x_btnPlay - cxMargin - szBtn);
#define __szOffset __size(6)
    rc.adjust(-__szOffset, -__szOffset, __szOffset, __szOffset);
    ui.labelSingerImg->setGeometry(rc);

    _resizeTitle();

    int y_MedialibView = rcReturn.bottom() + rcReturn.top();
    m_lv.setGeometry(0, y_MedialibView, cx, cy-y_MedialibView);

    m_wholeTrackDlg.relayout(ui.btnReturn->geometry(), ui.btnUpward->geometry(), ui.btnPlay->geometry(), m_lv.geometry());
    m_singerImgDlg.relayout(ui.btnReturn->geometry());
}

void CMedialibDlg::_resizeTitle() const
{
    int cxMargin = ui.btnReturn->x();
    auto pButton = ui.btnUpward->isVisible() ? ui.btnUpward : ui.btnReturn;
    int x_title = pButton->geometry().right() + cxMargin;

    int cx_title = 0;
    if (ui.btnPlay->isVisible())
    {
        cx_title = ui.labelSingerImg->x();
        //cx_title = ui.btnPlay->x() - cxMargin - x_title;
    }
    else
    {
        cx_title = width() - cxMargin - x_title;
    }

    ui.labelTitle->setGeometry(x_title, ui.btnReturn->y(), cx_title, ui.btnReturn->height());
}

void CMedialibDlg::updateHead(const WString& strTitle)
{
    E_LabelTextOption lto = E_LabelTextOption::LTO_AutoFit;
    bool bShowFilterLanguage =  false;
    bool bShowUpwardButton = false;
    bool bShowPlayButton = false;

    auto pDir = m_lv.currentDir();
    if (pDir)
    {
        lto = E_LabelTextOption::LtO_Elided;
        bShowUpwardButton = true;
        bShowPlayButton = pDir->files();
    }
    else
    {
        auto pMediaSet = m_lv.currentMediaSet();
        if (pMediaSet)
        {
            bShowUpwardButton = true;
            bShowPlayButton = E_MediaSetType::MST_Singer==pMediaSet->m_eType
                    || E_MediaSetType::MST_Album==pMediaSet->m_eType
                    || E_MediaSetType::MST_Playlist==pMediaSet->m_eType;

            if (&m_app.getPlaylistMgr() == pMediaSet)
            {
                bShowFilterLanguage = true;
            }
        }
    }

    ui.labelTitle->setText(strTitle, lto);

    ui.frameFilterLanguage->setVisible(bShowFilterLanguage);

    auto pSinger = m_lv.currentSinger();
    if (pSinger)
    {
        bShowPlayButton = true;
    }
    if (pSinger && m_app.getSingerImgMgr().getSingerHead(pSinger->m_strName))
    {
        m_app.getSingerImgMgr().downloadSingerHead({pSinger->m_strName});

        ui.labelSingerImg->setPixmap(m_lv.genSingerHead(pSinger->m_uID, pSinger->m_strName));
        ui.labelSingerImg->setVisible(true);
    }
    else
    {
        ui.labelSingerImg->clear();
        ui.labelSingerImg->setVisible(false);
    }

    ui.btnPlay->setVisible(bShowPlayButton);

/*#if __android// || __ios
    bShowUpwardButton = false;
#endif*/
    ui.btnUpward->setVisible(bShowUpwardButton);

    _resizeTitle();
}

void CMedialibDlg::updateSingerImg(cwstr strSingerName, const tagSingerImg& singerImg)
{
    auto pSinger = m_lv.currentSinger();
    if (pSinger)
    {
        if (pSinger->m_strName != strSingerName)
        {
            return;
        }

        if (singerImg.isSmall())
        {
            ui.labelSingerImg->setPixmap(m_lv.genSingerHead(pSinger->m_uID, strSingerName));
        }
        else
        {
            if (m_singerImgDlg.isVisible())
            {
                m_singerImgDlg.updateSingerImg();
            }
        }
    }
    else
    {
        if (!singerImg.isSmall())
        {
            return;
        }

        auto pMediaSet = m_lv.currentMediaSet();
        if (NULL == pMediaSet)
        {
            return;
        }
        if (E_MediaSetType::MST_SingerGroup != pMediaSet->m_eType
                && E_MediaSetType::MST_Playlist != pMediaSet->m_eType)
        {
            return;
        }

        m_lv.update();
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
