
#ifdef _MSC_VER
#include "stdafx.h"
#endif

#include "controller.h"

static const wstring g_strInvalidMediaName = L":\\/|*?\"<>";
static const wstring g_strInvalidMediaSetName = g_strInvalidMediaName + __wcDot;

#ifndef _MSC_VER
thread g_threadPlayCtrl;
TSignal<tagPlayCtrl> m_sigPlayCtrl(tagPlayCtrl(E_PlayCtrl::PC_Null));
#endif

bool CController::start()
{
#ifdef _MSC_VER
	(void)wintimer::setTimer(60000, [&]() {
		wstring strAlarmmedia = m_model.getOptionMgr().checkAlarm();
		if (!strAlarmmedia.empty())
		{
			strAlarmmedia = m_model.getRootMediaRes().toAbsPath(strAlarmmedia);

			if (m_model.getPlayMgr().playAlarm(strAlarmmedia))
			{
				(void)m_view.showMsgBox(L"点击确定停止闹铃！");
				m_model.getPlayMgr().SetPlayStatus(E_PlayStatus::PS_Stop);
			}
		}

		E_TimerOperate eTimerOperate = m_model.getOptionMgr().checkTimerOperate();
		if (TO_StopPlay == eTimerOperate)
		{
			m_model.getPlayMgr().SetPlayStatus(E_PlayStatus::PS_Stop);
		}
		else if (TO_Shutdown == eTimerOperate)
		{
			(void)::WinExec("shutdown -s -f -t 0", 0);
		}
		else if (TO_Hibernal == eTimerOperate)
		{
			(void)::WinExec("shutdown -h", 0);
		}

		return true;
	});
#endif
	
    auto fnTryPlay = [&]() {
		if (m_model.dbstatus() && m_model.getRootMediaRes().GetSubPath())
		{
			if (m_model.getPlayMgr().getPlayingItems())
			{
				m_model.getPlayMgr().SetPlayStatus(E_PlayStatus::PS_Play);
			}
			else
			{
				(void)m_model.getPlayMgr().demand(E_DemandMode::DM_DemandAlbum);
			}
		}
    };

#ifdef _MSC_VER
	CMainApp::async(fnTryPlay, 300);

#else
    g_threadPlayCtrl = thread([&, fnTryPlay]() {
        auto& PlayMgr = m_model.getPlayMgr();

        fnTryPlay();

        while (true)
        {
            tagPlayCtrl PlayCtrl(E_PlayCtrl::PC_Null);
            if (m_sigPlayCtrl.wait_for(2000, PlayCtrl))
            {
                switch (PlayCtrl.ePlayCtrl)
                {
                case E_PlayCtrl::PC_Null:
                    return;

                case E_PlayCtrl::PC_Pause:
                    PlayMgr.SetPlayStatus(E_PlayStatus::PS_Pause);
                    break;

                case E_PlayCtrl::PC_Play:
                    PlayMgr.SetPlayStatus(E_PlayStatus::PS_Play);
                    break;

                case E_PlayCtrl::PC_PlayPrev:
                    PlayMgr.playPrev();
                    break;

                case E_PlayCtrl::PC_PlayNext:
					(void)PlayMgr.playNext();
                    break;
                case E_PlayCtrl::PC_AutoPlayNext:
                    (void)PlayMgr.playNext(false);
                    break;

                case E_PlayCtrl::PC_Demand:
                    (void)PlayMgr.demand(PlayCtrl.eDemandMode, PlayCtrl.eDemandLanguage);
                    break;

                case E_PlayCtrl::PC_PlayMedias:
                    (void)PlayMgr.assign(PlayCtrl.arrPlayMedias);
                    break;
                }

                continue;
            }

            if (m_model.dbstatus() && m_model.getRootMediaRes().GetSubPath())
            {
                if (PlayMgr.getPlayer().GetDuration() == 0)
                {
                    (void)PlayMgr.playNext(false);
                }
            }
        }
    });
#endif

    return true;
}

void CController::stop()
{
#ifndef _MSC_VER
    if (g_threadPlayCtrl.joinable())
    {
        m_sigPlayCtrl.set(tagPlayCtrl(E_PlayCtrl::PC_Null));
        g_threadPlayCtrl.join();
    }
#endif
}

void CController::callPlayCtrl(const tagPlayCtrl& PlayCtrl)
{
#ifndef _MSC_VER
    m_sigPlayCtrl.set(PlayCtrl);
#endif
}

