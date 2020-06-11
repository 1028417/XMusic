#pragma once

class CModelObserver : public IModelObserver
{
public:
	CModelObserver(class __view& view)
		: m_view(view)
	{
	}

private:
	__view& m_view;

private:
	void initView() override;
	void clearView() override;

	void refreshMedia() override;

	void renameMedia(IMedia& media, cwstr strNewName) override;

	void onPlayingListUpdated(int nPlayingItem = -1, bool bSetActive = false) override;

	void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;
	void onPlayStop(bool bRet) override;
	
	UINT GetSingerImgPos(UINT uSingerID) override;
};
