#pragma once

#define ID_PLAYLIST_NULL 0

class __ModelExt CPlaylistMgr : public CMediaSet
{
public:
    CPlaylistMgr(CMediaSet& RootMediaSet, class IModelObserver& ModelObserver);

private:
    class IModelObserver& m_ModelObserver;

	CPlaylist m_Playinglist;
	list<CPlaylist> m_lstPlaylists;

private:
	int _indexOf(const CMediaSet&) const override;

    BOOL _AddPlayItems(const list<struct tagAddPlayItem>& lstPlayItem, CPlaylist& Playlist, int nPos);

public:
	const CPlaylist& playinglist() const
	{
		return m_Playinglist;
	}

	const list<CPlaylist>& playlists() const
	{
		return m_lstPlaylists;
	}

    void clear()
    {
        m_Playinglist.clear();
        m_lstPlaylists.clear();
    }

	BOOL Init();

    void GetSubSets(TD_MediaSetList& lstSubSets) override;

	CPlaylist *AddPlaylist(UINT uPos);

	BOOL RemovePlaylist(UINT uID);

	CPlaylist *RepositPlaylist(UINT uID, UINT uNewPos);

	BOOL AddPlayItems(const SArray<wstring>& arrOppPaths, CPlaylist& Playlist, int nPos);
	BOOL AddPlayItems(const TD_IMediaList& lstMedias, CPlaylist& Playlist, int nPos);

    BOOL RemovePlayItems(const ConstPtrArray<CPlayItem>& paPlayItems);
	BOOL RemoveAllPlayItems(CPlaylist& Playlist);

	int RepositPlayItem(CPlaylist& Playlist, const TD_IMediaList& lstMedias, UINT uTargetPos);

    BOOL SetBackPlayItems(CPlaylist& Playlist, const ConstPtrArray<CPlayItem>& paPlayItems);
};
