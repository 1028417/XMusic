#include "stdafx.h"

#include "PlayerView.h"

#include "dlg/option/OptionDlg.h"

#include "dlg/FindDlg.h"

#include "dlg/PlayRecordDlg.h"

#include "dlg/DuplicateMediaDlg.h"

#include "dlg/NewSongDlg.h"

#include "dlg/MediaSetDlg.h"

#include "dlg/BackupDlg.h"

#include "dlg/WholeTrackDlg.h"

#define __semilightFont "./font/Microsoft-YaHei-Semilight-11.0.ttc"
//#define __lightFont "./font/msyhl-6.23.ttc"

__ViewExt IPlayerView& genView(IXController& controller, IModel& model)
{
	static CPlayerView inst(controller, model);
	return inst;
}

CMainWnd* CPlayerView::show()
{
	(void)AddFontResourceExA(__semilightFont, FR_PRIVATE, NULL);
	//(void)AddFontResourceExA(__lightFont, FR_PRIVATE, NULL);

	if (!m_view.init())
	{
		return NULL;
	}

	return &m_view.m_MainWnd;
}

void CPlayerView::close()
{
	m_view.m_PlayCtrl.close();

	(void)RemoveFontResourceExA(__semilightFont, FR_PRIVATE, NULL);
	//(void)RemoveFontResourceExA(__lightFont, FR_PRIVATE, NULL);
}

bool CPlayerView::handleCommand(UINT uID)
{
	switch (uID)
	{
	case ID_SHOW:
		m_view.foregroundMainWnd();

		break;
	case ID_QUIT:
		CMainApp::GetMainApp()->Quit();

		break;
	case ID_AttachDir:
		m_view.m_MediaResPage.attachDir();

		break;
	case ID_Setting:
	{
		m_view.m_ResModule.ActivateResource();
		COptionDlg OptionDlg(m_view);
		(void)OptionDlg.DoModal();

		m_view.m_MainWnd.showMenu(!m_view.getOption().bHideMenuBar);

		//CDockView *pDockView = m_view.m_MainWnd.GetView(E_DockViewType::DVT_DockCenter);
		//if (NULL != pDockView)
		//{
		//	pDockView->SetTabStyle(m_bShowMenu? E_TabStyle::TS_Bottom:E_TabStyle::TS_Top);
		//}
	}

	break;
	case ID_Backup:
	{
		m_view.m_ResModule.ActivateResource();
		CBackupDlg BackupDlg(m_view);
		(void)BackupDlg.DoModal();
	}
	
	break;
	case ID_PlayRecord:
		__EnsureBreak(m_view.m_MainWnd.IsWindowEnabled());
		m_view.m_ResModule.ActivateResource();
		(void)CPlayRecordDlg(m_view).DoModal();

		break;
	case ID_WholeTrack:
		m_view.getModel().refreshMediaLib();
		(void)CWholeTrackDlg(m_view).DoModal();

		break;
	case ID_CheckSimilar:
		_checkSimilarFile();

		break;
	case ID_ExportDir:
	{
		m_view.getModel().refreshMediaLib();
		CMediaDir* pDir = m_view.showChooseDirDlg(L"选择导出目录");
		__EnsureBreak(pDir);

		m_view.exportDir(*pDir);
	}
	
	break;
	case ID_Snapshot:
	{
		m_view.getModel().refreshMediaLib();
		CMediaDir* pDir = m_view.showChooseDirDlg(L"选择快照目录");
		__EnsureBreak(pDir);

		m_view.snapshotDir(*pDir);
	}
	
	break;
	case ID_FIND:
	{
		__EnsureBreak(m_view.m_MainWnd.IsWindowEnabled());

		m_view.m_ResModule.ActivateResource();
		(void)CFindDlg(m_view).DoModal();
	}

	break;
	case ID_ExportMedia:
		__EnsureBreak(m_view.m_MainWnd.IsWindowEnabled());
		m_view.exportMediaSet(m_view.getMediaLib());

		break;
	case ID_VERIFY:
		__EnsureBreak(m_view.m_MainWnd.IsWindowEnabled());
		this->_verifyMedia();

		break;
	case ID_CheckDuplicate:
		_checkDuplicateMedia(E_CheckDuplicateMode::CDM_SamePath);
		break;
	case ID_CheckDuplicateName:
		_checkDuplicateMedia(E_CheckDuplicateMode::CDM_SameName);
		break;
	case ID_CheckDuplicateTitle:
		_checkDuplicateMedia(E_CheckDuplicateMode::CDM_SameTitle);
		break;

	case ID_AddIn:		
		_addInMedia();
	
		break;
	case ID_ViewNewSong:
		m_view.foregroundMainWnd();

		m_view.m_ResModule.ActivateResource();
		CNewSongDlg(m_view).DoModal();

		break;
	default:
		return false;
	}

	return true;
}

