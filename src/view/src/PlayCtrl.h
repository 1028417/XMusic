#pragma once

#include "Player.h"
#include "PlaySpirit.h"

class CPlayCtrl
{
public:
	CPlayCtrl(class __view& view);
	
private:
	__view& m_view;

	wstring m_strPlayingFile;

public:
	BOOL showPlaySpirit();

	void onPlay(CPlayItem& PlayItem);
	void onPlayFinish(bool bOpenFail);

	void handlePlaySpiritButtonClick(ST_PlaySpiritButton eButton, short para = 0);

	void setVolume(int offset);

	void seek(UINT uPos);
	
	void close();

	bool addPlayingItem(const SArray<wstring>& arrOppPaths, int nPos = -1);
	bool addPlayingItem(const wstring& strOppPaths, int nPos = -1)
	{
		return addPlayingItem(SArray<wstring>(strOppPaths), nPos);
	}
	bool addPlayingItem(const TD_IMediaList& lstMedias, int nPos = -1);
	
	bool addPlayingItem(CMediaSet& MediaSet);

private:
	inline CPlayer& player();

	void _handlePlaySpiritEvent(E_PlaySpiritEvent eEvent, UINT uButton, short para);
};
