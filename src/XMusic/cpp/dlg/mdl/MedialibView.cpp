
#include "xmusic.h"

#include "MedialibView.h"

#define __FlashingAlpha 170

#define __RemarkAlpha 200

#define __XpkRootName   L"离线音乐包"

#define __XSingerName   L" 歌手"
#define __XPlaylistName L" 歌单"
#define __XMusicDirName L"XMusic"
#define __LocalDirName  L" 本机"

#define __catDSD    L"直接比特流数字编码\nDirect Stream Digital"

#define __catHires  L"高解析音频  24~32Bit/96~192KHz\nHigh Resolution Audio"

#define __catMQS    L"录音棚级别无损  24Bit/96KHz\nMastering Quality Sound"

#define __catDTS    L"5.1声道  DTSDigitalSurround"

#define __catDisc   L"整轨"

#define __catCD     L"CD分轨  标准1411Kbps码率"
#define __catCD_t   L"CD分轨"

#define __catSQ     L"无损"

#define __catSQ24   L"24位无损  24Bit/48KHz"
#define __catSQ24_t L"24位无损"

#define __catPure   L"纯音乐"

inline void CMedialibView::_genDisplayTitle(const IMedia *pMedia, cwstr strSingerNamer)
{
    auto& strTitle = m_mapDisplayTitle[pMedia];
    if (strTitle.empty())
    {
        strTitle = pMedia->GetTitle();
        CFileTitle::genDisplayTitle(strTitle, &strSingerNamer);
    }
}

inline void CMedialibView::_genDisplayTitle(const IMedia *pMedia)
{
    auto& strTitle = m_mapDisplayTitle[pMedia];
    if (strTitle.empty())
    {
        strTitle = pMedia->GetTitle();
        CFileTitle::genDisplayTitle(strTitle);
    }
}

inline void CMedialibView::_genSingerMediaTitle(const IMedia *pMedia, CSinger& singer)
{
    cauto strSingerName = singer.m_strName;
    auto& strTitle = m_mapDisplayTitle[pMedia];
    if (strTitle.empty())
    {
        strTitle = pMedia->GetTitle();
        CFileTitle::genDisplayTitle(strTitle, &strSingerName);
        auto len = strSingerName.size()+3;
        if (strTitle.substr(0, len) == strSingerName + L" - ")
        {
            strTitle.erase(0, len);
        }
    }
}

CMedialibView::CMedialibView(CMedialibDlg& medialibDlg)
    : CMLListView(&medialibDlg, E_LVScrollBar::LVSB_Left)
    , m_medialibDlg(medialibDlg)
    , m_xpkRoot(__xmedialib.xpkRoot())
    , m_SingerLib(g_app.getSingerMgr())
    , m_PlaylistLib(g_app.getPlaylistMgr())
    , m_pmHDDisk(g_app.m_pmHDDisk)
    , m_pmSQDisk(g_app.m_pmSQDisk)
    , m_lpCatItem {
        {__mdlPng(dsd), __catDSD},
        {__mdlPng(hires), __catHires},
        {__mdlPng(mqs), __catMQS},
        {__mdlPng(dts), __catDTS},
        {__mdlPng(diskdir), __catDisc},
        {__mdlPng(compactDisc), __catCD, __catCD_t},
        {__mdlPng(sq), __catSQ, __catSQ},
        {__mdlPng(sq), __catSQ24, __catSQ24_t},
        {__mdlPng(pure), __catPure},
        {"", L""}
    }
{
}

