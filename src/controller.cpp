
#include "controller.h"

#if !__winvc
#include "xmusic.h"
#endif

static const wstring g_strInvalidMediaName = L":\\/|*?\"<>";
static const wstring g_strInvalidMediaSetName = g_strInvalidMediaName;// + __wcDot;

UINT g_uPlaySeq = 0;

void CXController::start()
{
#if __winvc
    static auto& PlayMgr = m_model.getPlayMgr();
    (void)timerutil::setTimer(60000, [&]{
        if (m_OptionMgr.checkAlarm())
		{
            cauto vctAlarmmedia = m_model.getDataMgr().alarmmedias();
            if (!vctAlarmmedia.empty())
            {
                __srand
                auto strAlarmmedia = vctAlarmmedia[__rand(vctAlarmmedia.size() - 1)];
                strAlarmmedia = __medialib.toAbsPath(strAlarmmedia, true);

                if (PlayMgr.play(strAlarmmedia))
                {
                    (void)m_view.msgBox(L"点击确定停止闹铃！");
					PlayMgr.SetPlayStatus(E_PlayStatus::PS_Stop);
                }

                return;
            }
		}

        E_TimerOperate eTimerOperate = m_OptionMgr.checkTimerOperate();
		if (TO_StopPlay == eTimerOperate)
		{
			PlayMgr.SetPlayStatus(E_PlayStatus::PS_Stop);
		}
		else if (TO_Shutdown == eTimerOperate)
		{
			(void)::WinExec("shutdown -s -f -t 0", 0);
		}
		else if (TO_Hibernal == eTimerOperate)
		{
			(void)::WinExec("shutdown -h", 0);
		}
	});

	/*cauto strRootDir = m_OptionMgr.getOption().strRootDir;
	if (strRootDir.empty() || !fsutil::existDir(strRootDir))
	{
		//strRootDir.clear();
		if (!setupMediaLib())
		{
			__mainApp->Quit();
			return;
		}
	}
    else
    {
		if (!m_model.initMediaLib())
		{
			__mainApp->Quit();
			return;
		}

        mtutil::thread([&]{
            CFolderDlg::preInit();
        });
    }

	__async([&]{
        int nRet = CPlayer::InitSDK();
        if (nRet != 0)
        {
            //g_logger << "initPlaySDK fail: " >> nRet;
            return;
        }

		PlayMgr.tryPlay();
	});*/

#else
    //m_threadPlayCtrl.start(
    mtutil::thread([&]() {
        int nRet = CPlayer::InitSDK();
        if (nRet != 0)
        {
            g_logger << "initPlaySDK fail: " >> nRet;
            return;
        }

        auto& PlayMgr = m_model.getPlayMgr();
        PlayMgr.tryPlay();

        while (usleepex(100)) //m_threadPlayCtrl.usleep(100))
        {
            tagPlayCmd PlayCmd;
            m_mutex.lock();
            m_PlayCmd.get(PlayCmd);
            m_mutex.unlock();

            if (PlayCmd.ePlayCmd != E_PlayCmd::PC_Null && PlayCmd.ePlayCmd != E_PlayCmd::PC_Append)
            {
                g_uPlaySeq++;
            }

            switch (PlayCmd.ePlayCmd)
            {
            case E_PlayCmd::PC_PlayIndex:
                (void)PlayMgr.play(PlayCmd.uPlayIdx);

                break;
            case E_PlayCmd::PC_PlayPrev:
                PlayMgr.playPrev();
                break;

            case E_PlayCmd::PC_PlayNext:
                (void)PlayMgr.playNext();

                break;
            case E_PlayCmd::PC_AutoPlayNext:
                (void)PlayMgr.playNext(false);

                break;
            case E_PlayCmd::PC_Demand:
                (void)PlayMgr.demand(PlayCmd.eDemandMode, PlayCmd.eDemandLanguage);

                break;
            case E_PlayCmd::PC_Assign:
                (void)PlayMgr.assign(PlayCmd.paMedias);

                break;
            case E_PlayCmd::PC_Append:
            case E_PlayCmd::PC_AppendPlay:
                if (PlayCmd.paMedias.size() == 1)
                {
                    bool bFlag = false;
                    PlayCmd.paMedias.get(0, [&](const IMedia& media){
                        PlayMgr.playingItems().back([&](const CPlayItem& PlayItem){
                            if (media.GetPath() == PlayItem.GetPath())
                            {
                                bFlag = true;
                            }
                        });
                    });
                    if (bFlag)
                    {
                        if (E_PlayCmd::PC_AppendPlay==PlayCmd.ePlayCmd)
                        {
                            (void)PlayMgr.play(PlayMgr.playingItems().size()-1);
                        }
                        break;
                    }
                }

                (void)PlayMgr.insert(PlayCmd.paMedias, E_PlayCmd::PC_AppendPlay==PlayCmd.ePlayCmd);

                break;
            default:
                break;
            }
        }
    });
#endif
}

