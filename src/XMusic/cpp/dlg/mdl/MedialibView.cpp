
#include "xmusic.h"

#include "MedialibView.h"

#define __FlashingAlpha 170

#define __RemarkAlpha 200

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
#define __catSQ     L"无损"
#define __catSQ24   L"24位无损  24Bit/48KHz"
#define __catPure   L"纯音乐"

inline void CMedialibView::_genDisplayTitle(const IMedia *pMedia, const wstring *pstrSingerName)
{
    auto& strTitle = m_mapDisplayName[pMedia];
    if (strTitle.empty())
    {
        strTitle = pMedia->GetTitle();
        CFileTitle::genDisplayTitle(strTitle, pstrSingerName);
    }
}

inline void CMedialibView::_genDisplayTitle(const IMedia *pMedia)
{
    auto& strTitle = m_mapDisplayName[pMedia];
    if (strTitle.empty())
    {
        strTitle = pMedia->GetTitle();
        CFileTitle::genDisplayTitle(strTitle);
    }
}

inline void CMedialibView::_genSingerMediaTitle(const IMedia *pMedia, cwstr strSingerName)
{
    auto& strTitle = m_mapDisplayName[pMedia];
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
    , m_SingerLib(g_app.getSingerMgr())
    , m_PlaylistLib(g_app.getPlaylistMgr())
    , m_lpCatItem {
        {__mdlPng(dsd), __catDSD},
        {__mdlPng(hires), __catHires},
        {__mdlPng(mqs), __catMQS},
        {__mdlPng(dts), __catDTS},
        {__mdlPng(diskdir), __catDisc},
        {__mdlPng(compactDisc), __catCD},
        {__mdlPng(sq), __catSQ},
        {__mdlPng(sq), __catSQ24},
        {__mdlPng(pure), __catPure},
        {__mdlPng(ssdir), L""}
    }
    , m_pmHDDisk(g_app.m_pmHDDisk)
    , m_pmSQDisk(g_app.m_pmSQDisk)
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

    (void)m_pmSSFile.load(__mdlPng(media));

#if __android
    (void)m_pmOuterDir.load(__mdlPng(tf));
#elif __windows
    (void)m_pmOuterDir.load(__mdlPng(windriver));
#endif

    (void)m_pmDirLink.load(__mdlPng(dirLink));
    (void)m_pmDir.load(__mdlPng(dir));
    (void)m_pmFile.load(__mdlPng(file));

    (void)m_pmPlay.load(__png(btnPlay));
    m_pmPlayOpacity = CPainter::alphaPixmap(m_pmPlay, 128);
    (void)m_pmAddPlay.load(__png(btnAddplay));
    m_pmAddPlayOpacity = CPainter::alphaPixmap(m_pmAddPlay, 128);

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
    m_mapDisplayName.clear();

    m_medialibDlg.updateHead(L"媒体库");
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    m_mapDisplayName.clear();

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

    if (E_MediaSetType::MST_SingerGroup == MediaSet.m_eType)
    {
        list<wstring> lstSingerName;
        for (cauto singer : ((CSingerGroup&)MediaSet).singers())
        {
            lstSingerName.push_back(singer.m_strName);
        }
         g_app.getSingerImgMgr().downloadSingerHead(lstSingerName);
        return;
    }

    if (E_MediaSetType::MST_Album == MediaSet.m_eType)
    {
        cauto strSingerName = ((CAlbum&)MediaSet).GetSinger().m_strName;
        for (cauto AlbumItem : ((CAlbum&)MediaSet).albumItems())
        {
            _genSingerMediaTitle(&AlbumItem, strSingerName);
        }
        return;
    }

    if (E_MediaSetType::MST_SnapshotMediaDir == MediaSet.m_eType)
    {
        auto pSinger = currentSinger();
        if (pSinger)
        {
            for (auto pSubFile : ((CSnapshotMediaDir&)MediaSet).files())
            {
                _genSingerMediaTitle((CMediaRes*)pSubFile, pSinger->m_strName);
            }
        }
        return;
    }

    if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
    {
        list<wstring> *plstSingerName = NULL;
        cauto itr = m_PlaylistSinger.find(&MediaSet);
        if (itr != m_PlaylistSinger.end())
        {
            plstSingerName = &itr->second;

            for (auto& PlayItem : ((CPlaylist&)MediaSet).playItems())
            {
                cauto strSingerName = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
                if (strSingerName.empty())
                {
                    _genDisplayTitle(&PlayItem);
                }
                else
                {
                    _genDisplayTitle(&PlayItem, &strSingerName);
                }
            }
        }
        else
        {
            plstSingerName = &m_PlaylistSinger[&MediaSet];

            cauto singerMgr =  g_app.getSingerMgr();
            for (auto& PlayItem : ((CPlaylist&)MediaSet).playItems())
            {
                auto pSinger = singerMgr.checkSingerDir(PlayItem.GetPath());
                if (pSinger)
                {
                    PlayItem.SetRelatedMediaSet(E_RelatedMediaSet::RMS_Singer
                                                              , pSinger->m_uID, pSinger->m_strName);
                    if (std::find(plstSingerName->begin(), plstSingerName->end(), pSinger->m_strName) == plstSingerName->end())
                    {
                        plstSingerName->push_back(pSinger->m_strName);
                    }
                    _genDisplayTitle(&PlayItem, &pSinger->m_strName);
                }
                else
                {
                    _genDisplayTitle(&PlayItem);
                }
            }
        }

        if (!plstSingerName->empty())
        {
             g_app.getSingerImgMgr().downloadSingerHead(*plstSingerName);
        }
    }
}

