
#if __winvc
#include "stdafx.h"
#endif

#include "controller.h"

static const wstring g_strInvalidMediaName = L":\\/|*?\"<>";
static const wstring g_strInvalidMediaSetName = g_strInvalidMediaName + __wcDot;

void CXController::start()
{
    static auto& PlayMgr = m_model.getPlayMgr();
    static cauto fnTryPlay = [&](){
#if !__OnlineMediaLib
        if (m_model.getMediaLib().empty())
        {
            return;
        }
#endif

        if (PlayMgr.getPlayingItems())
        {
            PlayMgr.SetPlayStatus(E_PlayStatus::PS_Play);
        }
        else
        {
            (void)PlayMgr.demand(E_DemandMode::DM_DemandAlbum);
        }
    };

#if __winvc
    (void)timerutil::setTimer(60000, [&]() {
        if (m_OptionMgr.checkAlarm())
        {
            cauto vctAlarmmedia = m_model.getDataMgr().alarmmedias();
            if (!vctAlarmmedia.empty())
            {
                __srand
                auto strAlarmmedia = vctAlarmmedia[__rand(vctAlarmmedia.size() - 1)];
                strAlarmmedia = m_model.getMediaLib().toAbsPath(strAlarmmedia);

                if (m_model.getPlayMgr().play(strAlarmmedia))
                {
                    (void)m_view.msgBox(L"点击确定停止闹铃！");
                    m_model.getPlayMgr().SetPlayStatus(E_PlayStatus::PS_Stop);
                }

                return;
            }
		}

        E_TimerOperate eTimerOperate = m_OptionMgr.checkTimerOperate();
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
	});

    __async(100, [&]() {
        if (m_model.getMediaLib().empty())
		{
			m_OptionMgr.getOption().strRootDir.clear();
			if (!setupMediaLib())
			{
				CMainApp::GetMainApp()->Quit();
				return;
			}
		}
		
        __async(100, [&]() {
            fnTryPlay();
        });
    });

#else
    fnTryPlay();

    m_threadPlayCtrl.start([&](const bool& bRunSignal)mutable {
        while (bRunSignal)
        {
            mtutil::usleep(100);

            tagPlayCtrl PlayCtrl;
            m_mtxPlayCtrl.lock([&](tagPlayCtrl& t_PlayCtrl){
                t_PlayCtrl.get(PlayCtrl);
            });

            switch (PlayCtrl.ePlayCtrl)
            {
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
                mtutil::usleep(1000);
                (void)PlayMgr.playNext(false);
                break;

            case E_PlayCtrl::PC_PlayIndex:
                (void)PlayMgr.play(PlayCtrl.uPlayIdx);

                break;
            case E_PlayCtrl::PC_Demand:
                (void)PlayMgr.demand(PlayCtrl.eDemandMode, PlayCtrl.eDemandLanguage);

                break;
            case E_PlayCtrl::PC_Assign:
                (void)PlayMgr.assign(PlayCtrl.paMedias);

                break;
            case E_PlayCtrl::PC_AppendPlay:
                (void)PlayMgr.insert(PlayCtrl.paMedias, true);

                break;
            case E_PlayCtrl::PC_Append:
                (void)PlayMgr.insert(PlayCtrl.paMedias, false);

                break;
            default:
                break;
            }
        }
    });
#endif
}

