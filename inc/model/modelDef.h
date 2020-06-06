
#pragma once

#include "../MediaLib/MediaDef.h"

#ifdef __ModelPrj
extern ITxtWriter& g_modelLogger;

#define __ModelExt __dllexport
#else
#define __ModelExt __dllimport
#endif

using dbtime_t = int32_t;

struct tagAddPlayItem
{
    tagAddPlayItem() = default;

    tagAddPlayItem(cwstr strPath) : strPath(strPath)
        {
        }

    tagAddPlayItem(cwstr strPath, long long nFileSize, UINT uDuration)
        : strPath(strPath)
        , nFileSize(nFileSize)
        , uDuration(uDuration)
        {
        }

        wstring strPath;
        long long nFileSize = 0;
        UINT uDuration = 0;
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
    tagCompareBackupResult() = default;

    tagCompareBackupResult(const wstring strSrcTag, const wstring strDstTag=L"")
        : strSrcTag(strSrcTag)
        , strDstTag(strDstTag)
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

struct __ModelExt tagExportMedia
{
	wstring strDstDir;

	TD_IMediaList paMedias;

	list<wstring> lstCueFiles;
};

struct __ModelExt tagExportOption
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

struct __ModelExt tagPlaySpiritOption
{
    wstring strSkinName;

    int nPosX = 10000;
    int nPosY = 10000;

    UINT uVolume = 100;
};

enum E_TimerOperate
{
    TO_Null = 0
    , TO_StopPlay
    , TO_Shutdown
    , TO_Hibernal
};

struct __ModelExt tagTimerOperateOpt
{
    tagTimerOperateOpt()
    {
        tmutil::getCurrentTime(nHour, nMinute);
    }

    E_TimerOperate eTimerOperate = TO_Null;

    int nHour = 0;
    int nMinute = 0;
};
#endif

struct __ModelExt tagOption
{
    wstring strRootDir;
    PairList<wstring, E_AttachDirType> plAttachDir;

    UINT uPlayingItem = 0;

    bool bRandomPlay = false;
    bool bForce48KHz = false;

    bool bFullScreen = false;

#if __winvc
    bool bHideMenuBar = false;

    tagPlaySpiritOption PlaySpiritOption;

    int nAlarmHour = 0;
    int nAlarmMinute = 0;

    tagTimerOperateOpt TimerOption;

#else
    bool bUseBkgColor = false;
    int64_t crBkg = -1;
    int64_t crFore = -1;

    wstring strHBkg;
    wstring strVBkg;
    wstring strAddBkgDir;

    bool bNetworkWarn = true;
#endif
};