void CXController::stop()
{
#if !__winvc
    g_logger >> "stop controller";
    //m_threadPlayCtrl.cancel();
#endif

    m_model.close();

    m_OptionMgr.saveOption();

    CPlayer::QuitSDK();
}

#if __winvc
bool CXController::setupMediaLib()
{
    static CFolderDlgEx FolderDlg;
    cauto strRootDir = FolderDlg.Show(L"设定根目录", L"请选择媒体库根目录");
    if (strRootDir.empty())
    {
        return false;
    }

    if (strutil::matchIgnoreCase(strRootDir, m_OptionMgr.getOption().strRootDir))
    {
        return false;
    }

    if (!m_model.setupMediaLib(strRootDir))
    {
        return false;
    }

    return true;
}

E_RenameRetCode CXController::renameMediaSet(CMediaSet& MediaSet, cwstr strNewName)
{
	if (strNewName.find_first_of(g_strInvalidMediaSetName) != __wnpos)
	{
		return E_RenameRetCode::RRC_InvalidName;
	}

	TD_MediaSetList arrMediaSets;
	MediaSet.m_pParent->GetAllMediaSets(MediaSet.m_eType, arrMediaSets);

	if (arrMediaSets.any([&](CMediaSet& SubSet) {
		if (&SubSet == &MediaSet)
		{
			return false;
		}
		
		return strutil::matchIgnoreCase(SubSet.m_strName, strNewName);
	}))
	{
		return E_RenameRetCode::RRC_NameExists;
	}

	__EnsureReturn(m_model.getDataMgr().updateMediaSetName(MediaSet, strNewName), E_RenameRetCode::RRC_Failure);
	
	tagMediaSetChanged MediaSetChanged(MediaSet.m_eType, MediaSet.m_uID, E_MediaSetChanged::MSC_Rename);
	MediaSetChanged.strNewName = strNewName;
	m_view.updateMediaRelated(MediaSetChanged);
	
	return E_RenameRetCode::RRC_Success;
}

bool CXController::removeMediaSet(CMediaSet& MediaSet)
{
	tagMediaSetChanged MediaSetChanged(MediaSet.m_eType, MediaSet.m_uID, E_MediaSetChanged::MSC_Remove);

	UINT uID = MediaSet.m_uID;
	switch (MediaSet.m_eType)
	{
	case E_MediaSetType::MST_Playlist:
		__EnsureReturn(m_model.getPlaylistMgr().RemovePlaylist(uID), false);

		break;
	case E_MediaSetType::MST_SingerGroup:
		__EnsureReturn(m_model.getSingerMgr().RemoveGroup(uID), false);

		return true;

		break;
	case E_MediaSetType::MST_Singer:
		__EnsureReturn(m_model.getSingerMgr().RemoveSinger(uID), false);

		break;
	case E_MediaSetType::MST_Album:
		__EnsureReturn(m_model.getSingerMgr().RemoveAlbum(uID), false);

		break;
	default:
		return false;
	}

	m_view.updateMediaRelated(MediaSetChanged);

	return true;
}

CMediaDir* CXController::attachDir(cwstr strDir)
{
	if (strDir.size() <= 3)
	{
		m_view.msgBox(L"不支持挂载驱动器，请选择具体目录");
		return NULL;
	}

    if (!__medialib.checkIndependentDir(strDir, false))
	{
		m_view.msgBox(L"请选择与根目录不相关的目录");
		return NULL;
	}

    bool bExist = m_OptionMgr.getOption().plAttachDir.anyFirst([&](cwstr strAttachDir) {
        return strutil::matchIgnoreCase(strAttachDir, strDir) || fsutil::CheckSubPath(strAttachDir, strDir);
    });
	if (bExist)
	{
		m_view.msgBox(L"所选目录已被挂载，请选择其它目录");
		return NULL;
	}

	return m_model.attachDir(strDir);
}