#if __winvc
bool CXController::setupMediaLib()
{
	static CFolderDlgEx FolderDlg;
    wstring strRootDir = FolderDlg.Show(L"设定根目录", L"请选择媒体库根目录");
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
#endif

void CXController::stop()
{
#if !__winvc
    m_threadPlayCtrl.cancel();
#endif

    m_model.close();

	m_OptionMgr.saveOption();
}

E_RenameRetCode CXController::renameMediaSet(CMediaSet& MediaSet, const wstring& strNewName)
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

CMediaDir* CXController::attachDir(const wstring& strDir)
{
	if (strDir.size() <= 3)
	{
		m_view.msgBox(L"不支持挂载驱动器，请选择具体目录");
		return NULL;
	}

	if (!m_model.getMediaLib().checkIndependentDir(strDir, false))
	{
		m_view.msgBox(L"请选择与根目录不相关的目录");
		return NULL;
	}

    bool bExist = m_OptionMgr.getOption().plAttachDir.anyFirst([&](const wstring& strAttachDir) {
        return strutil::matchIgnoreCase(strAttachDir, strDir) || fsutil::CheckSubPath(strAttachDir, strDir);
    });
	if (bExist)
	{
		m_view.msgBox(L"所选目录已被挂载，请选择其它目录");
		return NULL;
	}

	return m_model.attachDir(strDir);
}

bool CXController::renameMedia(const IMedia& media, const wstring& strNewName)
{
	if (strNewName.find_first_of(g_strInvalidMediaName) != __wnpos)
	{
		m_view.msgBox(L"名称含特殊字符！");
		return false;
	}

	cauto strOldOppPath = media.GetPath();

	wstring strExtName = media.GetFileTypeString();
	if (!strExtName.empty())
	{
        strExtName = __wcDot + strExtName;
	}

	cauto strOldAbsPath = media.GetAbsPath();
	cauto strNewAbsPath = fsutil::GetParentDir(strOldAbsPath) + __wcPathSeparator + strNewName + strExtName;
	
	bool bDir = media.IsDir();
    if (fsutil::existPath(strOldAbsPath, bDir))
	{
		bool bRet = false;
        m_model.getPlayMgr().renameFile(bDir, strOldAbsPath, strNewAbsPath, [&]() {
			if (!fsutil::moveFile(strOldAbsPath, strNewAbsPath))
			{
                m_view.msgBox(L"重命名失败: \n\n\t" + media.GetAbsPath());
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

	CMediaRes *pMediaRes = m_model.getMediaLib().findSubPath(media);
	if (pMediaRes)
	{
        pMediaRes->setName(strNewName + strExtName);
	}

	cauto strNewOppPath = fsutil::GetParentDir(strOldOppPath) + __wcPathSeparator + strNewName + strExtName;
	return m_model.renameMedia(strOldOppPath, strNewOppPath, bDir);
}

void CXController::moveMediaFile(const TD_IMediaList& lstMedias, const wstring& strDir)
{
	TD_IMediaList lstSrcMedias;
	lstMedias([&](IMedia& media){
		if (!strutil::matchIgnoreCase(fsutil::GetParentDir(media.GetPath()), strDir))
		{
			lstSrcMedias.add(media);
		}
	});
	__Ensure(lstSrcMedias);
	__Ensure(m_view.msgBox(L"确认移动选中的文件?", true));

	SMap<wstring, wstring> mapMovedFiles;

	lstSrcMedias([&](IMedia& media){
        cauto strSrcPath = media.GetPath();
		if (mapMovedFiles.includes(strSrcPath))
		{
			return;
		}

        cauto strDstPath = strDir + __wcPathSeparator + media.GetName();
		cauto strDstAbsPath = m_model.getMediaLib().toAbsPath(strDir) + __wcPathSeparator + media.GetName();

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
		
		m_model.getPlayMgr().moveFile(media.GetAbsPath(), strDstAbsPath, [&]() {
			(void)fsutil::removeFile(strDstAbsPath);
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

	(void)m_model.moveFiles(strDir, mapMovedFiles);

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

int CXController::addPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist)
{
	wstring strOppPath;
	SArray<wstring> arrOppPaths;
	for (cauto strFile : lstFiles)
	{
		strOppPath = m_model.getMediaLib().toOppPath(strFile);
		if (strOppPath.empty())
		{
            m_view.msgBox((L"添加曲目失败，请选择以下目录中的文件: \n\n\t"
				+ m_model.getMediaLib().GetAbsPath()).c_str());
			return 0;
		}

		arrOppPaths.add(strOppPath);
	}

	__EnsureReturn(m_model.getPlaylistMgr().AddPlayItems(arrOppPaths, Playlist, 0), -1);

	return arrOppPaths.size();
}

int CXController::addAlbumItems(const list<wstring>& lstFiles, CAlbum& Album)
{
	wstring strSingerOppPath = Album.GetBaseDir();

	list<wstring> lstOppPaths;
	for (cauto strFile : lstFiles)
	{
		wstring strOppPath = m_model.getMediaLib().toOppPath(strFile);
		if (!fsutil::CheckSubPath(strSingerOppPath, strOppPath))
		{
            m_view.msgBox((L"添加专辑曲目失败，请选择以下目录中的文件: \n\n\t" + strSingerOppPath).c_str());
			return 0;
		}

		lstOppPaths.push_back(strOppPath);
	}

	__EnsureReturn(m_model.getSingerMgr().AddAlbumItems(lstOppPaths, Album), -1);

	return lstOppPaths.size();
}

#if __winvc
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
