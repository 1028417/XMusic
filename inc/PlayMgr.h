#pragma once

#include "Player.h"

#define __DemandCount 20

enum class E_DemandMode
{
    DM_Null = 0,
    DM_DemandSinger,
    DM_DemandAlbum,
    DM_DemandAlbumItem,
    DM_DemandPlayItem,
    DM_DemandPlaylist
};

class __ModelExt CFileOpaqueEx : public CFileOpaque
{
public:
	static UINT checkDuration(const wstring& strFile, bool bLock = true)
	{
		CFileOpaqueEx FileOpaque(strFile);
		return FileOpaque.checkDuration(bLock);
	}

	static UINT checkDuration(IMedia& media, bool bLock = true)
	{
		return checkDuration(media.GetAbsPath(), bLock);
	}

public:
	CFileOpaqueEx(const wstring& strFile)
		: CFileOpaque(strFile)
	{
	}

	UINT checkDuration(bool bLock = true)
	{
		return CFileOpaque::checkDuration(bLock);
	}

private:
	void *m_pXmsc = NULL;

private:
	int64_t open() override;

	void close() override;

	size_t read(uint8_t *buf, int buf_size) override;
};

class __ModelExt CPlayMgr
{
public:
    CPlayMgr(class CModel& model, class IModelObserver& ModelObserver);
	
private:
    class CModel& m_model;

	IModelObserver& m_ModelObserver;

	CPlaylistMgr& m_PlaylistMgr;

    CPlaylist& m_Playinglist;
	
	UINT& m_uPlayingItem;
	
    CPlayer m_Player;

	set<UINT> m_setPlayedIDs;

	wstring m_strPlayingFile;
	
    CFileOpaqueEx m_FileOpaque;

    E_DemandMode m_eDemandMode = E_DemandMode::DM_Null;

private:
	bool _playFile(const wstring& strFile, uint64_t uStartPos = 0);

	bool _playNext(bool bNext, bool bManual);

	void _refresh();

	bool _clear();

	void _tryPause(const wstring& strCheckPath, const function<wstring(const wstring&)>& cb);

	void _genDemandableSinger(TD_MediaSetList& arrMediaSets, E_LanguageType eLanguageType);
	void _genDemandableAlbum(TD_MediaSetList& arrMediaSets, E_LanguageType eLanguageType);
	void _genDemandablePlaylist(TD_MediaSetList& arrMediaSets, E_LanguageType eLanguageType);

	int _demandAlbumItem(E_LanguageType eLanguageType);
	int _demandPlayItem(E_LanguageType eLanguageType);
	int _demandMediaSet(TD_MediaSetList& arrMediaSets);

public:
    E_DemandMode demandMode() const
    {
        return m_eDemandMode;
    }

    CMediaSet& getPlayinglist()
	{
        return m_Playinglist;
	}

	ArrList<CPlayItem>& getPlayingItems()
	{
        return m_Playinglist.playItems();
	}

	const wstring& GetPlayingFile() const
	{
		return m_strPlayingFile;
	}

	bool init();

    CPlayer& getPlayer()
	{
        return m_Player;
	}

	E_PlayStatus GetPlayStatus();

	void SetPlayStatus(E_PlayStatus ePlayStatus);
	
	bool clear();

	bool insert(const TD_IMediaList& lstMedias, bool bPlay, int iPos = -1);

    bool assign(const TD_IMediaList& lstMedias);

	int move(const TD_PlayItemList& lstPlayItems, UINT uPos);
	
	bool remove(const TD_PlayItemList& lstPlayItems);
	bool remove(const set<wstring>& setFiles);

	bool checkPlayedID(UINT uID);

	bool playAlarm(const wstring& strFile);

	bool play(UINT uItem, bool bManual=true);

	bool playNext(bool bManual=true);
	void playPrev();
	
	void renameFile(bool bDir, const wstring& strPath, const wstring& strNewPath, const function<bool()>& cb);
	void moveFile(const wstring& strPath, const wstring& strNewPath, const function<bool()>& cb);

    int demand(E_DemandMode eDemandMode, E_LanguageType eLanguageType = E_LanguageType::LT_None);
    int demand(const TD_MediaList& lstMedias, UINT uDemandCount = __DemandCount);
};
