#pragma once

#define ID_PLAYLIST_NULL 0

class __ModelExt CPlaylistMgr : public CMediaSet
{
public:
	CPlaylistMgr(CMediaSet& RootMediaSet, CDao& dao, class IModelObserver& ModelObserver)
        : CMediaSet(L"列表库", &RootMediaSet)
		, m_dao(dao)
		, m_ModelObserver(ModelObserver)
		, m_Playinglist(*this, ID_PLAYLIST_NULL, L"")
	{
	}

private:
	CDao& m_dao;
	
	class IModelObserver& m_ModelObserver;

	CPlaylist m_Playinglist;
	list<CPlaylist> m_lstPlaylists;

public:
	CPlaylist& playinglist()
	{
		return m_Playinglist;
	}
	const CPlaylist& playinglist() const
	{
		return m_Playinglist;
	}

	list<CPlaylist>& playlists()
	{
		return m_lstPlaylists;
	}
	const list<CPlaylist>& playlists() const
	{
		return m_lstPlaylists;
	}

	BOOL Init();

	void GetSubSets(TD_MediaSetList& lstSubSets) override;

	CPlaylist *AddPlaylist(UINT uPos);

	BOOL RemovePlaylist(UINT uID);

	CPlaylist *RepositPlaylist(UINT uID, UINT uNewPos);

	BOOL AddPlayItems(const SArray<wstring>& lstOppPaths, CPlaylist& Playlist, int nPos);
	BOOL AddPlayItems(const TD_IMediaList& lstMedias, CPlaylist& Playlist, int nPos);

	BOOL RemovePlayItems(const TD_PlayItemList& arrPlayItems);
	BOOL RemoveAllPlayItems(CPlaylist& Playlist);

	int RepositPlayItem(CPlaylist& Playlist, const TD_IMediaList& lstMedias, UINT uTargetPos);

	BOOL SetBackPlayItems(CPlaylist& Playlist, const TD_MediaList& lstPlayItems);
};