bool CPlayerView::handleHotkey(const tagHotkeyInfo& HotkeyInfo)
{
	switch (HotkeyInfo.uKey)
	{
	case 'P':
		if (E_PlayStatus::PS_Play == m_view.getPlayMgr().GetPlayStatus())
		{
			m_view.m_PlayCtrl.handlePlaySpiritButtonClick(ST_PlaySpiritButton::PSB_Pause);
		}
		else
		{
			m_view.m_PlayCtrl.handlePlaySpiritButtonClick(ST_PlaySpiritButton::PSB_Play);
		}

		break;
	case VK_UP:
		if (E_HotkeyFlag::HKF_Control == HotkeyInfo.eFlag)
		{
			m_view.m_PlayCtrl.handlePlaySpiritButtonClick(ST_PlaySpiritButton::PSB_Last);
		}
		else if (E_HotkeyFlag::HKF_Alt == HotkeyInfo.eFlag)
		{
			m_view.m_PlayCtrl.setVolume(5);
		}

		break;
	case VK_DOWN:
		if (E_HotkeyFlag::HKF_Control == HotkeyInfo.eFlag)
		{
			m_view.m_PlayCtrl.handlePlaySpiritButtonClick(ST_PlaySpiritButton::PSB_Next);
		}
		else if (E_HotkeyFlag::HKF_Alt == HotkeyInfo.eFlag)
		{
			m_view.m_PlayCtrl.setVolume(-5);
		}

		break;
	default:
		return false;
	}

	return true;
}

void CPlayerView::_verifyMedia()
{
	m_view.m_ResModule.ActivateResource();

	TD_MediaSetList lstMediaSets;
	CMediaSetDlg MediaSetDlg(m_view, m_view.getMediaLib(), lstMediaSets, L"选择检测项");
	if (IDOK != MediaSetDlg.DoModal())
	{
		return;
	}

	TD_MediaList lstMedias;
	lstMediaSets([&](CMediaSet& MediaSet){
		MediaSet.GetMedias(lstMedias);
	});

	m_view.verifyMedia(lstMedias);
}

void CPlayerView::_checkDuplicateMedia(E_CheckDuplicateMode eMode)
{
	TD_MediaList lstPlayItems;
	m_view.getPlaylistMgr().GetAllMedias(lstPlayItems);
	TD_MediaList lstAlbumItems;
	m_view.getSingerMgr().GetAllMedias(lstAlbumItems);

	__Ensure(lstPlayItems || lstAlbumItems);

	SArray<TD_MediaList> arrResult;

	auto fnCheck = [&](CProgressDlg& ProgressDlg) {
		auto cb = [&](CMedia& media) {
			if (ProgressDlg.checkCancel())
			{
				return false;
			}

			ProgressDlg.SetStatusText(media.GetPath().c_str(), 1);

			return true;
		};
		m_view.getModel().checkDuplicateMedia(eMode, lstPlayItems, cb, arrResult);
		m_view.getModel().checkDuplicateMedia(eMode, lstAlbumItems, cb, arrResult);
		
		if (!arrResult)
		{
			ProgressDlg.SetStatusText(L"检查完成，未发现重复曲目");
		}
		else
		{
			ProgressDlg.Close();
		}
	};

	CProgressDlg ProgressDlg(fnCheck, lstPlayItems.size() + lstAlbumItems.size());
	if (IDOK == ProgressDlg.DoModal(L"检查重复曲目", &m_view.m_MainWnd) && arrResult)
	{
		CResGuard ResGuard(m_view.m_ResModule);
		CDuplicateMediaDlg dlg(m_view, arrResult);
		(void)dlg.DoModal();
	}
}

