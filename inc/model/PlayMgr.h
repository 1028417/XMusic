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

	CPlaylistMgr& m_PlaylistMgr;
    CPlaylist& m_Playinglist;

	tagOption& m_Option;
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

	bool _play(uint64_t uStartPos = 0);

    void _tryPause(cwstr strCheckPath, cfn_void_t<wstring&> cb);

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

    const ArrList<CPlayItem>& playingItems() const
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

    E_PlayStatus playStatus() const;

	void SetPlayStatus(E_PlayStatus ePlayStatus);
	
	bool clear();

        bool insert(const SArray<wstring>& arrOppPaths, bool bPlay, int nPos = -1);
        bool insert(const TD_IMediaList& lstMedias, bool bPlay, int nPos = -1);

	bool assign(const SArray<wstring>& arrOppPaths);
	bool assign(const TD_IMediaList& lstMedias);

	int move(const TD_PlayItemList& lstPlayItems, UINT uPos);
	
    bool remove(const ConstPtrArray<CPlayItem>& paPlayItems);
	bool remove(const set<wstring>& setFiles);

	bool checkPlayedID(UINT uID);

    bool play(cwstr strFile, uint64_t uStartPos = 0);

	bool play(UINT uItem, bool bManual=true);

	bool playNext(bool bManual=true);
	void playPrev();
	
	void renameFile(bool bDir, cwstr strPath, cwstr strNewPath, cfn_bool cb);
	void moveFile(cwstr strPath, cwstr strNewPath, cfn_bool cb);

	void replay();
	
    int demand(E_DemandMode eDemandMode, E_LanguageType eLanguageType = E_LanguageType::LT_None);
    int demand(const TD_MediaList& lstMedias, UINT uDemandCount = __DemandCount);

	void tryPlay();
};
