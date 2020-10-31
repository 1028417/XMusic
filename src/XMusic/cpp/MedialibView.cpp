
#include "app.h"

#include "MedialibView.h"

#define __FlashingAlpha 170

#define __RemarkAlpha 200

CMedialibView::CMedialibView(CMedialibDlg& medialibDlg, class CApp& app, CMediaDir &OuterDir) :
    CMLListView(&medialibDlg, E_LVScrollBar::LVSB_Left)
    , m_medialibDlg(medialibDlg)
    , m_app(app)
    , m_SingerLib(app.getSingerMgr())
    , m_PlaylistLib(app.getPlaylistMgr())
    , m_OuterDir(OuterDir)
{
}

void CMedialibView::initpm()
{
    (void)m_pmSSFile.load(__mediaPng(media));

    (void)m_pmSingerGroup.load(__mediaPng(singergroup));
    (void)m_pmDefaultSinger.load(__mediaPng(singerdefault));
    (void)m_pmAlbum.load(__mediaPng(album));

    (void)m_pmPlaylistSet.load(__mediaPng(playlistset));
    (void)m_pmPlaylist.load(__mediaPng(playlist));
    (void)m_pmPlayItem.load(__mediaPng(playitem));

    (void)m_pmXmusicDir.load(__mediaPng(xmusicdir));
    (void)m_pmSSDir.load(__mediaPng(ssdir));

    (void)m_pmHires.load(__mediaPng(hires));
    (void)m_pmDSD.load(__mediaPng(dsd));
    (void)m_pmMQS.load(__mediaPng(mqs));
    (void)m_pmDTS.load(__mediaPng(dts));

    (void)m_pmDir.load(__mediaPng(dir));
    (void)m_pmDirLink.load(__mediaPng(dirLink));
    (void)m_pmFile.load(__mediaPng(file));

    (void)m_pmPlay.load(":/img/btnPlay.png");
    m_pmPlayOpacity = CPainter::alphaPixmap(m_pmPlay, 128);
    (void)m_pmAddPlay.load(":/img/btnAddplay.png");
    m_pmAddPlayOpacity = CPainter::alphaPixmap(m_pmAddPlay, 128);
}

void CMedialibView::init()
{
    TD_MediaSetList paSingers;
    m_SingerLib.GetAllMediaSets(E_MediaSetType::MST_Singer, paSingers);
    paSingers([&](CMediaSet& MediaSet){
        auto&& strSingerDir = strutil::lowerCase_r(((CSinger&)MediaSet). dir());
        m_plSingerDir.emplace_back(strSingerDir, &MediaSet);
    });
}

void CMedialibView::_onShowRoot()
{
    m_medialibDlg.updateHead();
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    m_medialibDlg.updateHead();

    if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
    {
        /*if (m_pthrFindRelated)
        {
            m_pthrFindRelated->join();
        }
        else
        {
            m_pthrFindRelated = &m_app.thread();
        }
        m_pthrFindRelated->start([&](XT_RunSignal bRunSignal){
            ((CPlaylist&)MediaSet).playItems()([&](const CPlayItem& playItem){
                mtutil::usleep(10);
                ((CPlayItem&)playItem).findRelatedMedia(E_RelatedMediaSet::RMS_Album);
                return bRunSignal;
            });

            if (bRunSignal)
            {
                m_app.sync([&](){
                    update();
                    setPlaylist.insert(&MediaSet);
                });
            }
        });*/

        list<wstring> *plstSingerName = NULL;
        static map<const void*, std::list<wstring>> mapSingerName;
        cauto itr = mapSingerName.find(&MediaSet);
        if (itr != mapSingerName.end())
        {
            plstSingerName = &itr->second;
        }
        else
        {
            plstSingerName = &mapSingerName[&MediaSet];
            for (cauto PlayItem : ((CPlaylist&)MediaSet).playItems())
            {
                cauto strPath = strutil::lowerCase_r(PlayItem.GetPath());
                for (cauto pr : m_plSingerDir)
                {
                    if (fsutil::CheckSubPath(pr.first, strPath))
                    {
                        cauto singer = *pr.second;
                        ((CPlayItem&)PlayItem).SetRelatedMediaSet(E_RelatedMediaSet::RMS_Singer
                                                                  , singer.m_uID, singer.m_strName);
                        if (std::find(plstSingerName->begin(), plstSingerName->end(), singer.m_strName) == plstSingerName->end())
                        {
                            plstSingerName->push_back(singer.m_strName);
                        }
                        break;
                    }
                }
            }
        }

        if (!plstSingerName->empty())
        {
            m_app.getSingerImgMgr().downloadSingerHead(*plstSingerName);
        }
    }
    else if (E_MediaSetType::MST_SingerGroup == MediaSet.m_eType)
    {
        list<wstring> lstSingerName;
        for (cauto singer : ((CSingerGroup&)MediaSet).singers())
        {
            lstSingerName.push_back(singer.m_strName);
        }
        m_app.getSingerImgMgr().downloadSingerHead(lstSingerName);
    }
}

