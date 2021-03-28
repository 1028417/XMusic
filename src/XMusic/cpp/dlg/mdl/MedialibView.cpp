
#include "xmusic.h"

#include "MedialibView.h"

#define __FlashingAlpha 170

#define __RemarkAlpha 200

#define __XSingerName   L" 歌手"
#define __XPlaylistName L" 歌单"
#define __XMusicDirName L"XMusic"
#define __LocalDirName  L" 本机"

#define __catDSD    L"直接比特流数字编码\nDirect Stream Digital"
#define __catHires  L"高解析音频 24~32Bit/96~192KHz\nHigh Resolution Audio"
#define __catMQS    L"录音棚级别无损 24Bit/96KHz\nMastering Quality Sound"
#define __catDTS    L"5.1声道 DTSDigitalSurround"
#define __catDisc   L"DISC整轨"
#define __catCD     L"CD分轨 标准1411Kbps码率"
#define __catSQ24   L"24位无损 24Bit/48KHz"

static map<const IMedia*, wstring> g_mapDisplayName;

inline static void genDisplayTitle(const IMedia* pMedia, const wstring *pstrSingerName)
{
    auto& strTitle = g_mapDisplayName[pMedia];
    if (strTitle.empty())
    {
        strTitle = pMedia->GetTitle();
        CFileTitle::genDisplayTitle(strTitle, pstrSingerName);
    }
}

inline static void genDisplayTitle(const IMedia* pMedia)
{
    auto& strTitle = g_mapDisplayName[pMedia];
    if (strTitle.empty())
    {
        strTitle = pMedia->GetTitle();
        CFileTitle::genDisplayTitle(strTitle);
    }
}

inline static void genDisplayTitle(const CAlbumItem& AlbumItem, cwstr strSingerName)
{
    auto& strTitle = g_mapDisplayName[&AlbumItem];
    if (strTitle.empty())
    {
        strTitle = AlbumItem.GetTitle();
        CFileTitle::genDisplayTitle(strTitle, &strSingerName);
        auto len = strSingerName.size()+3;
        if (strTitle.substr(0, len) == strSingerName + L" - ")
        {
            strTitle.erase(0, len);
        }
    }
}

CMedialibView::CMedialibView(CMedialibDlg& medialibDlg, CMediaDir &OuterDir)
    : CMLListView(&medialibDlg, E_LVScrollBar::LVSB_Left)
    , m_medialibDlg(medialibDlg)
    , m_SingerLib(__app.getSingerMgr())
    , m_PlaylistLib(__app.getPlaylistMgr())
    , m_OuterDir(OuterDir)
    , m_pmHDDisk(__app.m_pmHDDisk)
    , m_pmLLDisk(__app.m_pmLLDisk)
{
}

void CMedialibView::initpm()
{
    (void)m_brNullSingerHead.setTexture(__mdlPng(singerdefault));

    (void)m_pmSSFile.load(__mdlPng(media));

    (void)m_pmSingerGroup.load(__mdlPng(singergroup));
    (void)m_pmAlbum.load(__mdlPng(album));

    (void)m_pmPlaylistSet.load(__mdlPng(playlistset));
    (void)m_pmPlaylist.load(__mdlPng(playlist));
    (void)m_pmPlayItem.load(__mdlPng(playitem));

    (void)m_pmXmusicDir.load(__mdlPng(xmusicdir));
    (void)m_pmSSDir.load(__mdlPng(ssdir));

    (void)m_pmHires.load(__mdlPng(hires));
    (void)m_pmDSD.load(__mdlPng(dsd));
    (void)m_pmMQS.load(__mdlPng(mqs));
    (void)m_pmDTS.load(__mdlPng(dts));
    (void)m_pmDiskDir.load(__mdlPng(diskdir));
    (void)m_pmCD.load(__mdlPng(compactDisc));

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
    g_mapDisplayName.clear();

    m_medialibDlg.updateHead(L"媒体库");
}