bool CXController::renameMedia(const IMedia& media, cwstr strNewName)
{
	if (strNewName.find_first_of(g_strInvalidMediaName) != __wnpos)
	{
		m_view.msgBox(L"名称含特殊字符！");
		return false;
	}

    bool bDir = (media.type() == E_MediaType::MT_MediaRes && ((CMediaRes&)media).isDir());

	wstring strExtName;
	if (!bDir)
	{
		strExtName = __wcDot + media.GetExtName();
	}

    cauto strOldOppPath = media.GetPath();
	cauto strOldAbsPath = media.GetAbsPath();
	cauto strNewAbsPath = fsutil::GetParentDir(strOldAbsPath) + __wcPathSeparator + strNewName + strExtName;
    if (fsutil::existPath(strOldAbsPath, bDir))
	{
		bool bRet = false;
        m_model.getPlayMgr().renameFile(bDir, strOldAbsPath, strNewAbsPath, [&]{
			bool bRet = false;
			if (fsutil::MatchPath(strOldAbsPath, strNewAbsPath))
			{
				bRet = fsutil::moveFile(strOldAbsPath, strNewAbsPath + L".temp")
					& fsutil::moveFile(strNewAbsPath + L".temp", strNewAbsPath);
			}
			else
			{
				bRet = fsutil::moveFile(strOldAbsPath, strNewAbsPath);
			}
			if (!bRet)
			{
                m_view.msgBox(L"重命名失败: \n\n\t" + strOldAbsPath);
				return false;
			}

			bRet = true;

			return true;
		});

		if (!bRet)
		{
			return false;
		}
	}
	else
	{
		if (!m_view.msgBox(L"文件不存在，是否继续重命名？", true))
		{
			return false;
		}
	}

	CMediaRes *pMediaRes = __medialib.subPath(strOldOppPath, bDir);
	if (pMediaRes)
	{
        pMediaRes->setName(strNewName + strExtName);
	}

	cauto strNewOppPath = fsutil::GetParentDir(strOldOppPath) + __wcPathSeparator + strNewName + strExtName;
	return m_model.renameMedia(strOldOppPath, strNewOppPath, bDir);
}

void CXController::moveMediaFile(const TD_IMediaList& lstMedias, cwstr strOppDir)
{
	cauto t_lstMedias = lstMedias.filter([&](IMedia& media) {
		return !strutil::matchIgnoreCase(fsutil::GetParentDir(media.GetPath()), strOppDir);
	});
	__Ensure(t_lstMedias);
	__Ensure(m_view.msgBox(L"确认移动选中的文件?", true));

	_moveMediaFile(t_lstMedias, strOppDir);
}

void CXController::_moveMediaFile(const TD_IMediaList& lstMedias, cwstr strOppDir)
{
    cauto strDstAbsDir = __medialib.toAbsPath(strOppDir, true);

	SMap<wstring, wstring> mapMovedFiles;

	lstMedias([&](IMedia& media){
        cauto strSrcPath = media.GetPath();
		if (mapMovedFiles.includes(strSrcPath))
		{
			return;
		}

        cauto strDstPath = strOppDir + __wcPathSeparator + media.GetName();
		cauto strDstAbsPath = strDstAbsDir + __wcPathSeparator + media.GetName();

        cauto strSrcAbsPath = media.GetAbsPath();
        if (!fsutil::existFile(strSrcAbsPath))
		{
			mapMovedFiles.insert(strSrcPath, strDstPath);
			return;
		}
		
        if (fsutil::existFile(strDstAbsPath))
		{
            if (!m_view.msgBox(L"确认替换文件: \n\n\t" + strDstAbsPath, true))
			{
				return;
			}
		}
		
		m_model.getPlayMgr().moveFile(media.GetAbsPath(), strDstAbsPath, [&]{
			if (!fsutil::moveFile(strSrcAbsPath, strDstAbsPath))
			{
                m_view.msgBox(L"移动文件失败: \n\n\t" + strDstAbsPath);
				return false;
			}

			mapMovedFiles.insert(strSrcPath, strDstPath);
			
			return true;
		});
	});

	__Ensure(!mapMovedFiles.empty());

	(void)m_model.moveFiles(strOppDir, mapMovedFiles);

	m_model.refreshMediaLib();
}