void CMedialibView::_onShowDir(CPath&)
{
    m_medialibDlg.updateHead();
}

size_t CMedialibView::getColCount() const
{
    if (m_medialibDlg.isHLayout() && getItemCount() > getRowCount())
    {
        return 2;
    }

    return 1;
}

size_t CMedialibView::getRowCount() const
{
    bool bHLayout = m_medialibDlg.isHLayout();

    if (current())
    {
        if (currentDir() == &__medialib)
        {
            //return m_medialibDlg.rowCount()-1;

            if (bHLayout)
            {
                return ceil(__medialib.dirs().size()/2.0f);
            }
            else
            {
                return __medialib.dirs().size();
            }
        }

        auto uCount = getItemCount();
        if (bHLayout)
        {
            uCount = ceil(uCount/2.0f);
        }

        auto uRowCount = m_medialibDlg.rowCount();
        if (uRowCount > uCount)
        {
            uRowCount--;
        }

        /*if (uRowCount > 9)
        {
            if (uRowCount > uCount)
            {
                uRowCount--;
            }
        }*/

        return uRowCount;
    }
    else
    {
        if (m_medialibDlg.isHLayout())
        {
            return 6;
        }
        else
        {
            return 10;
        }
    }
}

size_t CMedialibView::_getRootItemCount() const
{
    if (m_medialibDlg.isHLayout())
    {
        return 12;
    }
    else
    {
        return 10;
    }
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context)
{
    context.nIconSize = __size(100);
    context.eStyle |= E_LVItemStyle::IS_MultiLine;
    if (context.pMediaSet)
    {
        switch (context.pMediaSet->m_eType)
        {
        case E_MediaSetType::MST_Playlist:
            context.pmIcon = &m_pmPlaylist;
            break;
        case E_MediaSetType::MST_Album:
            context.pmIcon = &m_pmAlbum;
            break;
        case E_MediaSetType::MST_Singer:
            context.pmIcon = &genSingerHead(context.pMediaSet->m_uID,
                                               context.pMediaSet->m_strName);
            break;
        case E_MediaSetType::MST_SingerGroup:
            context.pmIcon = &m_pmSingerGroup;

            context.eStyle |= E_LVItemStyle::IS_RightTip;
        default:
            break;
        };
    }
    else if (context.pMedia)
    {
        if (context.pMedia->GetMediaSetType() == E_MediaSetType::MST_Playlist)
        {
            context.pmIcon = &m_pmPlayItem;

            UINT uSingerID = context.pMedia->GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
            if (uSingerID > 0)
            {
                cauto strSingerName = context.pMedia->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
                if (!strSingerName.empty())
                {
                    context.pmIcon = &genSingerHead(uSingerID, strSingerName);
                }
            }
        }
        //else context.pixmap = &m_pmAlbumItem;
    }
    else if (context.pDir)
    {
        context.eStyle |= E_LVItemStyle::IS_RightTip;

        if (context.pDir->rootDir() == &__medialib)
        {
            context.pmIcon = &m_pmSSDir;

            if (context.pDir->parent() == &__medialib)
            {
                auto strDirName = context.pDir->fileName();
                strDirName = __substr(strDirName,3);
                context.strText = strDirName;

                //context.fIconMargin *= .9f * m_medialibDlg.rowCount()/this->getRowCount();
                context.nIconSize *= 1.13f;

                if (strutil::matchIgnoreCase(strDirName, L"hi-res"))
                {
                    context.pmIcon = &m_pmHires;
                    context.strText = L"高解析音频(High Resolution Audio)";
                }
                else if (strutil::matchIgnoreCase(strDirName, L"dsd"))
                {
                    context.pmIcon = &m_pmDSD;
                    context.strText = L"直接比特流数字编码(Direct Stream Digital)";
                }
                else if (strutil::matchIgnoreCase(strDirName, L"mqs"))
                {
                    context.pmIcon = &m_pmMQS;
                    context.strText = L"录音棚级别无损音乐(Mastering Quality Sound)";
                }
                else if (strutil::matchIgnoreCase(strDirName, L"dts"))
                {
                    context.pmIcon = &m_pmDTS;
                    context.strText = L"5.1声道DTSDigitalSurround";
                }
            }
        }
        else
        {
            context.pmIcon = &m_pmDir;

            if (context.pDir->parent() == NULL)
            {
                CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(context.pDir);
                if (pAttachDir)
                {
                    context.pmIcon = &m_pmDirLink;

                    /*if (E_AttachDirType::ADT_TF == pAttachDir->m_eType)
                    {
                        context.strRemark = L"扩展SD";
                    }
                    else if (E_AttachDirType::ADT_USB == pAttachDir->m_eType)
                    {
                        context.strRemark = L"USB";
                    }*/
                }
            }
        }
    }
    else if (context.pFile)
    {
        auto pMediaRes = ((CMediaRes*)context.pFile);
        if (pMediaRes->parent()->dirType() == E_MediaDirType::MDT_Snapshot)
        {
            if (pMediaRes->duration() > __wholeTrackDuration)
            {
                context.uIconRound = 0;
                if (pMediaRes->quality() >= E_MediaQuality::MQ_CD)
                {
                    context.pmIcon = &m_app.m_pmHDDisk;
                }
                else
                {
                    context.pmIcon = &m_app.m_pmLLDisk;
                }
            }
            else
            {
                context.pmIcon = &m_pmSSFile;
            }

            context.strText = pMediaRes->GetTitle();
        }
        else
        {
            context.pmIcon = &m_pmFile;
            context.strText = pMediaRes->fileName();
        }
    }
    else
    {
        context.eStyle = E_LVItemStyle::IS_CenterAlign;

        bool bHLayout = m_medialibDlg.isHLayout();

        //context.fIconMargin *= .9f * m_medialibDlg.rowCount()/this->getRowCount();
        context.nIconSize *= 1.15f;

        context.uIconRound = 0;

        cauto uRow = context->uRow;
        if ((bHLayout && 1 == uRow && 0 == context->uCol) || (!bHLayout && 1 == uRow))
        {
            context.pmIcon = &m_pmSingerGroup;
            context.strText = L" 歌手";
            context.pMediaSet = &m_SingerLib;
        }
        else if ((bHLayout && 1 == uRow && 1 == context->uCol) || (!bHLayout && 3 == uRow))
        {
            context.pmIcon = &m_pmPlaylistSet;
            context.strText = L" 歌单";
            context.pMediaSet = &m_PlaylistLib;
        }
        else if ((bHLayout && 3 == uRow && 0 == context->uCol) || (!bHLayout && 5 == uRow))
        {
            context.pmIcon = &m_pmXmusicDir;
            context.strText = __XMusicDirName;
            context.pDir = &__medialib;
        }
        else if ((bHLayout && 3 == uRow && 1 == context->uCol) || (!bHLayout && 7 == uRow))
        {
            context.pmIcon = &m_pmDir;
            context.strText << ' ' << __LocalDirName;
            context.pDir = &m_OuterDir;
        }
    }
}