void CMedialibView::_onShowMediaSet(CMediaSet& MediaSet)
{
    g_mapDisplayName.clear();

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
        __app.getSingerImgMgr().downloadSingerHead(lstSingerName);
        return;
    }

    if (E_MediaSetType::MST_Album == MediaSet.m_eType)
    {
        cauto strSingerName = ((CAlbum&)MediaSet).GetSinger().m_strName;
        for (cauto AlbumItem : ((CAlbum&)MediaSet).albumItems())
        {
            genDisplayTitle(AlbumItem, strSingerName);
        }
        return;
    }

    if (E_MediaSetType::MST_SnapshotMediaDir == MediaSet.m_eType)
    {
        auto& snapshotMediaDir = (CSnapshotMediaDir&)MediaSet;
        cauto strSingerName = snapshotMediaDir.singerName();
        if (strSingerName.empty())
        {
            for (auto pSubFile : snapshotMediaDir.files())
            {
                genDisplayTitle((CMediaRes*)pSubFile);
            }
        }
        else
        {
            for (auto pSubFile : snapshotMediaDir.files())
            {
                genDisplayTitle((CMediaRes*)pSubFile, &strSingerName);
            }
        }
        return;
    }

    if (E_MediaSetType::MST_Playlist == MediaSet.m_eType)
    {
        list<wstring> *plstSingerName = NULL;
        static map<const void*, std::list<wstring>> mapSingerName;
        cauto itr = mapSingerName.find(&MediaSet);
        if (itr != mapSingerName.end())
        {
            plstSingerName = &itr->second;

            for (auto& PlayItem : ((CPlaylist&)MediaSet).playItems())
            {
                cauto strSingerName = PlayItem.GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
                if (strSingerName.empty())
                {
                    genDisplayTitle(&PlayItem);
                }
                else
                {
                    genDisplayTitle(&PlayItem, &strSingerName);
                }
            }
        }
        else
        {
            plstSingerName = &mapSingerName[&MediaSet];

            cauto singerMgr = __app.getSingerMgr();
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
                    genDisplayTitle(&PlayItem, &pSinger->m_strName);
                }
                else
                {
                    genDisplayTitle(&PlayItem);
                }
            }
        }

        if (!plstSingerName->empty())
        {
            __app.getSingerImgMgr().downloadSingerHead(*plstSingerName);
        }
    }
}

