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

#include "dlg/UserDlg.h"

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
				__app->Quit();
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
		CMainApp::msgBox(L"����ִ�а�װ");
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
				__app->Quit();
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
	case ID_SignupUser:
	{
		m_view.m_ResModule.ActivateResource();
		CUserDlg UserDlg(m_view);
		(void)UserDlg.DoModal();
	}
	
	break;
	case ID_DeployUser:
		m_model.getUserMgr().deployUser();

		break;
	case ID_DeployMdl:
	{
		bool bDeploySingerImg = CMainApp::confirmBox(L"�Ƿ񷢲�����ͼƬ");
		m_view.deployMdl(bDeploySingerImg);
	}
	
	break;
	case ID_DeployMedias:
	{
		UINT uCount = 0;
		wstring strTip;
		bool bRet = m_view.showProgressDlg(L"������Ŀ", [&](CProgressDlg& ProgressDlg) {
			TD_IMediaList paMedias;
			__xmedialib.GetAllMedias(paMedias);
			set<wstring> setFiles;
			for (auto itr = paMedias.begin(); itr != paMedias.end(); )
			{
				if (setFiles.insert((*itr)->GetPath()).second)
				{
					++itr;
				}
				else
				{
					itr = paMedias.erase(itr);
				}
			}
			ProgressDlg.SetProgress(0, paMedias.size());

			uCount = m_model.deployXmsc(paMedias, [&](cwstr strTip, UINT uProgress, bool bFail) {
				if (!ProgressDlg.checkStatus())
				{
					return false;
				}

				if (bFail)
				{
					return ProgressDlg.confirmBox(strTip + L"���Ƿ������");
				}
				else
				{
					ProgressDlg.SetStatusText(strTip, 1);
					return true;
				}
			});

			strTip = L"�ѷ��� " + to_wstring(uCount) + L" ���ļ�";
			if (0 == uCount)
			{
				ProgressDlg.SetStatusText(strTip);
				return;
			}
			ProgressDlg.Close();
		});

		__EnsureBreak(bRet && uCount);
		__EnsureBreak(__app->confirmBox(strTip + L"���Ƿ񷢲�ý��⣿"));
		m_view.deployMdl(false);
	}

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
		CMediaDir *pDir = m_view.showChooseDirDlg(L"ѡ�񵼳�Ŀ¼");
		__EnsureBreak(pDir);

		pDir->clear();
		CMediaResPanel::RefreshMediaResPanel();
		m_view.exportDir(*pDir);
	}
	
	break;
	case ID_Snapshot:
	{
		m_view.getModel().refreshMediaLib();
		CMediaDir* pDir = m_view.showChooseDirDlg(L"ѡ�����Ŀ¼");
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

	TD_MediaSetList paMediaSets;
	CMediaSetDlg MediaSetDlg(m_view, __xmedialib, paMediaSets, L"ѡ������");
	if (IDOK != MediaSetDlg.DoModal())
	{
		return;
	}

	TD_IMediaList paMedias;
	for (auto pMediaSet : paMediaSets)
	{
		pMediaSet->GetMedias(paMedias);
	}

	m_view.verifyMedia(TD_MediaList(paMedias));
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

			ProgressDlg.SetStatusText(media.GetPath(), 1);

			return true;
		};
		m_view.getModel().checkDuplicateMedia(eMode, TD_MediaList(lstPlayItems), cb, arrResult);
		m_view.getModel().checkDuplicateMedia(eMode, TD_MediaList(lstAlbumItems), cb, arrResult);
		
		if (!arrResult)
		{
			ProgressDlg.SetStatusText(L"�����ɣ�δ�����ظ���Ŀ");
		}
		else
		{
			ProgressDlg.Close();
		}
	};

	if (!m_view.showProgressDlg(L"����ظ���Ŀ", lstPlayItems.size() + lstAlbumItems.size(), fnCheck))
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

	CMediaDir *pSrcDir = m_view.showChooseDirDlg(L"ѡ���һ��Ŀ¼");
	if (NULL == pSrcDir)
	{
		return;
	}

	if (pSrcDir == &__medialib)
	{
		m_view.checkSimilarFile(*pSrcDir);
		return;
	}

	CMediaDir* pDstDir = m_view.showChooseDirDlg(L"ѡ��ڶ���Ŀ¼", false);
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
			CMainApp::msgBox(L"��ѡ��ͬ��Ŀ¼��");
			return;
		}
		else if (fsutil::CheckSubPath(strSrcDir, strDstDir)
			|| fsutil::CheckSubPath(strDstDir, strSrcDir))
		{
			CMainApp::msgBox(L"��ѡ��û�д�����ϵ��Ŀ¼��");
			return;
		}
		
		m_view.checkSimilarFile(*pSrcDir, *pDstDir);
	}
}

void CPlayerView::_addInMedia()
{
	tagFileDlgOpt FileDlgOpt;
	FileDlgOpt.strTitle = L"ѡ���ļ�����";
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
		CMainApp::msgBox(L"��ѡ�� " + strRootDir + L" ������ļ���");
		return;
	}
	
	(void)m_view.showProgressDlg(L"�����ⲿ�ļ�", lstFiles.size(), [&](CProgressDlg& ProgressDlg) {
		m_view.addInMedia(lstFiles, ProgressDlg);
	});
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
