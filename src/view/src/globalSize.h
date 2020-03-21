
#pragma once

#define __MaxTabTitle 20

class CGlobalSize
{
public:
	CGlobalSize();

	void init();

public:
	UINT m_uScreenWidth = 0;
	UINT m_uScreenHeight = 0;

	UINT m_uMenuBarHeight = 0;

	UINT m_uScrollbarWidth = 0;

	UINT m_uLeftDockWidth = 0;
	UINT m_uAlbumDockWidth = 0;
	
	UINT m_uSmallIconSize = 0;
	UINT m_uBigIconSize = 0;

	UINT m_uMenuItemHeight = 0;
	UINT m_uTabHeight = 0;
	UINT m_uHeadHeight = 0;

	UINT m_uTileHeight = 0;
	UINT m_uIconSpace = 0;

	float m_fSmallFontSize = 0;
	float m_fBigFontSize = 0;
	
	float m_fMidFontSize = 0;
	
	UINT m_ColWidth_Name = 0;
	UINT m_ColWidth_Time = 0;
	UINT m_ColWidth_MediaTag_Title = 0;
	UINT m_ColWidth_MediaTag_Artist = 0;
	UINT m_ColWidth_MediaTag_Album = 0;

	UINT m_ColWidth_Type = 0;
	UINT m_ColWidth_FileSize = 0;
	UINT m_ColWidth_RelatedPlaylist = 0;
	UINT m_ColWidth_RelatedSingerAlbum = 0;
	UINT m_ColWidth_AddTime = 0;
};
