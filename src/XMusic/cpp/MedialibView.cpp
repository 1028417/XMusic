
#include "app.h"

#include "MedialibView.h"

#define __XMusicDirName L"XMusic"
#define __LocalDirName  L" 本机"

#define __FlashingAlpha 170

#define __RemarkAlpha 200

QPixmap g_pmDefaultSinger;

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
    (void)g_pmDefaultSinger.load(__mediaPng(singerdefault));
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

void CMedialibView::_onShowRoot()
{
    m_medialibDlg.updateHead(L"媒体库");
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    WString strTitle;
    auto pSinger = currentSinger();
    if (pSinger && &MediaSet != pSinger)
    {
        strTitle << pSinger->m_strName << __CNDot << MediaSet.name();

        if (!m_medialibDlg.isHLayout() && strutil::checkWordCount(strTitle) >= 14)
        {
            strTitle = MediaSet.name();
        }
    }
    else
    {
        if (MediaSet.m_pParent && MediaSet.m_pParent != &__xmedialib)
        {
            strTitle << MediaSet.m_pParent->m_strName;
            strTitle << __CNDot;
            strTitle << MediaSet.m_strName;

            if (!m_medialibDlg.isHLayout() && strutil::checkWordCount(strTitle) >= 14)
            {
                strTitle = MediaSet.m_strName;
            }
        }
        else
        {
            strTitle << MediaSet.m_strName;
        }
    }

    m_medialibDlg.updateHead(strTitle);

    if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
    {
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

            cauto singerMgr = m_app.getSingerMgr();
            for (auto& PlayItem : ((CPlaylist&)MediaSet).playItems())
            {
                auto pSinger = singerMgr.matchSingerDir(PlayItem.GetPath(), false);
                if (pSinger)
                {
                    PlayItem.SetRelatedMediaSet(E_RelatedMediaSet::RMS_Singer
                                                              , pSinger->m_uID, pSinger->m_strName);
                    if (std::find(plstSingerName->begin(), plstSingerName->end(), pSinger->m_strName) == plstSingerName->end())
                    {
                        plstSingerName->push_back(pSinger->m_strName);
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

void CMedialibView::_onShowDir(CPath& dir)
{
    wstring strTitle;
    if (&dir == &__medialib)
    {
        strTitle = __XMusicDirName;
    }
    else if (&dir == &m_OuterDir)
    {
        strTitle = __LocalDirName;
    }
    else
    {
        strTitle = dir.fileName();
        if (!((CMediaDir&)dir).isLocal() && dir.parent() == &__medialib)
        {
            strTitle = __substr(strTitle,3);
        }
    }

    m_medialibDlg.updateHead(strTitle);
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
    context.nIconSize = __size100;
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
            context.pmIcon = &genSingerHead(context.pMediaSet->m_uID, context.pMediaSet->m_strName);
            break;
        case E_MediaSetType::MST_SingerGroup:
            context.pmIcon = &m_pmSingerGroup;
            context.uStyle |= E_LVItemStyle::IS_ForwardButton;
            break;
        default:
            context.pmIcon = &m_pmSSDir;
            context.uStyle |= E_LVItemStyle::IS_ForwardButton;
            break;
        };
    }
    else if (context.pMedia)
    {
        if (context.pMedia->type() == E_MediaType::MT_PlayItem)
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
        else if (context.pMedia->type() != E_MediaType::MT_AlbumItem)
        {
            context.pmIcon = &m_pmSSFile;
        }
    }
    else if (context.pDir)
    {
        context.uStyle |= E_LVItemStyle::IS_ForwardButton;

        auto pMediaDir = (CMediaDir*)context.pDir;
        if (!pMediaDir->isLocal())
        {
            context.pmIcon = &m_pmSSDir;

            if (context.pDir->parent() == &__medialib)
            {
                auto strDirName = context.pDir->fileName();
                strDirName = __substr(strDirName,3);
                context.strText = strDirName;

                //context.fIconMargin *= .9f * m_medialibDlg.rowCount()/this->getRowCount();
                context.nIconSize *= 1.13f;

                if (strutil::matchIgnoreCase(strDirName, L"dsd"))
                {
                    context.pmIcon = &m_pmDSD;
                    context.strText = L"直接比特流数字编码\nDirect Stream Digital";
                }
                else if (strutil::matchIgnoreCase(strDirName, L"hi-res"))
                {
                    context.pmIcon = &m_pmHires;
                    context.strText = L"高解析音频\nHigh Resolution Audio";
                }
                else if (strutil::matchIgnoreCase(strDirName, L"mqs"))
                {
                    context.pmIcon = &m_pmMQS;
                    context.strText = L"录音棚级别无损音乐\nMastering Quality Sound";
                }
                else if (strutil::matchIgnoreCase(strDirName, L"dts"))
                {
                    context.pmIcon = &m_pmDTS;
                    context.strText = L"5.1声道\nDTSDigitalSurround";
                }
            }
            else
            {
                auto pSinger = m_app.getSingerMgr().matchSingerDir(pMediaDir->GetPath(), true);
                if (pSinger)
                {
                    context.pmIcon = &genSingerHead(pSinger->m_uID, pSinger->m_strName);
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
        if (!pMediaRes->parent()->isLocal())
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
        context.uStyle = E_LVItemStyle::IS_CenterAlign;

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
    while (pMediaSet)
    {
        if (E_MediaSetType::MST_Singer == pMediaSet->m_eType)
        {
            return (CSinger*)pMediaSet;
        }

        pMediaSet = pMediaSet->m_pParent;
    }

    return NULL;
}

void CMedialibView::_onPaint(CPainter& painter, int cx, int cy)
{
    CListView::_onPaint(painter, cx, cy);

    auto pMediaSet = currentMediaSet();
    if (NULL == pMediaSet)
    {
        return;
    }

    if (E_MediaSetType::MST_Playlist == pMediaSet->m_eType)
    {
        list<wstring> lstSingerName;
        for (auto uItem : currentItems())
        {
            currentSubMedias().get(uItem, [&](IMedia& media){
                cauto strSingerName = media.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
                if (!strSingerName.empty())
                {
                    lstSingerName.push_back(strSingerName);
                }
            });
        }

        if (!lstSingerName.empty())
        {
            m_app.getSingerImgMgr().downloadSingerHead(lstSingerName);
        }
    }
    else if (E_MediaSetType::MST_SingerGroup == pMediaSet->m_eType)
    {
        list<wstring> lstSingerName;
        cauto paSinger = currentSubSets();
        for (cauto uItem : currentItems())
        {
            paSinger.get(uItem, [&](CMediaSet& singer){
                lstSingerName.push_back(singer.m_strName);
            });
        }
        m_app.getSingerImgMgr().downloadSingerHead(lstSingerName);
    }
}

cqpm CMedialibView::genSingerHead(UINT uSingerID, cwstr strSingerName)
{
    auto& pSingerPixmap = m_mapSingerPixmap[uSingerID];
    if (pSingerPixmap)
    {
        return *pSingerPixmap;
    }

    auto pHeadImg = m_app.getSingerImgMgr().getSingerHead(strSingerName);
    if (NULL == pHeadImg)
    {
        pSingerPixmap = &g_pmDefaultSinger;
    }
    else if (pHeadImg->bExist)
    {
        m_lstSingerPixmap.emplace_back();
        pSingerPixmap = &m_lstSingerPixmap.back();

        QPixmap pm(__WS2Q(m_app.getSingerImgMgr().file(*pHeadImg)));
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
            pSingerPixmap->swap(pm);
        }

        return *pSingerPixmap;
    }

    return g_pmDefaultSinger;
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

    WString strRemark;
    if (mlContext.pMedia)
    {
        if (mlContext.pMedia->type() == E_MediaType::MT_AlbumItem)
        {
            rc.setLeft(rc.left() + __size10);
        }
    }
    else if (mlContext.pMediaSet)
    {
        if (E_MediaSetType::MST_Playlist == mlContext.pMediaSet->m_eType)
        {
            auto pPlaylist = (CPlaylist*)mlContext.pMediaSet;
            strRemark << pPlaylist->size() << L" 首";
        }
        else if (E_MediaSetType::MST_SingerGroup == mlContext.pMediaSet->m_eType)
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
            cauto strLanguageName = mlContext.pMediaSet->languageName();
            if (!strLanguageName.empty())
            {
                strRemark << strLanguageName << '\n';
            }

            auto uCount = ((CAlbum*)mlContext.pMediaSet)->albumItems().size();
            strRemark << uCount;
            if (uCount < 10)
            {
                strRemark << ' ';
            }
            strRemark << L"首";
        }        
        else if (E_MediaSetType::MST_SnapshotMediaDir == mlContext.pMediaSet->m_eType)
        {
            auto pDir = (CSnapshotMediaDir*)mlContext.pMediaSet;
            auto uCount = pDir->dirs().size() + pDir->files().size();
            if (uCount > 0)
            {
                strRemark << uCount << L" 项";
            }
        }
    }
    else if (mlContext.pFile)
    {
        auto pMediaRes = (CMediaRes*)mlContext.pFile;
        if (!pMediaRes->parent()->isLocal())
        {
            auto duration = pMediaRes->duration();
            if (duration > __wholeTrackDuration)
            {
                cauto cue = ((CSnapshotMediaRes*)pMediaRes)->cueFile();
                if (cue)
                {
                    strRemark << cue.m_alTrackInfo.size() << L" 首\n";
                }
                strRemark << IMedia::genDurationString(duration);
            }
        }
    }
    else if (mlContext.pDir && mlContext.pDir != &__medialib && mlContext.pDir != &m_OuterDir)
    {
        auto uCount = mlContext.pDir->dirs().size() + mlContext.pDir->files().size();
        if (uCount > 0)
        {
            strRemark << uCount << L" 项";
        }
    }

    if (!strRemark->empty())
    {
        cauto rcRet = _paintRemark(painter, rc, strRemark);
        rc.setRight(rcRet.x() - __size(20));
    }

    QString qsMediaQuality;
    auto pMedia = mlContext.pMedia;// ? (IMedia*)mlContext.pMedia : (CMediaRes*)mlContext.pFile;
    if (pMedia)
    {
        qsMediaQuality = mediaQualityString(pMedia->quality());
        rc.setRight(rc.right() - __size(20) - __size10*qsMediaQuality.length());
    }

    uTextAlpha = 255;
    uShadowAlpha = __ShadowAlpha;
    if ((int)mlContext->uItem == m_nFlashItem)
    {
        uTextAlpha = __FlashingAlpha;
        uShadowAlpha = uShadowAlpha*__FlashingAlpha/300;
    }

    if (mlContext.pDir && mlContext.pDir->parent() == &__medialib)
    {
        auto pos = mlContext.strText->find('\n');
        if (pos != __wnpos)
        {
            cauto qsText1 = __WS2Q(mlContext.strText->substr(0, pos));
            QRect rc1(rc);
            rc1.setBottom(rc.center().y() - __size10);
            painter.drawTextEx(rc1, Qt::AlignLeft|Qt::AlignBottom, qsText1, 1, uShadowAlpha, uTextAlpha);

            UINT uAlpha = CPainter::oppTextAlpha(__RemarkAlpha);
            cauto qsText2 = __WS2Q(mlContext.strText->substr(pos+1));
            QRect rc2(rc);
            rc2.setTop(rc.center().y() + __size10);
            return painter.drawTextEx(rc2, Qt::AlignLeft|Qt::AlignTop, qsText2, 1, __ShadowAlpha*uAlpha/255, uAlpha);
        }
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

        m_app.getCtrl().callPlayCmd(tagAssignMediaSetCmd(mediaSet));

        return;
    }

    CMLListView::_onItemClick(lvItem, me, mediaSet);
}

void CMedialibView::_onItemClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media)
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
            auto pAlbumItem = media.findRelatedAlbumItem();
            if (pAlbumItem)
            {
                hittestMedia(*pAlbumItem);
                return;
            }
        }
    }

    m_app.getCtrl().callPlayCmd(tagPlayMediaCmd(media));
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

    _onItemClick(lvItem, me, (IMedia&)mediaRes);
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
    if (NULL == currentDir.parent())
    {
        if (&currentDir != &__medialib && &currentDir != &m_OuterDir)
        {
            return &__medialib;
        }
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

void CMedialibView::cleanup()
{
    //if (m_pthrFindRelated) m_pthrFindRelated->cancel(false);

    m_mapSingerPixmap.clear();
    m_lstSingerPixmap.clear();

    CMLListView::_cleanup();
}
