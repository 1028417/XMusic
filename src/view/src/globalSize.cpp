
#include "stdafx.h"

#include <math.h>

#define uintRound(x) ((UINT)round(x))

#include <ShellScalingApi.h>

static HRESULT WINAPI GetDpiForMonitor(
	_In_ HMONITOR hmonitor,
	_In_ MONITOR_DPI_TYPE dpiType,
	_Out_ UINT *dpiX,
	_Out_ UINT *dpiY)
{
	HINSTANCE hInstWinSta = LoadLibraryW(L"SHCore.dll");

	if (hInstWinSta == nullptr) return E_NOINTERFACE;

	typedef HRESULT(WINAPI * PFN_GDFM)(HMONITOR, MONITOR_DPI_TYPE, UINT*, UINT*);

	PFN_GDFM pGetDpiForMonitor = (PFN_GDFM)GetProcAddress(hInstWinSta, "GetDpiForMonitor");

	if (pGetDpiForMonitor == nullptr) return E_NOINTERFACE;
	
	return pGetDpiForMonitor(hmonitor, dpiType, dpiX, dpiY);
}

// 开启对话框Per-Monitor DPI Aware支持(至少Win10)
static inline BOOL EnablePerMonitorDialogScaling()
{
	typedef BOOL(WINAPI *PFN_EnablePerMonitorDialogScaling)();
	PFN_EnablePerMonitorDialogScaling pEnablePerMonitorDialogScaling =
		(PFN_EnablePerMonitorDialogScaling)GetProcAddress(GetModuleHandleW(L"user32.dll"), (LPCSTR)2577);

	if (pEnablePerMonitorDialogScaling) return pEnablePerMonitorDialogScaling();

	return FALSE;
}

// 开启子窗体DPI消息(至少Win10)
static inline BOOL EnableChildWindowDpiMessage(_In_ HWND hWnd, _In_ BOOL bEnable)
{
	typedef BOOL(WINAPI *PFN_EnableChildWindowDpiMessage)(HWND, BOOL);
	PFN_EnableChildWindowDpiMessage pEnableChildWindowDpiMessage = 
		(PFN_EnableChildWindowDpiMessage)GetProcAddress(GetModuleHandleW(L"user32.dll"), "EnableChildWindowDpiMessage");

	if (pEnableChildWindowDpiMessage) return pEnableChildWindowDpiMessage(hWnd, bEnable);
	
	return FALSE;
}

#define __DPIDefault 96.0f

CGlobalSize::CGlobalSize()
{
	m_uLeftDockWidth = 285;
	m_uAlbumDockWidth = 215;

	m_uSmallIconSize = 44;
	m_uBigIconSize = 61;

	m_uHeadHeight = 24;
	m_uTabHeight = 24;
	m_uMenuItemHeight = 24;

	m_uPlayingItemHeight = 63;

	m_uTileHeight = 79;
	m_uIconSpace = 64;
	
	m_ColWidth_Name = 253;
	m_ColWidth_Time = 85;
	m_ColWidth_MediaTag_Title = 150;
	m_ColWidth_MediaTag_Artist = 90;
	m_ColWidth_MediaTag_Album = 120;

	m_ColWidth_Type = 48;
	m_ColWidth_FileSize = 65;
	m_ColWidth_Duration = 50;
	m_ColWidth_RelatedPlaylist = 105;
	m_ColWidth_RelatedSingerAlbum = 165;
	m_ColWidth_AddTime = 85;
}

void CGlobalSize::init()
{
	UINT uDPIX = 0;
	UINT uDPIY = 0;
	HRESULT hr = GetDpiForMonitor(MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTONEAREST), MDT_EFFECTIVE_DPI, &uDPIX, &uDPIY);
	if (S_OK != hr)
	{
		HDC hDCDesk = GetDC(NULL);
		uDPIX = GetDeviceCaps(hDCDesk, LOGPIXELSX);
		uDPIY = GetDeviceCaps(hDCDesk, LOGPIXELSY);
	}
	m_uDPI = (uDPIX + uDPIY) / 2;

	UINT uScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	UINT uScreenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_uScreenWidth = MAX(uScreenWidth, uScreenHeight);
	m_uScreenHeight = MIN(uScreenWidth, uScreenHeight);

	m_uMenuBarHeight = GetSystemMetrics(SM_CYMENU);

	m_uScrollbarWidth = GetSystemMetrics(SM_CYHSCROLL);

	float fWRate = 1;
	if (m_uScreenWidth > 1280)
	{
		fWRate = (float)m_uScreenWidth / 1280;

		m_uSmallIconSize = uintRound(m_uSmallIconSize*fWRate);
		m_uBigIconSize = uintRound(m_uBigIconSize*fWRate);

		m_uLeftDockWidth = uintRound(m_uLeftDockWidth*fWRate);

		m_uAlbumDockWidth = uintRound(m_uAlbumDockWidth*fWRate);

		for (auto p : { &m_ColWidth_Name, &m_ColWidth_MediaTag_Title
		   , &m_ColWidth_Name, &m_ColWidth_MediaTag_Title, &m_ColWidth_MediaTag_Artist
		   , &m_ColWidth_MediaTag_Album, &m_ColWidth_Type, &m_ColWidth_FileSize
		   , &m_ColWidth_Duration, &m_ColWidth_RelatedPlaylist, &m_ColWidth_AddTime
		   , &m_ColWidth_RelatedSingerAlbum, &m_ColWidth_Time })
		{
			*p = uintRound(*p*fWRate);
		}
	}

	float fHRate = 1;
	if (m_uScreenHeight > 720)
	{
		fHRate = (float)MIN(1080, m_uScreenHeight) / 720;
	}

	m_uHeadHeight = uintRound(m_uHeadHeight*fHRate*fHRate);
	m_uTabHeight = uintRound(m_uTabHeight*fHRate*fHRate);
	m_uMenuItemHeight = uintRound(m_uMenuItemHeight*fHRate*fHRate);

	m_uPlayingItemHeight = uintRound(m_uPlayingItemHeight*fHRate);
	
	m_uTileHeight = uintRound(m_uTileHeight*fHRate);
	m_uIconSpace = uintRound(m_uIconSpace*fHRate);

	float fDPIRate = __DPIDefault / m_uDPI;
	m_fSmallFontSize = (fHRate - 1.0f)*1.2f * fDPIRate * fDPIRate;

	m_fBigFontSize = m_fSmallFontSize * 1.8f;

	m_fMenuFontSize = (m_fBigFontSize + m_fSmallFontSize)/2.5f;
	m_fMenuBarFontSize = (m_fMenuFontSize + m_fSmallFontSize)/2.5f;

	m_uScreenWidth = uScreenWidth;
	m_uScreenHeight = uScreenHeight;
}
