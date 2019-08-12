#include "stdafx.h"

#include "PlayerApp.h"

#include "../view/resource.h"

static class __afxInit
{
public:
	__afxInit()
	{
		afxCurrentInstanceHandle = _AtlBaseModule.GetModuleInstance();
		afxCurrentResourceHandle = _AtlBaseModule.GetResourceInstance();
	}
} afxInit;

class CPlayerApp : public CMainApp
{
public:
	CPlayerApp()
		: m_view(genView(m_controller, m_model))
		, m_model(m_view.getModelObserver())
		, m_controller(m_view, m_model)
	{
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

	CModel m_model;

	CPlayerController m_controller;
};

CPlayerApp theApp;

bool CPlayerController::init()
{
	fsutil::setWorkDir(fsutil::getModuleDir());

	return ((CModel&)m_model).init();
}

bool CPlayerController::start()
{
	CMainApp::RegHotkey(tagHotkeyInfo('B', E_HotkeyFlag::HKF_Control, ID_Backup));

	CMainApp::RegHotkey(tagHotkeyInfo('R', E_HotkeyFlag::HKF_Control, ID_PlayRecord));

	CMainApp::RegHotkey(tagHotkeyInfo(VK_F5, E_HotkeyFlag::HKF_Null, ID_REFRESH_ROOT));

	CMainApp::RegHotkey(tagHotkeyInfo(VK_F3, E_HotkeyFlag::HKF_Null, ID_FIND));
	CMainApp::RegHotkey(tagHotkeyInfo('F', E_HotkeyFlag::HKF_Control, ID_FIND));

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

	return CController::start();
}

void CPlayerController::stop()
{
	CController::stop();
	m_model.close();
}

bool CPlayerController::handleCommand(UINT uID)
{
	switch (uID)
	{
	case ID_MODIFY_ROOT:
	{
		static CFolderDlgEx FolderDlg;
		wstring strRootDir = FolderDlg.Show(L"设定根目录", L"请选择媒体库根目录");
		if (!strRootDir.empty())
		{
			(void)m_model.initRootMediaRes(strRootDir);
		}
	}

	break;
	case ID_REFRESH_ROOT:
		m_model.refreshRootMediaRes();

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
		case ID_DemandSinger_TAI:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_TAI;
			break;
		case ID_DemandSinger_EN:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandSinger_EUR:
			eDemandMode = E_DemandMode::DM_DemandSinger;
			eLanguageType = E_LanguageType::LT_EUR;
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
		case ID_DemandAlbum_TAI:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_TAI;
			break;
		case ID_DemandAlbum_EN:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandAlbum_EUR:
			eDemandMode = E_DemandMode::DM_DemandAlbum;
			eLanguageType = E_LanguageType::LT_EUR;
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
		case ID_DemandAlbumItem_TAI:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_TAI;
			break;
		case ID_DemandAlbumItem_EN:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandAlbumItem_EUR:
			eDemandMode = E_DemandMode::DM_DemandAlbumItem;
			eLanguageType = E_LanguageType::LT_EUR;
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
		case ID_DemandPlaylist_TAI:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_TAI;
			break;
		case ID_DemandPlaylist_EN:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandPlaylist_EUR:
			eDemandMode = E_DemandMode::DM_DemandPlaylist;
			eLanguageType = E_LanguageType::LT_EUR;
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
		case ID_DemandPlayItem_TAI:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_TAI;
			break;
		case ID_DemandPlayItem_EN:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_EN;
			break;
		case ID_DemandPlayItem_EUR:
			eDemandMode = E_DemandMode::DM_DemandPlayItem;
			eLanguageType = E_LanguageType::LT_EUR;
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
