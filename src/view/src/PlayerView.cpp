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

static thread g_thread;
CMainWnd* CPlayerView::show()
{
	auto& strRootDir = m_controller.getOption().strRootDir;
	bool bExistMedialib = (!strRootDir.empty() && fsutil::existDir(strRootDir));
	if (bExistMedialib)
	{
		thread thr([&] {
			int nRet = CPlayer::InitSDK();
			if (nRet != 0)
			{
				//g_logger << "initPlaySDK fail: " >> nRet;
				return;
			}

			if (!m_model.initMediaLib(false))
			{
				CMainApp::GetMainApp()->Quit();
				return;
			}
			
			__appSync([&] {
				m_view.initView();
				m_model.getPlayMgr().tryPlay();
			}, false);

			CFolderDlg::preInit();
		});
		g_thread.swap(thr);
	}

	if (!CPlaySpirit::inst().Create())
	{
		CMainApp::msgBox(L"请先执行安装");
		return NULL;
	}

	(void)AddFontResourceExA(__semilightFont, FR_PRIVATE, NULL);
	//(void)AddFontResourceExA(__lightFont, FR_PRIVATE, NULL);

	if (!m_view.show())
	{
		return NULL;
	}

	if (!bExistMedialib)
	{
		__async([=] {
			if (!m_controller.setupMediaLib())
			{
				CMainApp::GetMainApp()->Quit();
				return;
			}

			m_model.getPlayMgr().tryPlay();
		});
	}

	return &m_view.m_MainWnd;
}

void CPlayerView::close()
{
	m_view.m_PlayCtrl.close();

	(void)RemoveFontResourceExA(__semilightFont, FR_PRIVATE, NULL);
	//(void)RemoveFontResourceExA(__lightFont, FR_PRIVATE, NULL);

	if (g_thread.joinable())
	{
		g_thread.join();
	}
}

