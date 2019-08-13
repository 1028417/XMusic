
#pragma once

#include "util.h"

#ifdef __MediaLibPrj
extern ITxtWriter& g_logWriter;

#define __MediaLibExt __dllexport
#else
#define __MediaLibExt __dllimport
#endif

#define __Space L' '

#define __CNDot L" · "

class IMedia;
using TD_IMediaList = PtrArray<IMedia>;

class CMedia;
using TD_MediaList = PtrArray<CMedia>;

class CMediaSet;
using TD_MediaSetList = PtrArray<CMediaSet>;

class CMediaRes;
using TD_MediaResList = PtrArray<CMediaRes>;

class CPlayItem;
using TD_PlayItemList = PtrArray<CPlayItem>;

class CAlbumItem;
using TD_AlbumItemList = PtrArray<CAlbumItem>;

enum class E_GlobalImage
{
	GI_Folder = 0
	, GI_FolderLink
	, GI_MediaFile
	, GI_WholeTrack

	, GI_Playlist
	, GI_PlayItem

	, GI_SingerGroup
	, GI_SingerDefault
	, GI_Album
	, GI_AlbumItem
};

enum class E_MediaSetType
{
	MST_Null = -1
	, MST_Playlist
	, MST_Album
	, MST_Singer
	, MST_SingerGroup
};


#define __MeidaTimeFormat L"%y-%m-%d\n %H:%M"

using mediatime_t = int32_t;

class __MediaLibExt CMediaTime
{
public:
	CMediaTime()
		: m_tAddTime((mediatime_t)time(0))
	{
	}

	CMediaTime(mediatime_t tAddTime)
		: m_tAddTime(tAddTime)
	{
	}

public:
	mediatime_t m_tAddTime;

public:
	bool operator <(CMediaTime time) const
	{
		return m_tAddTime < time.m_tAddTime;
	}

	wstring GetText() const
	{
		return tmutil::formatTime(__MeidaTimeFormat, m_tAddTime);
	}
};

enum class E_MediaSetChanged
{
	MSC_Rename
	, MSC_Remove

	, MSC_SingerImgChanged
};

struct tagMediaSetChanged
{
	tagMediaSetChanged(E_MediaSetType t_eMediaSetType, E_MediaSetChanged t_eChangedType, UINT t_uMediaSetID)
		: eMediaSetType(t_eMediaSetType)
		, eChangedType(t_eChangedType)
		, uMediaSetID(t_uMediaSetID)
	{
	}

	E_MediaSetType eMediaSetType;

	E_MediaSetChanged eChangedType;

	UINT uMediaSetID;

	wstring strNewName;

	UINT uSingerImgPos = 0;
};

struct tagRelatedMediaSet
{
	UINT uMediaSetID = 0;
	wstring strMediaSetName;

	UINT uMediaID = 0;

	void clear()
	{
		uMediaSetID = 0;
		strMediaSetName.clear();

		uMediaID = 0;
	}
};

enum class E_FindMediaMode
{
	FMM_MatchText = 0

	, FMM_MatchPath
	
	, FMM_MatchDir

	, FMM_MatchFiles

	, FMM_RenameDir
};

struct tagFindMediaPara
{
	tagFindMediaPara()
	{
	}

	tagFindMediaPara(E_FindMediaMode t_eFindMediaMode)
	{
		eFindMediaMode = t_eFindMediaMode;
	}

	tagFindMediaPara(E_FindMediaMode t_eFindMediaMode, const wstring& t_strFindText, bool t_bFindOne = false)
	{
		eFindMediaMode = t_eFindMediaMode;
		strFindText = wsutil::lowerCase_r(t_strFindText);

		bFindOne = t_bFindOne;
	}

	E_FindMediaMode eFindMediaMode = E_FindMediaMode::FMM_MatchText;
	wstring strFindText;

	bool bFindOne = false;

	wstring strFindSingerName;

	wstring strDir;

	wstring strRenameDir;

	std::set<wstring> setFiles;
};

enum class E_RenameRetCode
{
	RRC_Success = 0

	, RRC_Failure

	, RRC_InvalidName
	, RRC_NameExists
};

#include "IMedia.h"

#include "MediaRes.h"

#include "RootMediaRes.h"

#include "Media.h"

#include "MediaSet.h"

#include "PlaylistMedia.h"

#include "SingerMedia.h"

#include "MediaMixture.h"