void CMedialibView::_onShowDir(CPath& dir)
{
    g_mapDisplayName.clear();

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
        auto pSnapshotMediaDir = (CSnapshotMediaDir*)((CMediaDir&)dir).mediaSet();
        if (pSnapshotMediaDir)
        {
            if (NULL == pSnapshotMediaDir->m_pParent)// dir.parent() == &__medialib)
            {
                strTitle.erase(0, 3);
            }

            //文件标题
            cauto strSingerName = pSnapshotMediaDir->singerName();
            if (strSingerName.empty())
            {
                for (auto pSubFile : dir.files())
                {
                    genDisplayTitle((CMediaRes*)pSubFile);
                }
            }
            else
            {
                for (auto pSubFile : dir.files())
                {
                    genDisplayTitle((CMediaRes*)pSubFile, &strSingerName);
                }
            }

            //目录图标
            list<wstring> *plstSingerName = NULL;
            static map<const void*, std::list<wstring>> mapSingerName;
            cauto itr = mapSingerName.find(&dir);
            if (itr != mapSingerName.end())
            {
                plstSingerName = &itr->second;
            }
            else
            {
                plstSingerName = &mapSingerName[&dir];

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
                __app.getSingerImgMgr().downloadSingerHead(*plstSingerName);
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
        context.setIcon(m_pmSSDir);
        context.uStyle |= E_LVItemStyle::IS_ForwardButton;

        if (E_MediaSetType::MST_SnapshotMediaDir == MediaSet.m_eType)
        {
            auto catType = ((CSnapshotMediaDir&)MediaSet).catType();
            switch (catType)
            {
            case E_SSCatType::CT_DSD:
                context.setIcon(m_pmDSD);
                break;
            case E_SSCatType::CT_Hires:
                context.setIcon(m_pmHires);
                break;
            case E_SSCatType::CT_MQS:
                context.setIcon(m_pmMQS);
                break;
            case E_SSCatType::CT_DTS:
                context.setIcon(m_pmDTS);
                break;
            case E_SSCatType::CT_Disc:
                context.setIcon(m_pmDiskDir);
                break;
            case E_SSCatType::CT_CD:
                context.setIcon(m_pmCD);
                break;
            default:
                break;
            }

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
    context.strText = g_mapDisplayName[&Media];
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context, XFile& file)
{
    auto& MediaRes = ((CMediaRes&)file);
    if (MediaRes.mediaSet())
    {
        context.setIcon(m_pmSSFile);
        context.strText = g_mapDisplayName[&MediaRes];
    }
    else
    {
        context.setIcon(m_pmFile);
        context.strText = MediaRes.fileName();
    }
}

void CMedialibView::_genMLItemContext(tagMLItemContext& context, CPath& dir)
{
    context.uStyle |= E_LVItemStyle::IS_ForwardButton;

    if (&dir != &__medialib && &dir != &m_OuterDir)
    {
        auto uCount = dir.count();// dir.dirs().size() + dir.files().size();
        if (uCount > 0)
        {
            context.strRemark << uCount << L" 项";
        }
    }

    auto pSnapshotMediaDir = (CSnapshotMediaDir*)((CMediaDir&)dir).mediaSet();
    if (pSnapshotMediaDir)
    {
        context.setIcon(m_pmSSDir);

        if (NULL == pSnapshotMediaDir->m_pParent)// dir.parent() == &__medialib)
        {
            //context.fIconMargin *= .9f * m_medialibDlg.rowCount()/this->getRowCount();
            context.nIconSize *= 1.13f;

            switch (pSnapshotMediaDir->catType())
            {
            case E_SSCatType::CT_DSD:
                context.setIcon(m_pmDSD);
                context.strText = __catDSD;
                break;
            case E_SSCatType::CT_Hires:
                context.setIcon(m_pmHires);
                context.strText = __catHires;
                break;
            case E_SSCatType::CT_MQS:
                context.setIcon(m_pmMQS);
                context.strText = __catMQS;
                break;
            case E_SSCatType::CT_DTS:
                context.setIcon(m_pmDTS);
                context.strText = __catDTS;
                break;
            case E_SSCatType::CT_Disc:
                context.setIcon(m_pmDiskDir);
                context.strText = __catDisc;
                break;
            case E_SSCatType::CT_CD:
                context.setIcon(m_pmCD);
                context.strText = __catCD;
                break;
            case E_SSCatType::CT_SQ24:
                context.strText = __catSQ24;
                break;
            default:
                context.strText = pSnapshotMediaDir->name();// pSnapshotMediaDir->catName();
                break;
            }
        }
        else
        {
            auto uSingerID = pSnapshotMediaDir->GetRelatedMediaSetID(E_RelatedMediaSet::RMS_Singer); //pSnapshotMediaDir->singerID();
            if (uSingerID > 0)
            {
                cauto strSingerName = pSnapshotMediaDir->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer); //pSnapshotMediaDir->singerName();
                auto& brSingerHead = genSingerHead(uSingerID, strSingerName);
                context.setSingerIcon(brSingerHead);
            }
        }
    }
    else
    {
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
            if (pParentDir == &m_OuterDir && dynamic_cast<COuterDir*>(&dir))
            {
                context.setIcon(m_pmOuterDir);
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
        context.nIconSize *= 1.15f;

        cauto uRow = context->uRow;
        if ((bHLayout && 1 == uRow && 0 == context->uCol) || (!bHLayout && 1 == uRow))
        {
            context.setIcon(m_pmSingerGroup);
            context.strText = __XSingerName;
            context.pMediaSet = &m_SingerLib;
        }
        else if ((bHLayout && 1 == uRow && 1 == context->uCol) || (!bHLayout && 3 == uRow))
        {
            context.setIcon(m_pmPlaylistSet);
            context.strText = __XPlaylistName;
            context.pMediaSet = &m_PlaylistLib;
        }
        else if ((bHLayout && 3 == uRow && 0 == context->uCol) || (!bHLayout && 5 == uRow))
        {
            context.setIcon(m_pmXmusicDir);
            context.strText = __XMusicDirName;
            context.pDir = &__medialib;
        }
        else if ((bHLayout && 3 == uRow && 1 == context->uCol) || (!bHLayout && 7 == uRow))
        {
            context.setIcon(m_pmDir);
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
            context.setIcon(E_TrackType::TT_HDWhole == eTrackType ? &m_pmHDDisk : &m_pmLLDisk);

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
            __app.getSingerImgMgr().downloadSingerHead(lstSingerName);
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
        __app.getSingerImgMgr().downloadSingerHead(lstSingerName);
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

    if (mlContext.pMedia)
    {
        if (mlContext.pMedia->type() == E_MediaType::MT_AlbumItem)
        {
            rc.setLeft(rc.left() + __size10);
        }
    }

    if (!mlContext.strRemark->empty())
    {
        CPainterFontGuard fontGuard(painter, 0.81, QFont::Weight::ExtraLight);
        UINT uAlpha = CPainter::oppTextAlpha(__RemarkAlpha);
        cauto rcRet = painter.drawTextEx(rc, __rAlign, mlContext.strRemark, 1, __ShadowAlpha*uAlpha/255, uAlpha);
        rc.setRight(rcRet.x() - __size(20));
    }

    QString qsMediaQuality;
    auto pMedia = mlContext.pMedia;// ? (IMedia*)mlContext.pMedia : (CMediaRes*)mlContext.pFile;
    if (pMedia)
    {
        qsMediaQuality = mediaQualityString(pMedia->quality());
        rc.setRight(rc.right() - __size(20) - __size10*qsMediaQuality.length());
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
        CPainterFontGuard fontGuard(painter, 0.69, QFont::Weight::Thin);

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

    auto pHeadImg = __app.getSingerImgMgr().getSingerHead(strSingerName);
    if (NULL == pHeadImg)
    {
        pbrSingerHead = &m_brNullSingerHead;
    }
    else if (pHeadImg->bExist)
    {
        cauto qsFile = __WS2Q(__app.getSingerImgMgr().file(*pHeadImg));
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

void CMedialibView::_onItemClick(tagLVItem& lvItem, const QMouseEvent& me, CMediaSet& mediaSet)
{
    if (_hittestPlayIcon(tagMLItemContext(lvItem, mediaSet), me.x()))
    {
        _flashItem(lvItem.uItem);

        __app.getCtrl().callPlayCmd(tagAssignMediaSetCmd(mediaSet));

        return;
    }

    CMLListView::_onItemClick(lvItem, me, mediaSet);
}

void CMedialibView::_onItemClick(tagLVItem& lvItem, const QMouseEvent& me, IMedia& media)
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

        if (__app.getPlayMgr().playStatus() != E_PlayStatus::PS_Play)
        {
            __app.getCtrl().callPlayCmd(tagPlayMediaCmd(media));
        }
        else
        {
            __app.getCtrl().callPlayCmd(tagAppendMediaCmd(media));
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

        __app.getCtrl().callPlayCmd(tagPlayMediaCmd(media));
    }
}

CMediaSet* CMedialibView::_onUpward(CMediaSet& currentMediaSet)
{
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
    g_mapDisplayName.clear();

    m_lstSingerHead.clear();
    m_mapSingerHead.clear();

    CMLListView::_cleanup();
}

#if __android
void CMedialibView::showDir(CPath& dir)
{
    if (&dir == &m_OuterDir)
    {
        if (!requestAndroidSDPermission())
        {
            return;
        }
    }

    CMLListView::showDir(dir);
}
#endif