void CMedialibView::_onShowDir(CPath& dir)
{
    m_mapDisplayName.clear();

    wstring strTitle;
    if (&dir == &__medialib)
    {
        strTitle = __XMusicDirName;
    }
    else if (&dir == &m_OuterDir)
    {
        strTitle = __LocalDirName;
        strutil::ltrim(strTitle);
    }
    else
    {
        strTitle = dir.fileName();
        auto pSnapshotMediaDir = (CSnapshotMediaDir*)((CMediaDir&)dir).mediaSet();
        if (pSnapshotMediaDir)
        {
            if (NULL == pSnapshotMediaDir->m_pParent)// dir.parent() == &__medialib)
            {
                strTitle.erase(0, 3);
            }

            //文件标题
            auto pSinger = pSnapshotMediaDir->singer();
            if (pSinger)
            {
                for (auto pSubFile : dir.files())
                {
                    _genDisplayTitle((CMediaRes*)pSubFile, &pSinger->m_strName);
                }
            }
            else
            {
                for (auto pSubFile : dir.files())
                {
                    _genDisplayTitle((CMediaRes*)pSubFile);
                }
            }

            //目录图标
            list<wstring> *plstSingerName = NULL;
            cauto itr = m_mapDirSinger.find(&dir);
            if (itr != m_mapDirSinger.end())
            {
                plstSingerName = &itr->second;
            }
            else
            {
                plstSingerName = &m_mapDirSinger[&dir];

                for (auto pSubDir : dir.dirs())
                {
                    cauto strSingerName = ((CMediaDir*)pSubDir)->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
                    if (!strSingerName.empty())
                    {
                        plstSingerName->push_back(strSingerName);
                    }
                }
            }

            if (!plstSingerName->empty())
            {
                 g_app.getSingerImgMgr().downloadSingerHead(*plstSingerName);
            }
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

#define __IconSize __size100

void CMedialibView::_genMLItemContext(tagMLItemContext& context, CMediaSet& MediaSet)
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
        auto& brSingerHead = genSingerHead(MediaSet.m_uID, MediaSet.m_strName);
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

            auto catType = ((CSnapshotMediaDir&)MediaSet).catType();
            context.setIcon(_catIcon(catType));

            auto uCount = ((CSnapshotMediaDir&)MediaSet).count();
            //if (uCount > 0)
            {
                context.strRemark << uCount << L" 项";
            }
        }

        break;
    };
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context, IMedia& Media)
{
    auto eMediaType = Media.type();
    if (E_MediaType::MT_PlayItem == eMediaType)
    {
        context.setIcon(m_pmPlayItem);

        UINT uSingerID = Media.GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer);
        if (uSingerID > 0)
        {
            cauto strSingerName = Media.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
            if (!strSingerName.empty())
            {
                auto& brSingerHead = genSingerHead(uSingerID, strSingerName);
                context.setSingerIcon(brSingerHead);
            }
        }
    }
    else if (E_MediaType::MT_MediaRes == eMediaType)
    {
        context.setIcon(m_pmSSFile);
    }
    context.strText = m_mapDisplayName[&Media];
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context, XFile& file)
{
    auto& MediaRes = ((CMediaRes&)file);
    if (MediaRes.mediaSet())
    {
        context.setIcon(m_pmSSFile);
        context.strText = m_mapDisplayName[&MediaRes];
    }
    else
    {
        context.setIcon(m_pmFile);
        if (file.rootDir() == &m_OuterDir)
        {
            context.strText = MediaRes.GetName();
        }
        else //MediaRes.GetFileType() == E_MediaFileType::MFT_Null) //xpk
        {
            context.strText = MediaRes.GetTitle();
        }
    }
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context, CPath& dir)
{
    context.uStyle |= E_LVItemStyle::IS_ForwardButton;

    //if (dir.parent()) //(&dir != &__medialib && &dir != &m_OuterDir)
    if (dir.rootDir() != &m_OuterDir)
    {
        auto uCount = dir.count();
        if (uCount > 0)
        {
            context.strRemark << uCount << L" 项";
        }
    }

    auto pSnapshotMediaDir = (CSnapshotMediaDir*)((CMediaDir&)dir).mediaSet();
    if (pSnapshotMediaDir)
    {
        auto catType = pSnapshotMediaDir->catType();
        context.setIcon(_catIcon(catType));

        if (NULL == pSnapshotMediaDir->m_pParent)// dir.parent() == &__medialib)
        {
            //context.fIconMargin *= .9f * m_medialibDlg.rowCount()/this->getRowCount();
            context.nIconSize *= 1.13f;
            context.strText = _catText(catType);
        }
        else
        {
            auto pSinger = pSnapshotMediaDir->singer();
            if (pSinger)
            {
                auto& brSingerHead = genSingerHead(pSinger->m_uID, pSinger->m_strName);
                context.setSingerIcon(brSingerHead);
            }
        }
    }
    else
    {
        //xpk
        auto pRootDir = dir.rootDir();
        if (pRootDir != &__medialib && pRootDir != &m_OuterDir)//pRootDir == &__medialib)
        {
            context.setIcon(m_pmXpk);
            return;
        }

        context.setIcon(m_pmDir);

        auto pParentDir = dir.parent();
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
        else
        {
            if (pParentDir == &m_OuterDir)
            {
                if (dynamic_cast<COuterDir*>(&dir)) //windows驱动器、安卓tf卡
                {
                    context.setIcon(m_pmOuterDir);
                }
            }
        }
#endif
    }
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context)
{
    if (context.pMediaSet)
    {
        _genMLItemContext(context,*context.pMediaSet);
    }
    else if (context.pMedia)
    {
        _genMLItemContext(context, *context.pMedia);
    }
    else if (context.pFile)
    {
        _genMLItemContext(context, *context.pFile);
    }
    else if (context.pDir)
    {
        _genMLItemContext(context, *context.pDir);
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
            context.pDir = &m_OuterDir;
        }
    }

    auto pSnapshotMediaRes = context.snapshotMediaRes();
    if (pSnapshotMediaRes)
    {
        auto eTrackType = pSnapshotMediaRes->trackType();
        if (E_TrackType::TT_Single != eTrackType)
        {
            context.setIcon(E_TrackType::TT_HDWhole == eTrackType ? m_pmHDDisk : m_pmSQDisk);

            cauto cue = pSnapshotMediaRes->cueFile();
            if (cue)
            {
                context.strRemark << cue.m_alTrackInfo.size() << L" 首\n";
            }
            context.strRemark << pSnapshotMediaRes->durationString();
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
             g_app.getSingerImgMgr().downloadSingerHead(lstSingerName);
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
         g_app.getSingerImgMgr().downloadSingerHead(lstSingerName);
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
            szIcon = __size(80);
            szMargin = (cy - szIcon)/2;
        }

        int xIcon = rcRow.right() + __playIconOffset - szIcon;
        int yIcon = rcRow.top()+szMargin;
        QRect rcPlayIcon(xIcon, yIcon, szIcon, szIcon);

        bool bFlash = (int)mlContext->uItem == m_nFlashItem;
        cauto pm = mlContext.pMediaSet
                ?(bFlash?m_pmPlayOpacity:m_pmPlay)
               :(bFlash?m_pmAddPlayOpacity:m_pmAddPlay);
        painter.drawImg(rcPlayIcon, pm);

        rc.setRight(xIcon-__lvRowMargin+__playIconOffset);
    }

    if (!mlContext.strRemark->empty())
    {
        CPainterFontGuard fontGuard(painter, 0.81, TD_FontWeight::ExtraLight);
        UINT uAlpha = CPainter::oppTextAlpha(__RemarkAlpha);
        cauto rcRet = painter.drawTextEx(rc, __rAlign, mlContext.strRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);
        rc.setRight(rcRet.x() - __size(20));
    }

    QString qsMediaQuality;
    if (mlContext.pMedia)
    {
        auto eType = mlContext.pMedia->type();
        if (E_MediaType::MT_AlbumItem == eType)
        {
            rc.setLeft(rc.left() + __size10);
        }

        if (E_MediaType::MT_MediaRes != eType)
        {
            qsMediaQuality = mediaQualityString(mlContext.pMedia->quality());
            rc.setRight(rc.right() - __size(20) - __size10*qsMediaQuality.length());
        }
    }

    //uTextAlpha = 255;
    //uShadowAlpha = __ShadowAlpha;
    do {
        if (mlContext.pMediaSet)
        {
            if (E_MediaSetType::MST_SnapshotMediaDir == mlContext.pMediaSet->m_eType
                && !((CSnapshotMediaDir*)mlContext.pMediaSet)->available)
            {
                uShadowAlpha /= 2;
                uTextAlpha /= 2;
                break;
            }
        }
        else if (mlContext.pDir)
        {
            auto pSnapshotMediaDir = (CSnapshotMediaDir*)((CMediaDir*)mlContext.pDir)->mediaSet();
            if (pSnapshotMediaDir && !pSnapshotMediaDir->available)
            {
                uShadowAlpha /= 2;
                uTextAlpha /= 2;
                break;
            }
        }
        else
        {
            auto pSnapshotMediaRes = mlContext.snapshotMediaRes();
            if (pSnapshotMediaRes && !pSnapshotMediaRes->available)
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
        CPainterFontGuard fontGuard(painter, 0.69, TD_FontWeight::Thin);

        auto rcPos = rcRet;
        rcPos.setLeft(rcPos.right() + __size(20));
        rcPos.setTop(rcPos.top() - __size(9));
        rcPos.setRight(10000);
        painter.drawTextEx(rcPos, Qt::AlignLeft|Qt::AlignTop, qsMediaQuality, 1, uShadowAlpha, uTextAlpha);
    }

    return rcRet;
}