E_RenameRetCode CController::renameMediaSet(CMediaSet& MediaSet, const wstring& strNewName)
{
	if (wstring::npos != strNewName.find_first_of(g_strInvalidMediaSetName))
	{
		return E_RenameRetCode::RRC_InvalidName;
	}

	TD_MediaSetList arrMediaSets;
	MediaSet.m_pParent->GetAllMediaSets(MediaSet.m_eType, arrMediaSets);

	if (!arrMediaSets.every([&](CMediaSet& SubSet) {
		if (&SubSet != &MediaSet)
		{
			if (wsutil::matchIgnoreCase(SubSet.m_strName, strNewName))
			{
				return false;
			}
		}
		return true;
	}))
	{
		return E_RenameRetCode::RRC_NameExists;
	}

	__EnsureReturn(m_model.getDataMgr().updateMediaSetName(MediaSet, strNewName), E_RenameRetCode::RRC_Failure);
	
	tagMediaSetChanged MediaSetChanged(MediaSet.m_eType, E_MediaSetChanged::MSC_Rename, MediaSet.m_uID);
	MediaSetChanged.strNewName = strNewName;
	m_view.updateMediaRelated(MediaSetChanged);
	
	return E_RenameRetCode::RRC_Success;
}

bool CController::removeMediaSet(CMediaSet& MediaSet)
{
	tagMediaSetChanged MediaSetChanged(MediaSet.m_eType, E_MediaSetChanged::MSC_Remove, MediaSet.m_uID);

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

CMediaRes* CController::attachDir(const wstring& strDir)
{
	if (strDir.size() <= 3)
	{
		m_view.showMsgBox(L"不支持挂载驱动器，请选择具体目录");
		return NULL;
	}

	if (!m_model.getRootMediaRes().checkIndependentDir(strDir, false))
	{
		m_view.showMsgBox(L"请选择与根目录不相关的目录");
		return NULL;
	}

	auto& vecAttachDir = m_model.getOptionMgr().getOption().vecAttachDir;
	bool bExist = false;
	for (auto& strAttachDir : vecAttachDir)
	{
		if (wsutil::matchIgnoreCase(strAttachDir, strDir) || fsutil::CheckSubPath(strAttachDir, strDir))
		{
			bExist = true;
			break;
		}
	}
	if (bExist)
	{
		m_view.showMsgBox(L"所选目录已被挂载，请选择其它目录");
		return NULL;
	}

	m_model.attachDir(strDir);

	return (CMediaRes*)m_model.getRootMediaRes().FindSubPath(
        __wcBackSlant + fsutil::GetFileName(strDir), true);
}

bool CController::renameMedia(IMedia& media, const wstring& strNewName)
{
	if (wstring::npos != strNewName.find_first_of(g_strInvalidMediaName))
	{
		m_view.showMsgBox(L"名称含特殊字符！");
		return false;
	}

	wstring strOldOppPath = media.GetPath();

	wstring strExtName = media.GetFileTypeString();
	if (!strExtName.empty())
	{
		strExtName = L"." + strExtName;
	}

    wstring strNewOppPath = fsutil::GetParentDir(strOldOppPath) + __wcBackSlant + strNewName + strExtName;
	wstring strNewAbsPath = m_model.getRootMediaRes().toAbsPath(strNewOppPath);

	wstring strOldAbsPath = media.GetAbsPath();
	bool bDir = media.IsDir();
    if (fsutil::existPath(strOldAbsPath, bDir))
	{
		bool bRet = false;
        m_model.getPlayMgr().renameFile(bDir, strOldAbsPath, strNewAbsPath, [&]() {
			if (!fsutil::moveFile(strOldAbsPath, strNewAbsPath))
			{
                m_view.showMsgBox(L"重命名失败：\n\n\t" + media.GetAbsPath());
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
		if (!m_view.showMsgBox(L"文件不存在，是否继续重命名？", true))
		{
			return false;
		}
	}

	CMediaRes *pMediaRes = media.GetMediaRes();
	if (NULL != pMediaRes)
	{
		pMediaRes->SetName(strNewName + strExtName);
	}
	
	return m_model.renameMedia(strOldOppPath, strNewOppPath, bDir);
}

void CController::moveMediaFile(const TD_IMediaList& lstMedias, const wstring& strDir)
{
	TD_IMediaList lstSrcMedias;
	lstMedias([&](IMedia& Media){
		if (!wsutil::matchIgnoreCase(fsutil::GetParentDir(Media.GetPath()), strDir))
		{
			lstSrcMedias.add(Media);
		}
	});
	__Ensure(lstSrcMedias);
	__Ensure(m_view.showMsgBox(L"确认移动选中的文件?", true));

	SMap<wstring, wstring> mapMovedFiles;

	lstSrcMedias([&](IMedia& Media){
        cauto& strSrcPath = Media.GetPath();
		if (mapMovedFiles.includes(strSrcPath))
		{
			return;
		}

        cauto& strDstPath = strDir + __wcBackSlant + Media.GetName();

        cauto& strSrcAbsPath = Media.GetAbsPath();
        if (!fsutil::existFile(strSrcAbsPath))
		{
			mapMovedFiles.insert(strSrcPath, strDstPath);
			return;
		}

        cauto& strDstAbsPath = m_model.getRootMediaRes().toAbsPath(strDstPath);

        if (fsutil::existFile(strDstAbsPath))
		{
            if (!m_view.showMsgBox(L"确认替换文件：\n\n\t" + strDstAbsPath, true))
			{
				return;
			}
		}
		
		m_model.getPlayMgr().moveFile(Media.GetAbsPath(), strDstAbsPath, [&]() {
			(void)fsutil::removeFile(strDstAbsPath);
			if (!fsutil::moveFile(strSrcAbsPath, strDstAbsPath))
			{
                m_view.showMsgBox(L"移动文件失败：\n\n\t" + strDstAbsPath);
				return false;
			}

			mapMovedFiles.insert(strSrcPath, strDstPath);
			
			return true;
		});
	});

	__Ensure(!mapMovedFiles.empty());

	(void)m_model.moveFiles(strDir, mapMovedFiles);

	m_model.refreshRootMediaRes();
}

bool CController::removeMediaRes(const TD_MediaResList& lstMediaRes)
{
    std::set<wstring> setFiles;
	lstMediaRes([&](CMediaRes& MediaRes) {
		setFiles.insert(wsutil::lowerCase_r(MediaRes.GetPath()));
	});
	__EnsureReturn(m_model.getPlayMgr().remove(setFiles), false);

	lstMediaRes([&](CMediaRes& MediaRes) {
		if (fsutil::removeFile(MediaRes.GetAbsPath()))
		{
			MediaRes.RemoveSelf();
		}
		else
		{
			m_view.showMsgBox(L"删除文件失败：\n\n\t" + MediaRes.GetAbsPath());
		}
	});

	__EnsureReturn(m_model.removeFiles(setFiles), false);

	return true;
}

int CController::addPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist)
{
	wstring strOppPath;
	SArray<wstring> lstOppPaths;
	for (cauto& strFile : lstFiles)
	{
		strOppPath = m_model.getRootMediaRes().toOppPath(strFile);
		if (strOppPath.empty())
		{
            m_view.showMsgBox((L"添加曲目失败，请选择以下目录中的文件：\n\n\t"
				+ m_model.getRootMediaRes().GetAbsPath()).c_str());
			return 0;
		}

		lstOppPaths.add(strOppPath);
	}

	__EnsureReturn(m_model.getPlaylistMgr().AddPlayItems(lstOppPaths, Playlist, 0), -1);

	return lstOppPaths.size();
}

int CController::addAlbumItems(const list<wstring>& lstFiles, CAlbum& Album)
{
	wstring strSingerOppPath = Album.GetBaseDir();

	list<wstring> lstOppPaths;
	for (cauto& strFile : lstFiles)
	{
		wstring strOppPath = m_model.getRootMediaRes().toOppPath(strFile);
		if (!fsutil::CheckSubPath(strSingerOppPath, strOppPath))
		{
            m_view.showMsgBox((L"添加专辑曲目失败，请选择以下目录中的文件：\n\n\t" + strSingerOppPath).c_str());
			return 0;
		}

		lstOppPaths.push_back(strOppPath);
	}

	__EnsureReturn(m_model.getSingerMgr().AddAlbumItems(lstOppPaths, Album), -1);

	return lstOppPaths.size();
}

bool CController::autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias, const CB_AutoMatchProgress& cbProgress
	, const CB_AutoMatchConfirm& cbConfirm, map<CMedia*, wstring>& mapUpdatedMedias)
{
	list<pair<CMedia*, wstring>> lstResult;
	
	map<wstring, tagMediaResInfo*> mapMatchInfo;

	CAutoMatch AutoMatch((CModel&)m_model, cbProgress, [&](CSearchMediaInfo& SearchMediaInfo, tagMediaResInfo& MediaResInfo) {
		auto itr = mapMatchInfo.find(SearchMediaInfo.m_strAbsPath);
		if (itr != mapMatchInfo.end())
		{
			if (itr->second != &MediaResInfo)
			{
				return E_MatchResult::MR_No;
			}
		}
		else
		{
			auto eRet = cbConfirm(SearchMediaInfo, MediaResInfo);
			__EnsureReturn(E_MatchResult::MR_Yes == eRet, eRet);

			mapMatchInfo[SearchMediaInfo.m_strAbsPath] = &MediaResInfo;
		}

		SearchMediaInfo.m_lstMedias([&](CMedia& Media){
			mapUpdatedMedias[&Media] = MediaResInfo.strPath;
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

UINT CController::addInMedia(const list<wstring>& lstFiles, const CB_AddInMediaProgress& cbProgress, const CB_AddInConfirm& cbAddInConfirm)
{
	TD_MediaList lstMedias;
	m_model.getRootMediaSet().GetAllMedias(lstMedias);
	__EnsureReturn(lstMedias, 0);

	CSearchMediaInfoGuard SearchMediaInfoGuard(m_model.getSingerMgr());
	TD_SearchMediaInfoMap mapSearchMedias;
	lstMedias([&](CMedia& Media) {
		SearchMediaInfoGuard.genSearchMediaInfo(Media, mapSearchMedias);
	});

	list<pair<wstring, CSearchMediaInfo>> lstMatchResult;
	for (auto& strFile : lstFiles)
	{
		__EnsureReturn(cbProgress(strFile), 0);
		
		tagMediaResInfo MediaResInfo(strFile);		
		for (auto itr = mapSearchMedias.begin(); itr != mapSearchMedias.end(); )
		{
			CSearchMediaInfo& SearchMediaInfo = itr->second;

			if (SearchMediaInfo.matchMediaRes(MediaResInfo))
			{
				E_MatchResult eRet = cbAddInConfirm(SearchMediaInfo, MediaResInfo);
				if (E_MatchResult::MR_Ignore == eRet)
				{
					itr = mapSearchMedias.erase(itr);
					continue;
				}

				if (E_MatchResult::MR_Yes == eRet)
				{
					lstMatchResult.push_back({ MediaResInfo.strPath, SearchMediaInfo });
				
					(void)mapSearchMedias.erase(itr);

					break;
				}
			}

			++itr;
		}
	}

	SMap<wstring, wstring> mapUpdateFiles;
	map<CMedia*, wstring> mapUpdatedMedias;
	for (auto& pr : lstMatchResult)
	{
		wstring& strSrcAbsPath = pr.first;
		CSearchMediaInfo& SearchMediaInfo = pr.second;

		wstring strDstAbsPath = fsutil::GetParentDir(SearchMediaInfo.m_strAbsPath)
            + __wcBackSlant + fsutil::GetFileName(strSrcAbsPath);

		m_model.getPlayMgr().moveFile(SearchMediaInfo.m_strAbsPath, strDstAbsPath, [&]() {
            (void)fsutil::removeFile(SearchMediaInfo.m_strAbsPath);
			
			if (!fsutil::moveFile(strSrcAbsPath, strDstAbsPath))
			{
                m_view.showMsgBox(L"复制文件失败：\n\n\t" + strDstAbsPath);
				return false;
			}

			wstring strDstOppPath = m_model.getRootMediaRes().toOppPath(strDstAbsPath);
			mapUpdateFiles.set(m_model.getRootMediaRes().toOppPath(SearchMediaInfo.m_strAbsPath), strDstOppPath);

			SearchMediaInfo.m_lstMedias([&](CMedia& Media) {
				mapUpdatedMedias[&Media] = strDstOppPath;
			});

			return true;
		});
	}

	__EnsureReturn(!mapUpdatedMedias.empty(), 0);
	
	__EnsureReturn(m_model.updateMediaPath(mapUpdatedMedias), -1);
	
	__EnsureReturn(m_model.updateFile(mapUpdateFiles), -1);

	m_model.refreshRootMediaRes();

	return mapUpdatedMedias.size();
}
