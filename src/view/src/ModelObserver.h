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

	void onPlayingListUpdated(int nPlayingItem = -1, bool bSetActive = false) override;

	void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) override;
	void onPlayStop(bool bCanceled, bool bOpenFail) override;
	
	UINT GetSingerImgPos(UINT uSingerID) override;

	bool renameMedia(IMedia& media, const wstring& strNewName) override;
};