bool CXController::removeMediaRes(const TD_MediaResList& lstMediaRes)
{
    std::set<wstring> setFiles;
	lstMediaRes([&](CMediaRes& MediaRes) {
		setFiles.insert(strutil::lowerCase_r(MediaRes.GetPath()));
	});
	__EnsureReturn(m_model.getPlayMgr().remove(setFiles), false);

	lstMediaRes([&](CMediaRes& MediaRes) {
		if (fsutil::removeFile(MediaRes.GetAbsPath()))
		{
            MediaRes.remove();
		}
		else
		{
            m_view.msgBox(L"删除文件失败: \n\n\t" + MediaRes.GetAbsPath());
		}
	});

	__EnsureReturn(m_model.removeFiles(setFiles), false);

	return true;
}

int CXController::AddPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist)
{
	wstring strOppPath;
	SArray<wstring> arrOppPaths;
	for (cauto strFile : lstFiles)
	{
        strOppPath = __medialib.toOppPath(strFile);
		if (strOppPath.empty())
		{
            m_view.msgBox((L"添加曲目失败，请选择以下目录中的文件: \n\n\t"
                + __medialib.path()).c_str());
			return 0;
		}

		arrOppPaths.add(strOppPath);
	}

	__EnsureReturn(m_model.getPlaylistMgr().AddPlayItems(arrOppPaths, Playlist, 0), -1);

	return arrOppPaths.size();
}

int CXController::AddAlbumItems(const list<wstring>& lstAbsPath, CAlbum& album, int nPos)
{
	cauto strSingerDir = album.GetSinger().dir();

	SArray<wstring> lstOppPaths;
	for (cauto strFile : lstAbsPath)
	{
        wstring strOppPath = __medialib.toOppPath(strFile);
		strOppPath = fsutil::GetOppPath(strSingerDir, strOppPath);
		if (strOppPath.empty())
		{
            m_view.msgBox((L"添加专辑曲目失败，请选择以下目录中的文件: \n\n\t" + strSingerDir).c_str());
			return 0;
		}

		lstOppPaths.add(strOppPath);
	}

	__EnsureReturn(m_model.getSingerMgr().AddAlbumItems(lstOppPaths, album, nPos), -1);

	return lstOppPaths.size();
}

int CXController::AddAlbumItems(const TD_IMediaList& paMedias, CAlbum& album, int nPos)
{
	cauto strSingerDir = album.GetSinger().dir();
	
	TD_IMediaList paMoveMedias;

	SArray <wstring> lstOppPaths;
	paMedias([&](IMedia& media) {
		auto strOppPath = fsutil::GetOppPath(strSingerDir, media.GetPath());
		if (strOppPath.empty())
		{
			if (!m_view.msgBox(L"确认移动以下文件到歌手目录？\n\n  " + media.GetAbsPath(), true))
			{
				return;
			}

			paMoveMedias.add(media);
			strOppPath = L'\\' + media.GetName();
		}

		lstOppPaths.add(strOppPath);
	});
	
	if (paMoveMedias)
	{
		_moveMediaFile(paMoveMedias, strSingerDir);
	}

	__EnsureReturn(m_model.getSingerMgr().AddAlbumItems(lstOppPaths, album, nPos), -1);

	return lstOppPaths.size();
}

bool CXController::autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias, const CB_AutoMatchProgress& cbProgress
	, const CB_AutoMatchConfirm& cbConfirm, map<CMedia*, wstring>& mapUpdatedMedias)
{	
	map<wstring, bool> mapMatchRecoed;

	CAutoMatch AutoMatch((CModel&)m_model, cbProgress, [&](CMatchMediaInfo& MatchMediaInfo, CMediaResInfo& MediaResInfo) {
		auto& bMatchRecord = mapMatchRecoed[MatchMediaInfo->m_strPath];
		if (bMatchRecord)
		{
			return E_MatchResult::MR_No;
		}
		
		auto eRet = cbConfirm(MatchMediaInfo, MediaResInfo);
		__EnsureReturn(E_MatchResult::MR_Yes == eRet, eRet);
		
		bMatchRecord = true;

		MatchMediaInfo.medias()([&](CMedia& media){
			mapUpdatedMedias[&media] = MediaResInfo->m_strPath;
		});

		return E_MatchResult::MR_Yes;
	});
	AutoMatch.autoMatchMedia(SrcPath, lstMedias);

	if (!mapUpdatedMedias.empty())
	{
		__EnsureReturn(m_model.updateMediaPath(mapUpdatedMedias), false);
	}

	return true;
}
#endif
