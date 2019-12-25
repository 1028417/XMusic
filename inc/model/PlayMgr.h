#pragma once

#include "MediaOpaque.h"

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

enum class E_PlayStatus
{
	PS_Stop
	, PS_Play
	, PS_Pause
};

class __ModelExt CPlayMgr
{
public:
    CPlayMgr(class CModel& model, class IModelObserver& ModelObserver, tagOption& Option);
	
private:
    class CModel& m_model;

	IModelObserver& m_ModelObserver;

	tagOption& m_Option;

	CPlaylistMgr& m_PlaylistMgr;

    CPlaylist& m_Playinglist;
	
	UINT& m_uPlayingItem;
	
    set<UINT> m_setPlayedIDs;

    E_DemandMode m_eDemandMode = E_DemandMode::DM_Null;

	CMediaOpaque m_MediaOpaque;
	CPlayer m_Player;

private:
    void _refresh();

    template <class T>
    bool _insert(const T& t, bool bPlay, int nPos = -1);

    bool _clear();

    bool _playNext(bool bNext, bool bManual);

    BOOL _play(CPlayItem& PlayItem);

	bool _play(uint64_t uStartPos);

    void _tryPause(const wstring& strCheckPath, cfn_void_t<wstring&> cb);

	bool _checkDemandableSinger(CSinger& singer, E_LanguageType eLanguageType);

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

    bool init();

    const CMediaOpaque& mediaOpaque() const
    {
        return m_MediaOpaque;
    }

    CPlayer& player()
    {
        return m_Player;
	}

	E_PlayStatus GetPlayStatus();

	void SetPlayStatus(E_PlayStatus ePlayStatus);
	
	bool clear();

        bool insert(const SArray<wstring>& arrOppPaths, bool bPlay, int nPos = -1);
        bool insert(const TD_IMediaList& lstMedias, bool bPlay, int nPos = -1);

	bool assign(const SArray<wstring>& arrOppPaths);
	bool assign(const TD_IMediaList& lstMedias);

	int move(const TD_PlayItemList& lstPlayItems, UINT uPos);
	
	bool remove(const TD_PlayItemList& lstPlayItems);
	bool remove(const set<wstring>& setFiles);

	bool checkPlayedID(UINT uID);

    bool playAlarm(const wstring& strFile);

	bool play(UINT uItem, bool bManual=true);

	bool playNext(bool bManual=true);
	void playPrev();
	
	void renameFile(bool bDir, const wstring& strPath, const wstring& strNewPath, cfn_bool cb);
	void moveFile(const wstring& strPath, const wstring& strNewPath, cfn_bool cb);

    int demand(E_DemandMode eDemandMode, E_LanguageType eLanguageType = E_LanguageType::LT_None);
    int demand(const TD_MediaList& lstMedias, UINT uDemandCount = __DemandCount);
};