CSinger *CMedialibView::currentSinger() const
{
    auto pMediaSet = currentMediaSet();
    if (pMediaSet)
    {
        if (E_MediaSetType::MST_Singer == pMediaSet->m_eType)
        {
            return (CSinger*)pMediaSet;
        }

        if (E_MediaSetType::MST_Album == pMediaSet->m_eType)
        {
            return (CSinger*)pMediaSet->m_pParent;
        }
    }

    return NULL;
}

cqpm CMedialibView::genSingerHead(UINT uSingerID, cwstr strSingerName)
{
    auto& pSingerPixmap = m_mapSingerPixmap[uSingerID];
    if (pSingerPixmap)
    {
        return *pSingerPixmap;
    }

    cauto strSingerImg = m_app.getSingerImgMgr().getSingerHead(strSingerName);
    if (!strSingerImg.empty())
    {
        QPixmap pm;
        if (pm.load(__WS2Q(strSingerImg)))
        {
            m_lstSingerPixmap.emplace_back();
            pSingerPixmap = &m_lstSingerPixmap.back();

/*#define __singerimgZoomout 128
            if (pm.width() > __singerimgZoomout && pm.height() > __singerimgZoomout)
            {
                if (pm.width() < pm.height())
                {
                    *pSingerPixmap = pm.scaledToWidth(__singerimgZoomout, Qt::SmoothTransformation);
                }
                else
                {
                    *pSingerPixmap = pm.scaledToHeight(__singerimgZoomout, Qt::SmoothTransformation);
                }
            }
            else*/
            {
                *pSingerPixmap = std::move(pm);
            }

            return *pSingerPixmap;
        }
    }

    return m_pmDefaultSinger;
}