void CPlayerView::_checkSimilarFile()
{
	m_view.getModel().refreshMediaLib();

	CMediaDir* pSrcDir = m_view.showChooseDirDlg(L"选择第一个目录");
	if (NULL == pSrcDir)
	{
		return;
	}

	if (pSrcDir == &m_view.getMediaLib())
	{
		m_view.checkSimilarFile(*pSrcDir);
		return;
	}

	CMediaDir* pDstDir = m_view.showChooseDirDlg(L"选择第二个目录", false);
	if (NULL == pDstDir)
	{
		m_view.checkSimilarFile(*pSrcDir);
	}
	else
	{
		wstring strSrcDir = pSrcDir->GetPath();
		wstring strDstDir = pDstDir->GetPath();
		if (strutil::matchIgnoreCase(strSrcDir, strDstDir))
		{
			CMainApp::showMsg(L"请选择不同的目录！");
			return;
		}
		else if (fsutil::CheckSubPath(strSrcDir, strDstDir)
			|| fsutil::CheckSubPath(strDstDir, strSrcDir))
		{
			CMainApp::showMsg(L"请选择没有从属关系的目录！");
			return;
		}
		
		m_view.checkSimilarFile(*pSrcDir, *pDstDir);
	}
}

void CPlayerView::_addInMedia()
{
	tagFileDlgOpt FileDlgOpt;
	FileDlgOpt.strTitle = L"选择文件合入";
	FileDlgOpt.strFilter = __MediaFilter;
	FileDlgOpt.strInitialDir = fsutil::GetParentDir(m_view.getMediaLib().GetAbsPath());
	static CFileDlgEx fileDlg(FileDlgOpt);

	list<wstring> lstFiles;
	wstring strDir = fileDlg.ShowOpenMulti(lstFiles);		
	__Ensure(!strDir.empty());

	cauto strRootDir = m_view.getMediaLib().GetAbsPath();
	if (strutil::matchIgnoreCase(strDir, strRootDir)
		|| fsutil::CheckSubPath(strRootDir, strDir))
	{
		CMainApp::showMsg(L"请选择 " + strRootDir + L" 以外的文件！");
		return;
	}

	auto cbAddInMedia = [&](CProgressDlg& ProgressDlg) {
		auto cbProgress = [&](const wstring& strFile) {
			ProgressDlg.SetStatusText(strFile.c_str(), 1);

			return !ProgressDlg.checkCancel();
		};

		auto cbConfirm = [&](CSearchMediaInfo& SearchMediaInfo, tagMediaResInfo& MediaResInfo)
		{
			wstring strText = fsutil::GetFileName(MediaResInfo.strPath)
				+ L"\n大小: " + MediaResInfo.strFileSize + L"字节\n时长: "
				+ CMedia::GetDurationString(CMediaOpaque::checkDuration(MediaResInfo.strPath))
				+ L"\n\n是否更新以下曲目？\n"
				+ fsutil::GetFileName(SearchMediaInfo.m_strAbsPath)
				+ L"\n大小: " + SearchMediaInfo.GetFileSize() + L"字节\n时长: "
				+ CMedia::GetDurationString(CMediaOpaque::checkDuration(SearchMediaInfo.m_strAbsPath))
				+ L"\n目录: " + m_view.getMediaLib().toOppPath(fsutil::GetParentDir(SearchMediaInfo.m_strAbsPath))
				+ L"\n\n关联: ";

			SearchMediaInfo.m_lstMedias([&](CMedia& media) {
				strText.append(L"\n" + media.m_pParent->GetLogicPath());
			});

			int iRet = ProgressDlg.showMsgBox(strText, L"匹配到文件", MB_YESNOCANCEL);
			if (IDCANCEL == iRet)
			{
				return E_MatchResult::MR_Ignore;
			}

			__EnsureReturn(IDYES == iRet, E_MatchResult::MR_No);

			return E_MatchResult::MR_Yes;
		};

		UINT uRet = m_view.getController().addInMedia(lstFiles, cbProgress, cbConfirm);

		ProgressDlg.SetStatusText((L"匹配结束, 更新" + to_wstring(uRet) + L"个曲目").c_str());
	};
	
	CProgressDlg ProgressDlg(cbAddInMedia, lstFiles.size());
	(void)ProgressDlg.DoModal(L"合入外部文件");
}

bool CPlayerView::showMsgBox(const wstring& strMsg, bool bWarning)
{
	if (bWarning)
	{
		return CMainApp::showConfirmMsg(strMsg);
	}
	else
	{
		CMainApp::showMsg(strMsg);
		return true;
	}
}
