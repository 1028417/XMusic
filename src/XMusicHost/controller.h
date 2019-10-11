#pragma once

#include "viewdef.h"

class CPlayerController : public IPlayerController
{
public:
	CPlayerController(IPlayerView& view, IModel& model)
		: m_view(view)
		, m_model(model)
	{
	}
	
protected:
	IPlayerView& m_view;

	IModel& m_model;

private:
    void _tryPlay();

public:
#if __windows
    bool setupRootDir(HWND hWndParent=NULL);
#endif

	bool start();

    void stop();

    void callPlayCtrl(const tagPlayCtrl& PlayCtrl) override;

	CMediaRes* attachDir(const wstring& strDir) override;

	bool renameMedia(IMedia& media, const wstring& strNewName) override;

	void moveMediaFile(const TD_IMediaList& lstMedias, const wstring& strDir) override;

	bool removeMediaRes(const TD_MediaResList& lstMediaRes) override;

	int addPlayItems(const list<wstring>& lstFiles, CPlaylist& Playlist) override;
	
	int addAlbumItems(const list<wstring>& lstFiles, CAlbum& Album) override;
	
	E_RenameRetCode renameMediaSet(CMediaSet& MediaSet, const wstring& strNewName) override;

	bool removeMediaSet(CMediaSet& MediaSet) override;

	bool autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias, const CB_AutoMatchProgress& cbProgress
		, const CB_AutoMatchConfirm& cbConfirm, map<CMedia*, wstring>& mapUpdatedMedia) override;

	UINT addInMedia(const list<wstring>& lstFiles, const CB_AddInMediaProgress& cbProgress, const CB_AddInConfirm& cbAddInConfirm) override;
};