void CMedialibView::initpm()
{
    (void)m_brNullSingerHead.setTexture(__mdlPng(singerdefault));

    (void)m_pmXpk.load(__mdlPng(xpk));

    (void)m_pmSingerGroup.load(__mdlPng(singergroup));
    (void)m_pmAlbum.load(__mdlPng(album));

    (void)m_pmPlaylistSet.load(__mdlPng(playlistset));
    (void)m_pmPlaylist.load(__mdlPng(playlist));
    (void)m_pmPlayItem.load(__mdlPng(playitem));

    (void)m_pmXmusicDir.load(__mdlPng(xmusicdir));

    (void)m_pmMedia.load(__mdlPng(media));
    //(void)m_pmXpkMedia.load(__mdlPng(xpkmedia));
    (void)m_pmFlac.load(__mdlPng(flac));

#if __android
    (void)m_pmLocalDir.load(__mdlPng(tf));
#elif __windows
    (void)m_pmLocalDir.load(__mdlPng(windriver));
#endif

    (void)m_pmDirLink.load(__mdlPng(dirLink));
    (void)m_pmDir.load(__mdlPng(dir));
    (void)m_pmFile.load(__mdlPng(file));

    (void)m_pmPlay.load(__png(btnPlay));
    (void)m_pmAddPlay.load(__png(btnAddplay));

    QMatrix matrix;
    matrix.rotate(-10);
#define __languageIconAlpha 185
    (void)m_pmCN.load(__mdlPng(cn));
    m_pmCN = CPainter::alphaPixmap(m_pmCN, __languageIconAlpha);
    m_pmCN = m_pmCN.transformed(matrix, Qt::SmoothTransformation);

    (void)m_pmHK.load(__mdlPng(hk));
    m_pmHK = CPainter::alphaPixmap(m_pmHK, __languageIconAlpha);
    m_pmHK = m_pmHK.transformed(matrix, Qt::SmoothTransformation);

    (void)m_pmKR.load(__mdlPng(kr));
    m_pmKR = CPainter::alphaPixmap(m_pmKR, __languageIconAlpha);
    m_pmKR = m_pmKR.transformed(matrix, Qt::SmoothTransformation);

    (void)m_pmJP.load(__mdlPng(jp));
    m_pmJP = CPainter::alphaPixmap(m_pmJP, __languageIconAlpha);
    m_pmJP = m_pmJP.transformed(matrix, Qt::SmoothTransformation);

    (void)m_pmEN.load(__mdlPng(en));
    m_pmEN = CPainter::alphaPixmap(m_pmEN, __languageIconAlpha);
    m_pmEN = m_pmEN.transformed(matrix, Qt::SmoothTransformation);
}

