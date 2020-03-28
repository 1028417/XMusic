
#pragma once

#include "../MediaLib/MediaDef.h"

using dbtime_t = int32_t;

struct tagPlayItemInfo
{
	int nID = 0;
	int nPlaylistID = 0;
	wstring strPath;
	dbtime_t tTime = 0;

	int fileSize = 0;
	int duration = 0;
};

struct tagPlaylistInfo
{
	int nID = 0;
	wstring strName;

	UINT language = 0;
    bool disableDemand = false;
    bool disableExport = false;

	SArray<tagPlayItemInfo> arrPlayItemInfo;
};

struct tagAlbumItemInfo
{
	int nID = 0;
	wstring strPath;
	dbtime_t tTime = 0;

	long long fileSize = 0;
	int duration = 0;
};

struct tagAlbumInfo
{
	int nID = 0;
	wstring strName;

	UINT language = 0;
    bool disableDemand = false;
    bool disableExport = false;

	SArray<tagAlbumItemInfo> arrAlbumItemInfo;
};

struct tagSingerInfo
{
	int nID = 0;
	wstring strName;
	wstring strPath;
	int nPos = 0;

    UINT language = 0;
    bool disableDemand = false;
    bool disableExport = false;

	SArray<tagAlbumInfo> arrAlbumInfo;
};

struct tagSingerGroupInfo
{
	int nID = 0;
	wstring strName;

	UINT language = 0;
    bool disableDemand = false;
    bool disableExport = false;

	SArray<tagSingerInfo> arrSingerInfo;
};

struct tagDiffMedia
{
	wstring strPath;

	UINT uMediaSetID;
	wstring strMediaSetName;

	UINT uSingerID = 0;
	wstring strSingerName;
	wstring strSingerDir;
};

struct tagModifyedMedia : tagDiffMedia
{
	wstring strNewPath;
	wstring strModifyedPath;
};

struct tagMovedMedia : tagDiffMedia
{
	wstring strNewMediaSetName;
};

struct tagCompareBackupResult
{
	tagCompareBackupResult() {}

	tagCompareBackupResult(const wstring t_strSrcTag, const wstring t_strDstTag=L"")
		: strSrcTag(t_strSrcTag)
		, strDstTag(t_strDstTag)
	{
	}

	wstring strSrcTag;
	wstring strDstTag;

	SArray<tagDiffMedia> arrNewPlayItem;
	SArray<tagDiffMedia> arrNewAlbumItem;

	SArray<tagDiffMedia> arrDeletedPlayItem;
	SArray<tagDiffMedia> arrDeletedAlbumItem;

	SArray<tagModifyedMedia> arrModifyedMedia;

	SArray<tagMovedMedia> arrMovedMedia;
};

struct tagAddPlayItem
{
	tagAddPlayItem() {}

	tagAddPlayItem(const wstring& t_strPath) : strPath(t_strPath)
	{
	}

	tagAddPlayItem(const wstring& t_strPath, long long t_nFileSize, UINT t_uDuration)
		: strPath(t_strPath)
		, nFileSize(t_nFileSize)
		, uDuration(t_uDuration)
	{
	}

	wstring strPath;
	long long nFileSize = 0;
	UINT uDuration = 0;
};

#if __winvc
enum class E_CheckDuplicateMode
{
    CDM_SamePath
    , CDM_SameName
    , CDM_SameTitle
};

using CB_checkDuplicateMedia = cfn_bool_t<CMedia&>;

using CB_checkSimilarFile = cfn_bool_t<CMediaRes&>;

using TD_SimilarFileGroup = SArray<pair<CMediaRes*, UINT>>;
using TD_SimilarFile = SArray<TD_SimilarFileGroup>;

struct tagExportMedia
{
	wstring strDstDir;

	TD_IMediaList paMedias;

	list<wstring> lstCueFiles;
};

struct tagExportOption
{
	bool bActualMode = false;

	bool bExportXmsc = false;

	bool bCompareFileSize = false;
	bool bCompareFileTime = false;

	bool bDeleteOther = true;

	bool bExportDB = false;

	wstring strExportPath;

	list<tagExportMedia> lstExportMedias;
};
#endif
