
#pragma once

#include "util/util.h"

#ifdef __MediaLibPrj
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
class CMediaDir;
using TD_MediaDirList = PtrArray<CMediaDir>;

class CPlayItem;
using TD_PlayItemList = PtrArray<CPlayItem>;

class CAlbumItem;
using TD_AlbumItemList = PtrArray<CAlbumItem>;

enum class E_GlobalImage
{
	GI_Dir = 0
	, GI_AttachDir
	, GI_File
	, GI_AttachFile
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
	MST_Playlist = 0
	, MST_Album
	, MST_Singer
	, MST_SingerGroup
    , MST_SnapshotMediaDir
    , MST_None
};

using mediatime_t = time32_t;

class __MediaLibExt CMediaTime
{
public:
	template <typename T>
	static wstring genFileTimeString(T time, bool bNewLine)
	{
		if (time <= 0)
		{
			return L"";
		}

		return tmutil::formatTime(bNewLine ? L"%y-%m-%d\n %H:%M" : L"%y-%m-%d %H:%M", time);
	}

public:
	CMediaTime() : m_time((mediatime_t)time(0))
	{
	}

    CMediaTime(mediatime_t tAddTime) : m_time(tAddTime)
	{
	}

public:
	mediatime_t m_time;

public:
	bool operator <(CMediaTime time) const
	{
		return m_time < time.m_time;
	}

	wstring GetText(bool bNewLine) const
	{
		return CMediaTime::genFileTimeString(m_time, bNewLine);
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
    tagMediaSetChanged(E_MediaSetType eMediaSetType, UINT uMediaSetID, E_MediaSetChanged eChangedType)
        : eMediaSetType(eMediaSetType)
        , uMediaSetID(uMediaSetID)
        , eChangedType(eChangedType)
	{
	}

	E_MediaSetType eMediaSetType;
	UINT uMediaSetID;

	E_MediaSetChanged eChangedType;

	wstring strNewName;

	//UINT uSingerImgPos = 0;
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
	tagFindMediaPara(E_FindMediaMode eFindMediaMode)
		: eFindMediaMode(eFindMediaMode)
	{
	}

    tagFindMediaPara(cwstr strText)
        : eFindMediaMode(E_FindMediaMode::FMM_MatchText)
        , strFind(strutil::lowerCase_r(strText))
    {
    }

    tagFindMediaPara(cwstr strPath, bool bDir, bool bFindOne=false)
        : eFindMediaMode(bDir ? E_FindMediaMode::FMM_MatchDir : E_FindMediaMode::FMM_MatchPath)
        , strFind(strPath)
        , bFindOne(bFindOne)
    {
        if (bDir)
        {
            strDir = strPath;
        }
        else
        {
            strDir = fsutil::GetParentDir(strFind);
        }
    }

    tagFindMediaPara(const std::set<wstring>& setFiles)
        : eFindMediaMode(E_FindMediaMode::FMM_MatchFiles)
        , setFiles(setFiles)
    {
    }

    tagFindMediaPara(cwstr strDir, cwstr strNewPath)
        : eFindMediaMode(E_FindMediaMode::FMM_RenameDir)
        , strDir(strDir)
        , strNewDirPath(strNewPath)
    {
    }

    E_FindMediaMode eFindMediaMode;
    wstring strFind;
    wstring strDir;

	bool bFindOne = false;

    wstring strMatchSingerName;

	std::set<wstring> setFiles;

    wstring strNewDirPath;
};

struct tagFindMediaResult
{
	TD_MediaList lstRetMedias;

	map<UINT, wstring> mapSingerDirChanged;

        class CSinger *pRetSinger = NULL;
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

#include "Media.h"

#include "MediaSet.h"

#include "MediaLib.h"

#include "PlaylistMedia.h"

#include "SingerMedia.h"

#if __winvc
#include "MediaMixture.h"
#endif