bool CPlayerView::handleCommand(UINT uID)
{
	switch (uID)
	{
	case ID_SHOW:
		m_view.m_MainWnd.setForeground();

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
	case ID_PlayRecord:
		__EnsureBreak(m_view.m_MainWnd.IsWindowEnabled());
		m_view.m_ResModule.ActivateResource();
		(void)CPlayRecordDlg(m_view).DoModal();

		break;
	case ID_Backup:
	{
		m_view.m_ResModule.ActivateResource();
		CBackupDlg BackupDlg(m_view);
		(void)BackupDlg.DoModal();
	}

	break;
	case ID_DeployMdl:
		m_view.showProgressDlg(L"发布媒体库", [&](CProgressDlg& ProgressDlg) {
			m_model.deployMdl([&](UINT, cwstr strTip) {
				ProgressDlg.SetStatusText(strTip.c_str());
				return ProgressDlg.checkStatus();
			});
		});

		break;
	case ID_DeployMedias:
		m_view.showProgressDlg(L"发布曲目", [&](CProgressDlg& ProgressDlg) {
			TD_IMediaList paMedias;
			__xmedialib.GetAllMedias(paMedias);
			auto uTotalProgress = paMedias.size();
			auto uCount = m_model.deployArti(paMedias, [&](cwstr strTip, UINT uProgress, bool bFail) {
				if (!ProgressDlg.checkStatus())
				{
					return false;
				}

				if (bFail)
				{
					return ProgressDlg.confirmBox(strTip + L"，是否继续？");
				}
				else
				{
					ProgressDlg.SetStatusText((L"正在发布: " + strTip).c_str());
					ProgressDlg.SetProgress(uProgress, uTotalProgress);
					return true;
				}
			});

			CString cstrTip;
			cstrTip.Format(L"已发布 %d 个文件", uCount);
			ProgressDlg.SetStatusText(cstrTip);
		});

		break;
	case ID_AttachDir:
		m_view.m_MediaResPage.attachDir();

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
		CMediaDir *pDir = m_view.showChooseDirDlg(L"选择导出目录");
		__EnsureBreak(pDir);

		pDir->clear();
		CMediaResPanel::RefreshMediaResPanel();
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
	case ID_Find:
	{
		__EnsureBreak(m_view.m_MainWnd.IsWindowEnabled());

		m_view.m_ResModule.ActivateResource();
		(void)CFindDlg(m_view).DoModal();
	}

	break;
	case ID_ExportMedia:
		__EnsureBreak(m_view.m_MainWnd.IsWindowEnabled());
		m_view.exportMediaSet(__xmedialib);

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
	case ID_CheckSimilarTitle:
		_checkDuplicateMedia(E_CheckDuplicateMode::CDM_SimilarTitle);
		break;

	case ID_AddIn:		
		_addInMedia();
	
		break;
	case ID_ViewNewSong:
		m_view.m_MainWnd.setForeground();

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
		if (E_PlayStatus::PS_Play == m_view.getPlayMgr().playStatus())
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
	CMediaSetDlg MediaSetDlg(m_view, __xmedialib, lstMediaSets, L"选择检测项");
	if (IDOK != MediaSetDlg.DoModal())
	{
		return;
	}

	TD_IMediaList lstMedias;
	lstMediaSets([&](CMediaSet& MediaSet){
		MediaSet.GetMedias(lstMedias);
	});

	m_view.verifyMedia(TD_MediaList(lstMedias));
}

void CPlayerView::_checkDuplicateMedia(E_CheckDuplicateMode eMode)
{
	TD_IMediaList lstPlayItems;
	m_view.getPlaylistMgr().GetAllMedias(lstPlayItems);
	TD_IMediaList lstAlbumItems;
	m_view.getSingerMgr().GetAllMedias(lstAlbumItems);

	__Ensure(lstPlayItems || lstAlbumItems);

	SArray<TD_MediaList> arrResult;

	auto fnCheck = [&](CProgressDlg& ProgressDlg) {
		auto cb = [&](CMedia& media) {
			if (!ProgressDlg.checkStatus())
			{
				return false;
			}

			ProgressDlg.SetStatusText(media.GetPath().c_str(), 1);

			return true;
		};
		m_view.getModel().checkDuplicateMedia(eMode, TD_MediaList(lstPlayItems), cb, arrResult);
		m_view.getModel().checkDuplicateMedia(eMode, TD_MediaList(lstAlbumItems), cb, arrResult);
		
		if (!arrResult)
		{
			ProgressDlg.SetStatusText(L"检查完成，未发现重复曲目");
		}
		else
		{
			ProgressDlg.Close();
		}
	};

	if (!m_view.showProgressDlg(L"检查重复曲目", fnCheck, lstPlayItems.size() + lstAlbumItems.size()))
	{
		return;
	}
	if (arrResult)
	{
		CResGuard ResGuard(m_view.m_ResModule);
		CDuplicateMediaDlg dlg(m_view, arrResult);
		(void)dlg.DoModal();
	}
}

void CPlayerView::_checkSimilarFile()
{
	m_view.getModel().refreshMediaLib();

	CMediaDir *pSrcDir = m_view.showChooseDirDlg(L"选择第一个目录");
	if (NULL == pSrcDir)
	{
		return;
	}

	if (pSrcDir == &__medialib)
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
			CMainApp::msgBox(L"请选择不同的目录！");
			return;
		}
		else if (fsutil::CheckSubPath(strSrcDir, strDstDir)
			|| fsutil::CheckSubPath(strDstDir, strSrcDir))
		{
			CMainApp::msgBox(L"请选择没有从属关系的目录！");
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
	FileDlgOpt.strInitialDir = fsutil::GetParentDir(__medialib.path());
	static CFileDlgEx fileDlg(FileDlgOpt);

	list<wstring> lstFiles;
	wstring strDir = fileDlg.ShowOpenMulti(lstFiles);		
	__Ensure(!strDir.empty());

	cauto strRootDir = __medialib.path();
	if (strutil::matchIgnoreCase(strDir, strRootDir)
		|| fsutil::CheckSubPath(strRootDir, strDir))
	{
		CMainApp::msgBox(L"请选择 " + strRootDir + L" 以外的文件！");
		return;
	}
	
	(void)m_view.showProgressDlg(L"合入外部文件", [&](CProgressDlg& ProgressDlg) {
		m_view.addInMedia(lstFiles, ProgressDlg);
	}, lstFiles.size());
}

bool CPlayerView::msgBox(cwstr strMsg, bool bWarning)
{
	if (bWarning)
	{
		return CMainApp::confirmBox(strMsg);
	}
	else
	{
		CMainApp::msgBox(strMsg);
		return true;
	}
}
