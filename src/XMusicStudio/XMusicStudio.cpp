#include "stdafx.h"

#include "../view/resource.h"

#include "../controller.h"

class CController : public CXController, public IController
{
public:
	CController(IPlayerView& view, IModel& model)
		: CXController(view, model)
	{
	}

public:
	void start() override;

	void stop()
	{
		CXController::stop();

		extern bool g_bDisableDump;
		g_bDisableDump = true;
#if __isdebug
		exit(0);
#endif
	}

	bool handleCommand(UINT uID) override;
};

void CController::start()
{
	CMainApp::RegHotkey(tagHotkeyInfo('B', E_HotkeyFlag::HKF_Control, ID_Backup));

	CMainApp::RegHotkey(tagHotkeyInfo('R', E_HotkeyFlag::HKF_Control, ID_PlayRecord));

	CMainApp::RegHotkey(tagHotkeyInfo(VK_F5, E_HotkeyFlag::HKF_Null, ID_REFRESH_ROOT));

	CMainApp::RegHotkey(tagHotkeyInfo(VK_F3, E_HotkeyFlag::HKF_Null, ID_Find));
	CMainApp::RegHotkey(tagHotkeyInfo('F', E_HotkeyFlag::HKF_Control, ID_Find));

	CMainApp::RegHotkey(tagHotkeyInfo('D', E_HotkeyFlag::HKF_Control, ID_VERIFY));
	CMainApp::RegHotkey(tagHotkeyInfo('E', E_HotkeyFlag::HKF_Control, ID_ExportMedia));

	CMainApp::RegHotkey(tagHotkeyInfo(VK_F1, E_HotkeyFlag::HKF_Control, ID_DemandSinger, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_F2, E_HotkeyFlag::HKF_Control, ID_DemandAlbum, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_F3, E_HotkeyFlag::HKF_Control, ID_DemandAlbumItem, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_F4, E_HotkeyFlag::HKF_Control, ID_DemandPlayItem, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_F5, E_HotkeyFlag::HKF_Control, ID_DemandPlaylist, true));

	CMainApp::RegHotkey(tagHotkeyInfo(VK_F12, E_HotkeyFlag::HKF_Control, ID_SHOW, true));

	CMainApp::RegHotkey(tagHotkeyInfo('P', E_HotkeyFlag::HKF_Control, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_UP, E_HotkeyFlag::HKF_Control, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_DOWN, E_HotkeyFlag::HKF_Control, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_UP, E_HotkeyFlag::HKF_Alt, true));
	CMainApp::RegHotkey(tagHotkeyInfo(VK_DOWN, E_HotkeyFlag::HKF_Alt, true));
	
	CXController::start();
}

bool CController::handleCommand(UINT uID)
{
	switch (uID)
	{
	case ID_QUIT:
		__app->Quit();

		break;
	case ID_MODIFY_ROOT:
		if (setupMediaLib())
		{
			if (m_model.getPlayMgr().playingItems())
			{
				m_model.getPlayMgr().SetPlayStatus(E_PlayStatus::PS_Play);
			}
		}
		
		break;
	case ID_REFRESH_ROOT:
	{
		__waitCursor;

		m_model.refreshMediaLib();
	}
	
	break;
	default:
	{
		E_DemandMode eDemandMode = (E_DemandMode)0;
		E_LanguageType eLanguageType = E_LanguageType::LT_None;
		switch (uID)
		{
		case ID_DemandSinger:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			break;
		case ID_DemandSinger_CN:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_CN;
			break;
		case ID_DemandSinger_HK:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_HK;
			break;
		case ID_DemandSinger_KR:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_KR;
			break;
		case ID_DemandSinger_JP:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_JP;
			break;
		case ID_DemandSinger_EN:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandSinger_Other:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_Other;
			break;

		case ID_DemandAlbum:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			break;
		case ID_DemandAlbum_CN:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_CN;
			break;
		case ID_DemandAlbum_HK:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_HK;
			break;
		case ID_DemandAlbum_KR:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_KR;
			break;
		case ID_DemandAlbum_JP:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_JP;
			break;
		case ID_DemandAlbum_EN:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandAlbum_Other:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_Other;
			break;

		case ID_DemandAlbumItem:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			break;
		case ID_DemandAlbumItem_CN:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_CN;
			break;
		case ID_DemandAlbumItem_HK:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_HK;
			break;
		case ID_DemandAlbumItem_KR:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_KR;
			break;
		case ID_DemandAlbumItem_JP:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_JP;
			break;
		case ID_DemandAlbumItem_EN:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandAlbumItem_Other:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_Other;
			break;

		case ID_DemandPlaylist:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			break;
		case ID_DemandPlaylist_CN:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_CN;
			break;
		case ID_DemandPlaylist_HK:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_HK;
			break;
		case ID_DemandPlaylist_KR:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_KR;
			break;
		case ID_DemandPlaylist_JP:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_JP;
			break;
		case ID_DemandPlaylist_EN:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandPlaylist_Other:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_Other;
			break;

		case ID_DemandPlayItem:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			break;
		case ID_DemandPlayItem_CN:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_CN;
			break;
		case ID_DemandPlayItem_HK:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_HK;
			break;
		case ID_DemandPlayItem_KR:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_KR;
			break;
		case ID_DemandPlayItem_JP:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_JP;
			break;
		case ID_DemandPlayItem_EN:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandPlayItem_Other:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_Other;
			break;
		default:
			return false;
		}

		(void)m_model.getPlayMgr().demand(eDemandMode, eLanguageType);
	}
	break;
	}

	return true;
}

struct tagStartup
{
	tagStartup()
	{
		fsutil::setWorkDir(fsutil::getModuleDir());

		extern void InitMinDump(const string&);
		InitMinDump("xmusicstudio_dump_");
	}
};

class CApp : private tagStartup, public CMainApp
{
public:
	CApp()
		: m_view(genView(m_controller, m_model))
		, m_controller(m_view, m_model)
		, m_model(m_view.getModelObserver(), m_controller.getOption())
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);
		if (!cmdInfo.m_strFileName.IsEmpty())
		{
			m_model.convertXpk(wstring(cmdInfo.m_strFileName));
			exit(0);
		}

		if (CMainApp::checkRuning())
		{
			exit(0);
			return;
		}

		(void)m_controller.initOption();
	}

	IView& getView() override
	{
		return m_view;
	}

	IController& getController() override
	{
		return m_controller;
	}

public:
	IPlayerView& m_view;

	CController m_controller;

	CModel m_model;
};
static CApp g_app;