#define __rAlign Qt::AlignRight|Qt::AlignVCenter

inline static cqrc _paintRemark(CPainter& painter, cqrc rc, cqstr qsRemark)
{
    CPainterFontGuard fontGuard(painter, 0.8, QFont::Weight::ExtraLight);
    UINT uAlpha = CPainter::oppTextAlpha(__RemarkAlpha);
    return painter.drawTextEx(rc, __rAlign, qsRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);
}

cqrc CMedialibView::_paintText(tagLVItemContext& context, CPainter& painter, QRect& rc
                               , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    cauto mlContext = (tagMLItemContext&)context;

    if (mlContext.playable())
    {
        UINT cy = context->rc.height();

        int szMargin = 0;
        int szIcon = 0;
        /*if (mlContext.pMedia)
        {
            szMargin = cy * context.fIconMargin * m_medialibDlg.rowCount() / getRowCount();
            szIcon = cy - szMargin*2;
        }
        else*/
        {
            //szMargin = context.nIconSize;
            szIcon = __size(80);
            szMargin = (cy - szIcon)/2;
        }

        int xIcon = context->rc.right() + __playIconOffset - szIcon;
        int yIcon = context->rc.top()+szMargin;
        QRect rcPlayIcon(xIcon, yIcon, szIcon, szIcon);

        bool bFlash = (int)mlContext->uItem == m_nFlashItem;
        cauto pm = mlContext.pMediaSet
                ?(bFlash?m_pmPlayOpacity:m_pmPlay)
               :(bFlash?m_pmAddPlayOpacity:m_pmAddPlay);
        painter.drawPixmap(rcPlayIcon, pm);

        rc.setRight(xIcon-__lvRowMargin+__playIconOffset);
    }

    QString qsMediaQuality;
    WString strRemark;
    if (mlContext.pMedia)
    {
        if (mlContext.pMedia->GetMediaSetType() == E_MediaSetType::MST_Album)
        {
            rc.setLeft(rc.left() + __size(10));
        }
    }
    else if (mlContext.pMediaSet)
    {
        if (E_MediaSetType::MST_SingerGroup == mlContext.pMediaSet->m_eType)
        {
            auto pSingerGroup = (CSingerGroup*)mlContext.pMediaSet;
            size_t uAlbumCount = 0;
            cauto lstSingers = pSingerGroup->singers();
            for (cauto singer : lstSingers)
            {
                uAlbumCount += singer.albums().size();
            }
            strRemark << lstSingers.size() << L" 歌手\n" << uAlbumCount << L" 专辑";
        }
        else if (E_MediaSetType::MST_Singer == mlContext.pMediaSet->m_eType)
        {
            auto& lstAlbum = ((CSinger*)mlContext.pMediaSet)->albums();
            size_t uAlbumItemCount = 0;
            for (CAlbum& album : lstAlbum)
            {
                uAlbumItemCount += album.albumItems().size();
            }
            strRemark << lstAlbum.size() << L" 专辑\n" << uAlbumItemCount << L" 曲目";
        }
        else if (E_MediaSetType::MST_Album == mlContext.pMediaSet->m_eType)
        {
            auto pAlbum = (CAlbum*)mlContext.pMediaSet;
            cauto strLanguageName = pAlbum->languageName();
            if (!strLanguageName.empty())
            {
                strRemark << strLanguageName << '\n';
            }
            strRemark << pAlbum->albumItems().size() << L" 曲目";
        }
        else if (E_MediaSetType::MST_Playlist == mlContext.pMediaSet->m_eType)
        {
            auto pPlaylist = (CPlaylist*)mlContext.pMediaSet;
            strRemark << pPlaylist->size() << L" 曲目";
        }
    }
    if (!strRemark->empty())
    {
        _paintRemark(painter, rc, strRemark);
    }
    else
    {
        IMedia *pMedia = mlContext.pMedia;
        if (pMedia)
        {
            qsMediaQuality = mediaQualityString(pMedia->quality());
            rc.setRight(rc.right() - __size(20) - __size(10)*qsMediaQuality.length());
        }
        else
        {
            auto pMediaRes = (CMediaRes*)mlContext.pFile;
            if (pMediaRes)
            {
                pMedia = pMediaRes;

                if (pMediaRes->parent()->dirType() == E_MediaDirType::MDT_Snapshot)
                {
                    auto duration = pMediaRes->duration();
                    cauto cue = ((CSnapshotMediaRes*)pMediaRes)->cueFile();
                    if (cue)
                    {
                        WString strRemark;
                        strRemark << IMedia::genDurationString(duration) << '\n' << cue.m_alTrackInfo.size() << L"曲目";
                        _paintRemark(painter, rc, strRemark);

                        //QColor cr = g_crFore;
                        //cr.setAlpha(CPainter::oppTextAlpha(__RemarkAlpha)/2);
                        //painter.drawRectEx(QRect(rcRet.left(), rcRet.bottom()+1, rcRet.width(), 1), cr);

                        rc.setRight(rc.right() - __size(120));
                    }
                    else if (duration > __wholeTrackDuration)
                    {
                        _paintRemark(painter, rc, __WS2Q(L'\n' + IMedia::genDurationString(duration)));
                        rc.setRight(rc.right() - __size(100));
                    }
                }
            }
        }
    }

    uTextAlpha = 255;
    uShadowAlpha = __ShadowAlpha;
    if ((int)mlContext->uItem == m_nFlashItem)
    {
        uTextAlpha = __FlashingAlpha;
        uShadowAlpha = uShadowAlpha*__FlashingAlpha/300;
    }

    cauto rcRet = CListView::_paintText(context, painter, rc, flags, uShadowAlpha, uTextAlpha);

    if (!qsMediaQuality.isEmpty())
    {
        CPainterFontGuard fontGuard(painter, 0.7, QFont::Weight::Thin);

        auto rcPos = rcRet;
        rcPos.setLeft(rcPos.right() + __size(20));
        rcPos.setTop(rcPos.top() - __size(9));
        rcPos.setRight(10000);
        painter.drawTextEx(rcPos, Qt::AlignLeft|Qt::AlignTop, qsMediaQuality, 1, uShadowAlpha, uTextAlpha);
    }

    return rcRet;
}