void CMedialibView::_onShowRoot()
{
    m_mapDisplayTitle.clear();

    m_medialibDlg.updateHead(L"媒体库");
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    m_mapDisplayTitle.clear();

    WString strTitle;
    auto pSinger = currentSinger();
    if (pSinger && &MediaSet != pSinger)
    {
        strTitle << pSinger->m_strName << __CNDot << MediaSet.name();
        if (!m_medialibDlg.isHLayout() && strutil::checkWordCount(strTitle) >= 18)
        {
            strTitle = MediaSet.name();
        }
    }
    else
    {
        if (MediaSet.m_pParent && MediaSet.m_pParent != &__xmedialib)
        {
            strTitle << MediaSet.m_pParent->m_strName << __CNDot << MediaSet.m_strName;
            if (!m_medialibDlg.isHLayout() && strutil::checkWordCount(strTitle) >= 18)
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

    if (pSinger)
    {
        for (auto pMedia : currentSubMedias())
        {
            _genSingerMediaTitle(pMedia, *pSinger);
        }
        return;
    }

    if (E_MediaSetType::MST_SingerGroup == MediaSet.m_eType)
    {
        list<UINT> lstSinger;
        for (cauto singer : ((CSingerGroup&)MediaSet).singers())
        {
            lstSinger.push_back(singer.m_uID);
        }
         g_app.getSingerImgMgr().downloadSingerHead(lstSinger);
        return;
    }

    if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
    {
        list<UINT> *plstSinger = NULL;
        cauto itr = m_PlaylistSinger.find(&MediaSet);
        if (itr != m_PlaylistSinger.end())
        {
            plstSinger = &itr->second;

            for (cauto PlayItem : ((CPlaylist&)MediaSet).playItems())
            {
                cauto strSingerName = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
                if (!strSingerName.empty())
                {
                    _genDisplayTitle(&PlayItem, strSingerName);
                }
                else
                {
                    _genDisplayTitle(&PlayItem);
                }
            }
        }
        else
        {
            plstSinger = &m_PlaylistSinger[&MediaSet];

            cauto singerMgr =  g_app.getSingerMgr();
            for (auto& PlayItem : ((CPlaylist&)MediaSet).playItems())
            {
                auto pSinger = singerMgr.checkSingerDir(PlayItem.GetPath());
                if (pSinger)
                {
                    PlayItem.SetRelatedMediaSet(E_RelatedMediaSet::RMS_Singer, pSinger->m_uID, pSinger->m_strName);
                    if (std::find(plstSinger->begin(), plstSinger->end(), pSinger->m_uID) == plstSinger->end())
                    {
                        plstSinger->push_back(pSinger->m_uID);
                    }

                    _genDisplayTitle(&PlayItem, pSinger->m_strName);
                }
                else
                {
                    _genDisplayTitle(&PlayItem);
                }
            }
        }

        if (!plstSinger->empty())
        {
             g_app.getSingerImgMgr().downloadSingerHead(*plstSinger);
        }
    }
}

void CMedialibView::_onShowDir(CPath& dir)
{
    m_mapDisplayTitle.clear();

    if (&dir == &m_xpkRoot)
    {
        m_medialibDlg.updateHead(__XpkRootName);
        return;
    }
    else if (&dir == &__medialib)
    {
        m_medialibDlg.updateHead(__XMusicDirName);
        return;
    }
    else if (&dir == &m_LocalDir)
    {
        m_medialibDlg.updateHead(strutil::ltrim_r(wstring(__LocalDirName)));
        return;
    }
    //非根目录 else {

    auto strTitle = dir.fileName();
    if (dir.rootDir() == &m_LocalDir)
    {
        m_medialibDlg.updateHead(strTitle);
        return;
    }

    auto pSnapshotDir = _snapshotDir(dir);
    if (pSnapshotDir && dir.parent() == &__medialib)
    {
        cauto strCatTitle = _catTitle(*pSnapshotDir);
        if (!strCatTitle.empty())
        {
            strTitle = strCatTitle;
        }
        else
        {
            strTitle.erase(0, 3);
        }
    }
    m_medialibDlg.updateHead(strTitle);

    //目录歌手图标
    list<UINT> *plstSinger = NULL;
    cauto itr = m_mapDirSinger.find(&dir);
    if (itr != m_mapDirSinger.end())
    {
        plstSinger = &itr->second;
    }
    else
    {
        plstSinger = &m_mapDirSinger[&dir];

        for (auto pSubDir : dir.dirs())
        {
            auto uSingerID = ((CMediaDir*)pSubDir)->GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
            if (uSingerID)
            {
                plstSinger->push_back(uSingerID);
            }
        }
    }

    if (!plstSinger->empty())
    {
         g_app.getSingerImgMgr().downloadSingerHead(*plstSinger);
    }

    //文件标题
    CSinger *pSinger = NULL;
    if (pSnapshotDir)
    {
        pSinger = pSnapshotDir->singer();
    }
    else
    {
        pSinger = g_app.getSingerMgr().checkSingerDir(((CMediaDir&)dir).GetPath());
    }
    if (pSinger)
    {
        cauto strSingerName = pSinger->m_strName;
        for (auto pSubFile : dir.files())
        {
            _genDisplayTitle((CMediaRes*)pSubFile, strSingerName);
        }
    }
    else
    {
        for (auto pSubFile : dir.files())
        {
            _genDisplayTitle((CMediaRes*)pSubFile);
        }
    }
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

    auto pDir = currentDir();
    if (pDir != NULL || currentMediaSet() != NULL)
    {
        if (pDir == &__medialib)
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

#define __IconSize __size100

void CMedialibView::_genMediaSetContext(tagMLItemContext& context, CMediaSet& MediaSet)
{
    switch (MediaSet.m_eType)
    {
    case E_MediaSetType::MST_Playlist:
        context.setIcon(m_pmPlaylist);
        context.strRemark << ((CPlaylist&)MediaSet).size() << L" 首";

        break;
    case E_MediaSetType::MST_Album:
    {
        context.setIcon(m_pmAlbum);

        cauto strLanguageName = MediaSet.languageName();
        if (!strLanguageName.empty())
        {
            context.strRemark << strLanguageName << '\n';
        }

        auto uCount = ((CAlbum&)MediaSet).albumItems().size();
        context.strRemark << uCount;
        if (uCount < 10)
        {
            context.strRemark << ' ';
        }
        context.strRemark << L"首";
    }

    break;
    case E_MediaSetType::MST_Singer:
    {
        auto& brSingerHead = genSingerHead(MediaSet.m_uID);
        context.setSingerIcon(brSingerHead);

        auto& lstAlbum = ((CSinger&)MediaSet).albums();
        size_t uAlbumItemCount = 0;
        for (auto& album : lstAlbum)
        {
            uAlbumItemCount += album.albumItems().size();
        }
        context.strRemark << lstAlbum.size() << L" 专辑\n" << uAlbumItemCount << L" 曲目";
    }

    break;
    case E_MediaSetType::MST_SingerGroup:
    {
        context.setIcon(m_pmSingerGroup);
        context.uStyle |= E_LVItemStyle::IS_ForwardButton;

        cauto lstSingers = ((CSingerGroup&)MediaSet).singers();
        size_t uAlbumCount = 0;
        for (cauto singer : lstSingers)
        {
            uAlbumCount += singer.albums().size();
        }
        context.strRemark << lstSingers.size() << L" 歌手\n" << uAlbumCount << L" 专辑";
    }

    break;
    default:
        if (E_MediaSetType::MST_SnapshotMediaDir == MediaSet.m_eType)
        {
            context.uStyle |= E_LVItemStyle::IS_ForwardButton;

            cauto dir = (CSnapshotDir&)MediaSet;
            context.setIcon(_catIcon(dir));

            auto uCount = dir.count();
            //if (uCount > 0)
            {
                context.strRemark << uCount << L" 项";
            }
        }

        break;
    };
}

void CMedialibView::_genMediaContext(tagMLItemContext& context, IMedia& Media)
{
    auto eMediaType = Media.type();
    if (E_MediaType::MT_PlayItem == eMediaType)
    {
        context.setIcon(m_pmPlayItem);

        auto uSingerID = Media.GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
        if (uSingerID)
        {
            auto& brSingerHead = genSingerHead(uSingerID);
            context.setSingerIcon(brSingerHead);
        }
    }
    else if (E_MediaType::MT_MediaRes == eMediaType)
    {
        context.setIcon(m_pmMedia);
    }
    context.strText = m_mapDisplayTitle[&Media];
}

void CMedialibView::_genFileContext(tagMLItemContext& context, XFile& file)
{
    context.setIcon(m_pmFile);

    auto& MediaRes = (CMediaRes&)file;
    auto pRootDir = file.rootDir();
    if (pRootDir == &m_LocalDir)
    {
        context.strText = MediaRes.GetName();
    }
    else
    {
        context.setIcon(m_pmMedia);

        context.strText = m_mapDisplayTitle[&MediaRes];
        auto pSnapshotDir = _snapshotDir(MediaRes);
        if (pSnapshotDir)
        {
            auto catType = pSnapshotDir->catType();
            if (E_SSCatType::CT_SQ == catType || E_SSCatType::CT_SQ24 == catType)
            {
                context.setIcon(m_pmFlac);
            }
        }
        /*else// if (pRootDir == &m_xpkRoot)
        {
            context.setIcon(m_pmXpkMedia);
        }*/
    }
}

void CMedialibView::_genDirContext(tagMLItemContext& context, CPath& dir)
{
    context.uStyle |= E_LVItemStyle::IS_ForwardButton;

    auto pParentDir = dir.parent();
    if (dir.rootDir() == &m_LocalDir)
    {
        context.setIcon(m_pmDir);

        if (NULL == pParentDir)
        {
            CAttachDir *pAttachDir = dynamic_cast<CAttachDir*>(&dir);
            if (pAttachDir)
            {
                context.setIcon(m_pmDirLink);

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
#if __android || __windows
        else if (pParentDir == &m_LocalDir) {
            if (dynamic_cast<CLocalDir*>(&dir)) //windows驱动器、安卓tf卡
            {
                context.setIcon(m_pmLocalDir);
            }
        }
#endif

        return;
    }

    auto uCount = dir.count();
    if (uCount > 0)
    {
        context.strRemark << uCount << L" 项";
    }

    if (dir.rootDir() == &m_xpkRoot)
    {
       context.setIcon(m_pmXpk);

       if (pParentDir == &m_xpkRoot)
       {
           context.nIconSize *= 1.13f;
           return;
       }
    }
    else
    {
        auto catItem = _catItem(dir);
        if (catItem)
        {
            context.setIcon(catItem->pmIcon);

            if (pParentDir == &__medialib)
            {
                context.nIconSize *= 1.13f;
                context.strText = catItem->strCatDetail;
                return;
            }
        }
    }

    auto uSingerID = ((CMediaDir&)dir).GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
    if (uSingerID)
    {
        auto& brSingerHead = genSingerHead(uSingerID);
        context.setSingerIcon(brSingerHead);
    }
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context)
{
    if (context.pMediaSet)
    {
        _genMediaSetContext(context,*context.pMediaSet);
    }
    else if (context.pMedia)
    {
        _genMediaContext(context, *context.pMedia);
    }
    else if (context.pFile)
    {
        _genFileContext(context, *context.pFile);
    }
    else if (context.pDir)
    {
        _genDirContext(context, *context.pDir);
    }
    else
    {
        context.uStyle = E_LVItemStyle::IS_CenterAlign;

        bool bHLayout = m_medialibDlg.isHLayout();

        //context.fIconMargin *= .9f * m_medialibDlg.rowCount()/this->getRowCount();
#define __szIcon __size100 * 1.15

        cauto uRow = context->uRow;
        if ((bHLayout && 1 == uRow && 0 == context->uCol) || (!bHLayout && 1 == uRow))
        {
            context.setIcon(m_pmSingerGroup, __szIcon);
            context.strText = __XSingerName;
            context.pMediaSet = &m_SingerLib;
        }
        else if ((bHLayout && 1 == uRow && 1 == context->uCol) || (!bHLayout && 3 == uRow))
        {
            context.setIcon(m_pmPlaylistSet, __szIcon);
            context.strText = __XPlaylistName;
            context.pMediaSet = &m_PlaylistLib;
        }
        else if ((bHLayout && 3 == uRow && 0 == context->uCol) || (!bHLayout && 5 == uRow))
        {
            context.setIcon(m_pmXmusicDir, __szIcon);
            context.strText = __XMusicDirName;
            context.pDir = &__medialib;
        }
        else if ((bHLayout && 3 == uRow && 1 == context->uCol) || (!bHLayout && 7 == uRow))
        {
            context.setIcon(m_pmDir, __szIcon);
            context.strText << ' ' << __LocalDirName;
            context.pDir = &m_LocalDir;
        }
    }

    auto pSnapshotMedia = context.snapshotMedia();
    if (pSnapshotMedia)
    {
        auto eTrackType = pSnapshotMedia->trackType();
        if (E_TrackType::TT_Single != eTrackType)
        {
            context.setIcon(E_TrackType::TT_HDWhole == eTrackType ? m_pmHDDisk : m_pmSQDisk);

            cauto cue = pSnapshotMedia->cueFile();
            if (cue)
            {
                context.strRemark << cue.m_alTrackInfo.size() << L" 首\n";
            }
            context.strRemark << pSnapshotMedia->durationString();
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
        list<UINT> lstSinger;
        for (auto uItem : currentItems())
        {
            currentSubMedias().get(uItem, [&](IMedia& media){
                auto uSingerID = media.GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
                if (uSingerID)
                {
                    lstSinger.push_back(uSingerID);
                }
            });
        }

        if (!lstSinger.empty())
        {
             g_app.getSingerImgMgr().downloadSingerHead(lstSinger);
        }
    }
    else if (E_MediaSetType::MST_SingerGroup == pMediaSet->m_eType)
    {
        list<UINT> lstSinger;
        cauto paSinger = currentSubSets();
        for (cauto uItem : currentItems())
        {
            paSinger.get(uItem, [&](CMediaSet& singer){
                lstSinger.push_back(singer.m_uID);
            });
        }
         g_app.getSingerImgMgr().downloadSingerHead(lstSinger);
    }
}

void CMedialibView::_paintIcon(tagLVItemContext& context, CPainter& painter, cqrc rc)
{
    CListView::_paintIcon(context, painter, rc);

    wstring strLanguage;
    cauto mlContext = (tagMLItemContext&)context;
    if (mlContext.pMediaSet)
    {
        if (mlContext.pMediaSet->m_pParent != &m_SingerLib && mlContext.pMediaSet->m_pParent != &m_PlaylistLib)
        {
            return;
        }
        strLanguage = mlContext.pMediaSet->m_strName.substr(0, 2);
    }
    else if (mlContext.pDir)
    {
        auto pParent = mlContext.pDir->parent();
        if (NULL == pParent || pParent->parent() != &__medialib)
        {
            return;
        }
        strLanguage = mlContext.pDir->fileName().substr(0, 2);
    }
    else
    {
        return;
    }

    const QPixmap *pmLanguage = NULL;
    if (L"国语" == strLanguage || L"华语" == strLanguage)
    {
        pmLanguage = &m_pmCN;
    }
    else if (L"粤语" == strLanguage)
    {
        pmLanguage = &m_pmHK;
    }
    else if (L"韩语" == strLanguage)
    {
        pmLanguage = &m_pmKR;
    }
    else if (L"日语" == strLanguage)
    {
        pmLanguage = &m_pmJP;
    }
    else if (L"英文" == strLanguage)
    {
        pmLanguage = &m_pmEN;
    }
    else
    {
        return;
    }
#define __szLanguageIcon __size(54)
    QRect rcLanguage(rc.right()-__szLanguageIcon/2, rc.y()-__szLanguageIcon/3, __szLanguageIcon, __szLanguageIcon);
    painter.drawImg(rcLanguage, *pmLanguage);
}

#define __rAlign Qt::AlignRight|Qt::AlignVCenter

cqrc CMedialibView::_paintText(tagLVItemContext& context, CPainter& painter, QRect& rc
                               , int flags, UINT uShadowAlpha, UINT uTextAlpha)
{
    cauto rcRow = context->rc;

    cauto mlContext = (tagMLItemContext&)context;
    if (mlContext.playable())
    {
        UINT cy = rcRow.height();

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
            szIcon = __size(78);
            szMargin = (cy - szIcon)/2;
        }

        int xIcon = rcRow.right() + __playIconOffset - szIcon;
        int yIcon = rcRow.top()+szMargin;
        QRect rcPlayIcon(xIcon, yIcon, szIcon, szIcon);

        cauto pm = mlContext.pMediaSet ? m_pmPlay : m_pmAddPlay;

        bool bFlash = (int)mlContext->uItem == m_nFlashItem;
        if (bFlash)
        {
            painter.setOpacity(0.5f);
        }
        painter.drawImg(rcPlayIcon, pm);
        if (bFlash)
        {
            painter.setOpacity(1);
        }

        rc.setRight(xIcon-__lvRowMargin+__playIconOffset);
    }

    if (!mlContext.strRemark->empty())
    {
        CPainterFontGuard fontGuard(painter, 0.81, TD_FontWeight::ExtraLight);
        UINT uAlpha = CPainter::oppTextAlpha(__RemarkAlpha);
        cauto rcRet = painter.drawTextEx(rc, __rAlign, mlContext.strRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);
        rc.setRight(rcRet.x() - __size(20));
    }

    const QString *pqsQuality = NULL;
    IMedia *pMedia = NULL;
    if (mlContext.pMedia)
    {
        if (E_MediaType::MT_AlbumItem == mlContext.pMedia->type())
        {
            rc.setLeft(rc.left() + __size10);
        }

        pMedia = mlContext.pMedia;
    }
    else
    {
        pMedia = (CMediaRes*)mlContext.pFile;
    }

    if (pMedia)
    {
        if (E_MediaType::MT_MediaRes != pMedia->type()
                || ((CMediaRes*)pMedia)->rootDir() != &m_LocalDir)
        {
            cauto qsQuality = mediaQualityString(*pMedia);
            if (!qsQuality.isEmpty())
            {
                pqsQuality = &qsQuality;
                rc.setRight(rc.right() - __size(20) - __size10*qsQuality.length());
            }
        }
    }

    //uTextAlpha = 255;
    //uShadowAlpha = __ShadowAlpha;
    do {
        if (mlContext.pMediaSet)
        {
            if (E_MediaSetType::MST_SnapshotMediaDir == mlContext.pMediaSet->m_eType
                && !((CSnapshotDir*)mlContext.pMediaSet)->available)
            {
                uShadowAlpha /= 2;
                uTextAlpha /= 2;
                break;
            }
        }
        else if (mlContext.pDir)
        {
            auto pSnapshotDir = _snapshotDir(*mlContext.pDir);
            if (pSnapshotDir && !pSnapshotDir->available)
            {
                uShadowAlpha /= 2;
                uTextAlpha /= 2;
                break;
            }
        }
        else
        {
            auto pSnapshotMedia = mlContext.snapshotMedia();
            if (pSnapshotMedia && !pSnapshotMedia->available)
            {
                uShadowAlpha /= 2;
                uTextAlpha /= 2;
                break;
            }
        }

        if ((int)mlContext->uItem == m_nFlashItem)
        {
            uShadowAlpha = uShadowAlpha*__FlashingAlpha/300;
            uTextAlpha = __FlashingAlpha;
        }
    } while(0);

    // xmusic下文本换行
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

    if (pqsQuality)
    {
        CPainterFontGuard fontGuard(painter, 0.69, TD_FontWeight::Thin);

        auto rcPos = rcRet;
        rcPos.setLeft(rcPos.right() + __size(20));
        rcPos.setTop(rcPos.top() - __size(9));
        rcPos.setRight(10000);
        painter.drawTextEx(rcPos, Qt::AlignLeft|Qt::AlignTop, *pqsQuality, 1, uShadowAlpha, uTextAlpha);
    }

    return rcRet;
}

CBrush& CMedialibView::genSingerHead(UINT uSingerID)
{
    auto& pbrSingerHead = m_mapSingerHead[uSingerID];
    if (pbrSingerHead)
    {
        return *pbrSingerHead;
    }

    auto pHeadImg =  g_app.getSingerImgMgr().getSingerHead(uSingerID);
    if (NULL == pHeadImg)
    {
        pbrSingerHead = &m_brNullSingerHead;
    }
    else if (pHeadImg->bExist)
    {
        cauto qsFile = __WS2Q( g_app.getSingerImgMgr().file(*pHeadImg));
        m_lstSingerHead.emplace_back(qsFile);
        pbrSingerHead = &m_lstSingerHead.back();

        /*QPixmap pm(qsFile);
#define __singerimgZoomout 128
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
        else
        {
            pSingerPixmap->swap(pm);
        }*/

        return *pbrSingerHead;
    }

    return m_brNullSingerHead;
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

void CMedialibView::_onMediasetClick(tagLVItem& lvItem, const QMouseEvent& me, CMediaSet& mediaSet)
{
    if (_hittestPlayIcon(tagMLItemContext(lvItem, mediaSet), me.x()))
    {
        _flashItem(lvItem.uItem);

         g_app.getCtrl().callPlayCmd(tagAssignMediaSetCmd(mediaSet));

        return;
    }

    CMLListView::_onMediasetClick(lvItem, me, mediaSet);
}

#define __checkIconArea (me.x() <= (int)rowHeight())

void CMedialibView::_onMediaClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media)
{
    if (__checkIconArea)
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

    tagMLItemContext context(lvItem, media);
    auto pSnapshotMedia = context.snapshotMedia();

    if (_hittestPlayIcon(context, me.x()))
    {
        if (pSnapshotMedia && !pSnapshotMedia->available)
        {
            return;
        }

        _flashItem(lvItem.uItem);

        if ( g_app.getPlayMgr().playStatus() != E_PlayStatus::PS_Play)
        {
             g_app.getCtrl().callPlayCmd(tagPlayMediaCmd(media));
        }
        else
        {
             g_app.getCtrl().callPlayCmd(tagAppendMediaCmd(media));
        }
    }
    else
    {
        if (m_medialibDlg.tryShowWholeTrack(media))
        {
            selectItem(lvItem.uItem);
            return;
        }

        if (pSnapshotMedia && !pSnapshotMedia->available)
        {
            return;
        }

        _flashItem(lvItem.uItem, true);

         g_app.getCtrl().callPlayCmd(tagPlayMediaCmd(media));
    }
}

void CMedialibView::_onDirClick(tagLVItem& lvItem, const QMouseEvent& me, CPath& dir)
{
#if __android
    if (&dir == &m_LocalDir)
    {
        if (!requestAndroidSDPermission())
        {
            return;
        }
    }
#endif

    if (dir.rootDir() == &m_LocalDir)
    {
        list<CPath*> lstRemove;
        for (auto pDir : dir.dirs())
        {
            if (!pDir->dirs() && !pDir->files())
            {
                lstRemove.push_back(pDir);
            }
        }
        for (auto pDir : lstRemove)
        {
            pDir->remove();
        }
    }
    else
    {
        do {
            if (!__checkIconArea)
            {
                break;
            }

            auto uSingerID = ((CMediaDir&)dir).GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
            if (0 == uSingerID)
            {
                break;
            }
            auto pSinger = g_app.getSingerMgr().getSinger(uSingerID);
            if (NULL == pSinger)
            {
                break;
            }

            showMediaSet(*pSinger);

            auto pSnapshotDir = _snapshotDir(dir);
            if (pSnapshotDir)
            {
                (void)_hittestMediaSet(*pSnapshotDir);
            }

            return;
        } while(0);
    }

    CMLListView::_onDirClick(lvItem, me, dir);
}

CMediaSet* CMedialibView::_onUpward(CMediaSet& currentMediaSet)
{
    if (&currentMediaSet == &m_SingerLib || &currentMediaSet == &m_PlaylistLib)
    {
        return NULL;
    }

    return CMLListView::_onUpward(currentMediaSet);
}

void CMedialibView::_flashItem(UINT uMSDelay, UINT uItem, bool bSelect)
{
    dselectItem();
    m_nFlashItem = uItem;
    update();

    async(uMSDelay, [=]{
        if (bSelect)
        {
            selectItem(m_nFlashItem);
        }
        m_nFlashItem = -1;
        update();
    });
}

void CMedialibView::cleanup()
{
    m_LocalDir.clear();

    m_mapDisplayTitle.clear();

    m_lstSingerHead.clear();
    m_mapSingerHead.clear();

    CMLListView::_cleanup();
}

CMediaRes* CMedialibView::hittestLocalFile(cwstr strPath)
{
    CMediaRes *pMediaRes = m_LocalDir.subFile(strPath);
    if (pMediaRes)
    {
        hittestFile(*pMediaRes);
    }
    return pMediaRes;
}

CPath* CLocalDir::_newSubDir(const tagFileInfo& fileInfo)
{
    if (fileInfo.strName.front() == __wcDot)
    {
        return NULL;
    }

    CMediaDir *pSubDir = new CMediaDir(fileInfo);
/*#if __windows
    if (m_fi.pParent && NULL == m_fi.pParent->parent())
#else
    if (NULL == m_fi.pParent)
#endif
    {
        if (!pSubDir->files())
        {
            cauto paDirs = pSubDir->dirs();
            if (paDirs.size() <= 1)
            {
                if (!paDirs.any([&](CPath& subDir){
                    return subDir.dirs() || subDir.files();
                }))
                {
                    delete pSubDir;
                    return NULL;
                }
            }
        }
    }*/

    return pSubDir;
}
