
#include "stdafx.h"

#include <math.h>

#define uintRound(x) ((UINT)round(x))

// 开启子窗体DPI消息(至少Win10)
static BOOL EnableChildWindowDpiMessage(_In_ HWND hWnd, _In_ BOOL bEnable)
{
	typedef BOOL(WINAPI *PFN_EnableChildWindowDpiMessage)(HWND, BOOL);
	PFN_EnableChildWindowDpiMessage pEnableChildWindowDpiMessage = 
		(PFN_EnableChildWindowDpiMessage)GetProcAddress(GetModuleHandleW(L"user32.dll"), "EnableChildWindowDpiMessage");

	if (pEnableChildWindowDpiMessage) return pEnableChildWindowDpiMessage(hWnd, bEnable);
	
	return FALSE;
}

CGlobalSize::CGlobalSize()
{
	m_uLeftDockWidth = 285;
	m_uAlbumDockWidth = 215;

	m_uSmallIconSize = 44;
	m_uBigIconSize = 61;

	m_uHeadHeight = 24;
	m_uTabHeight = 24;
	m_uMenuItemHeight = 24;

	m_uPlayingItemHeight = 61;

	m_uTileHeight = 79;
	m_uIconSpace = 64;
	
	m_ColWidth_Name = 255;
	m_ColWidth_Time = 85;
	m_ColWidth_MediaTag_Title = 150;
	m_ColWidth_MediaTag_Artist = 90;
	m_ColWidth_MediaTag_Album = 120;

	m_ColWidth_Type = 45;
	m_ColWidth_FileSize = 58;
	m_ColWidth_RelatedPlaylist = 120;
	m_ColWidth_RelatedSingerAlbum = 175;
	m_ColWidth_AddTime = 82;
}

void CGlobalSize::init()
{
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

		for (auto p : { &m_ColWidth_Name, &m_ColWidth_MediaTag_Title, &m_ColWidth_Name
			, &m_ColWidth_MediaTag_Title, &m_ColWidth_MediaTag_Artist, &m_ColWidth_MediaTag_Album
			, &m_ColWidth_Type, &m_ColWidth_FileSize, &m_ColWidth_RelatedPlaylist
			, &m_ColWidth_AddTime, &m_ColWidth_RelatedSingerAlbum, &m_ColWidth_Time })
		{
			*p = uintRound(*p*fWRate);
		}
	}

	float fHRate = 1;
	if (m_uScreenHeight > 720)
	{
		fHRate = (float)MIN(1080, m_uScreenHeight) / 720;
	}

	m_uMenuItemHeight = uintRound(m_uMenuItemHeight*fHRate*fHRate);
	m_uTabHeight = uintRound(m_uTabHeight*fHRate*fHRate);
	m_uHeadHeight = uintRound(m_uHeadHeight*fHRate*fHRate);
	
	m_uPlayingItemHeight = uintRound(m_uPlayingItemHeight*fHRate);
	
	m_uTileHeight = uintRound(m_uTileHeight*fHRate);
	m_uIconSpace = uintRound(m_uIconSpace*fHRate);

	float fDPIRate = getDPIRate();
	m_fSmallFontSize = (fHRate - 1)*1.13f * fDPIRate * fDPIRate;

	m_fBigFontSize = m_fSmallFontSize * 1.77f;

	m_fMenuFontSize = (m_fBigFontSize + m_fSmallFontSize)/2.5f;
	m_fMenuBarFontSize = (m_fMenuFontSize + m_fSmallFontSize)/2.5f;

	m_uScreenWidth = uScreenWidth;
	m_uScreenHeight = uScreenHeight;
}