inline static bool _hittestPlayIcon(const tagMLItemContext& context, int x)
{
    if (!context.playable())
    {
        return false;
    }

    int xPlayIcon = context->rc.width() - __lvRowMargin + __playIconOffset - context->rc.height();
    return x >= xPlayIcon;
}

void CMedialibView::_onItemClick(tagLVItem& lvItem, const QMouseEvent& me, CMediaSet& mediaSet)
{
    if (_hittestPlayIcon(tagMLItemContext(lvItem, mediaSet), me.x()))
    {
        _flashItem(lvItem.uItem);

        TD_MediaList lstMedias;
        mediaSet.GetAllMedias(lstMedias);
        m_app.getCtrl().callPlayCmd(tagAssignMediaCmd(TD_IMediaList(lstMedias)));

        return;
    }

    CMLListView::_onItemClick(lvItem, me, mediaSet);
}

void CMedialibView::_onItemClick(tagLVItem& lvItem, const QMouseEvent& me, CPath& path)
{
    auto& mediaRes = (CMediaRes&)path;
    if (!_hittestPlayIcon(tagMLItemContext(lvItem, (IMedia&)mediaRes), me.x()))
    {
        if (m_medialibDlg.tryShowWholeTrack(mediaRes))
        {
            selectItem(lvItem.uItem);
            return;
        }
    }

    _onMediaClick(lvItem, me, mediaRes);
}