CBrush& CMedialibView::genSingerHead(UINT uSingerID, cwstr strSingerName)
{
    auto& pbrSingerHead = m_mapSingerHead[uSingerID];
    if (pbrSingerHead)
    {
        return *pbrSingerHead;
    }

    auto pHeadImg =  g_app.getSingerImgMgr().getSingerHead(strSingerName);
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

void CMedialibView::_onMediaClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media)
{
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

    tagMLItemContext context(lvItem, media);
    auto pSnapshotMediaRes = context.snapshotMediaRes();

    if (_hittestPlayIcon(context, me.x()))
    {
        if (pSnapshotMediaRes && !pSnapshotMediaRes->available)
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

        if (pSnapshotMediaRes && !pSnapshotMediaRes->available)
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
    if (&dir == &m_OuterDir)
    {
        if (!requestAndroidSDPermission())
        {
            return;
        }
    }
#endif

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

/*CPath* CMedialibView::_onUpward(CPath& currentDir)
{
    ???if (NULL == currentDir.parent())
    {
        if (&currentDir != &__medialib && &currentDir != &m_OuterDir)
        {
            return &__medialib;
        }
    }

    return CMLListView::_onUpward(currentDir);
}*/

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
    m_OuterDir.clear();

    m_mapDisplayName.clear();

    m_lstSingerHead.clear();
    m_mapSingerHead.clear();

    CMLListView::_cleanup();
}

CMediaRes* CMedialibView::showMediaRes(cwstr strPath)
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

    hittestFile(*pMediaRes);

    return pMediaRes;
}

CPath* COuterDir::_newSubDir(const tagFileInfo& fileInfo)
{
    if (fileInfo.strName.front() == L'.')
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
