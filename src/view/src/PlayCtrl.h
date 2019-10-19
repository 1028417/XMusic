#pragma once

#include "Player.h"
#include "PlaySpirit.h"

class CPlayCtrl
{
public:
	CPlayCtrl(class __view& view);
	
private:
	__view& m_view;

	CPlaySpirit m_PlaySpirit;
	
	wstring m_strPlayingFile;

public:
	inline CPlaySpirit& getPlaySpirit()
	{
		return m_PlaySpirit;
	}

	bool init();

	void onPlay(CPlayItem& PlayItem);
	void onPlayFinish();

	void handlePlaySpiritButtonClick(ST_PlaySpiritButton eButton, short para = 0);

	void setVolume(int offset);

	void seek(UINT uPos);
	
	void close();

	bool addPlayingItem(const TD_IMediaList& lstMedias, int iPos = -1);
	bool addPlayingItem(CMediaSet& MediaSet);

private:
	inline CPlayer& player();

	bool _initPlaySpirit(int iPosX, int iPosY, const wstring& strSkin, UINT uVolume);

	void _handlePlaySpiritEvent(E_PlaySpiritEvent eEvent, UINT uButton, short para);
};