void CMedialibView::_onMediaClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media)
{
    if (_hittestPlayIcon(tagMLItemContext(lvItem, media), me.x()))
    {
        _flashItem(lvItem.uItem);

        if (m_app.getPlayMgr().playStatus() != E_PlayStatus::PS_Play)
        {
            m_app.getCtrl().callPlayCmd(tagPlayMediaCmd(media));
        }
        else
        {
            m_app.getCtrl().callPlayCmd(tagAppendMediaCmd(media));
        }

        return;
    }

    selectItem(lvItem.uItem);
    _flashItem(lvItem.uItem);

    if (me.x() <= (int)rowHeight())
    {
        if (media.type() == E_MediaType::MT_PlayItem)
        {
            auto pAlbumItem = media.findRelatedMedia(E_RelatedMediaSet::RMS_Album);
            if (pAlbumItem)
            {
                hittestMedia(*pAlbumItem);
                return;
            }
        }
    }

    m_app.getCtrl().callPlayCmd(tagPlayMediaCmd(media));
}

CMediaSet* CMedialibView::_onUpward(CMediaSet& currentMediaSet)
{
    //if (m_pthrFindRelated) m_pthrFindRelated->cancel(false);

    if (&currentMediaSet == &m_SingerLib || &currentMediaSet == &m_PlaylistLib)
    {
        return NULL;
    }

    return CMLListView::_onUpward(currentMediaSet);
}

CPath* CMedialibView::_onUpward(CPath& currentDir)
{
    if (NULL == currentDir.parent() && ((CMediaDir&)currentDir).dirType() == E_MediaDirType::MDT_Attach)
    {
        return &__medialib;
    }

    return CMLListView::_onUpward(currentDir);
}

void CMedialibView::_flashItem(UINT uItem, UINT uMSDelay)
{
    m_nFlashItem = uItem;
    update();

    CApp::async(uMSDelay, [&](){
        m_nFlashItem = -1;
        update();
    });
}

void CMedialibView::play()
{
    dselectItem();

    TD_IMediaList paMedias;
    CMediaSet *pMediaSet = currentMediaSet();
    if (pMediaSet)
    {
        TD_MediaList lstMedias;
        pMediaSet->GetAllMedias(lstMedias);
        paMedias.add(lstMedias);
    }
    else
    {
        CPath *pDir = currentDir();
        if (pDir)
        {
            pDir->files()([&](XFile& file){
                paMedias.add((CMediaRes&)file);
            });
        }
    }

    if (paMedias)
    {
        m_app.getCtrl().callPlayCmd(tagAssignMediaCmd(paMedias));
    }
}

void CMedialibView::cleanup()
{
    //if (m_pthrFindRelated) m_pthrFindRelated->cancel(false);

    m_mapSingerPixmap.clear();
    m_lstSingerPixmap.clear();

    CMLListView::_cleanup();
}
