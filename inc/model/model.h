
#pragma once

#include "../MediaLib/MediaDef.h"

#ifdef __ModelPrj
#define __ModelExt __dllexport
#else
#define __ModelExt __dllimport
#endif

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

#if !__winvc
enum class E_UpgradeResult
{
	UR_Success,
	UR_Fail,

	UR_DownloadFail,
	UR_MedialibInvalid,
	UR_ReadMedialibFail,

    UR_MedialibUnmatch,
	UR_MedialibUncompatible,
	UR_AppUpgradeFail,
	UR_AppUpgraded
};
#endif

#include "XMediaLib.h"

#include "DataMgr.h"

#include "FileTitle.h"

#if __winvc
#include "AutoMatch.h"

#include "MediaMixer.h"

#include "BackupMgr.h"
#endif

#include "PlaylistMgr.h"

#include "PlayMgr.h"

#include "SingerMgr.h"

#include "SingerImgMgr.h"

class IModelObserver
{
public:
    virtual void initView() {}
    virtual void clearView() {}

    virtual void refreshMedia() {}

    virtual void renameMedia(IMedia& media, cwstr strNewName)
    {
        (void)media;
        (void)strNewName;
    }

    virtual void onPlayingListUpdated(int nPlayingItem = -1, bool bSetActive = false) = 0;

    virtual void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) = 0;
    virtual void onPlayStop(bool bRet) = 0;

    virtual UINT GetSingerImgPos(UINT uSingerID)
    {
        (void)uSingerID;
        return 0;
    }

    virtual void onSingerImgDownloaded() {}

    virtual bool installApp(const CByteBuffer&) {return true;}
};

class IModel
{
public:
    virtual bool initMediaLib() = 0;

    virtual CDataMgr& getDataMgr() = 0;

    virtual CPlaylistMgr& getPlaylistMgr() = 0;
    virtual CPlayMgr& getPlayMgr() = 0;

    virtual CSingerMgr& getSingerMgr() = 0;
    virtual CSingerImgMgr& getSingerImgMgr() = 0;

#if !__winvc
    virtual E_UpgradeResult upgradeMdl(cbyte_p lpMdlConf, size_t size, const bool&bRunSignal, UINT& uAppUpgradeProgress, wstring& strAppVersion) = 0;

#else
    virtual CBackupMgr& getBackupMgr() = 0;

    virtual void convertXmsc(const wstring& strFile) = 0;

    virtual bool setupMediaLib(cwstr strRootDir) = 0;

    virtual void refreshMediaLib() = 0;

    virtual CMediaDir* attachDir(cwstr strDir) = 0;
    virtual void detachDir(cwstr strDir) = 0;

    virtual bool renameMedia(cwstr strOldOppPath, cwstr strNewOppPath, bool bDir) = 0;

    virtual bool removeMedia(const TD_MediaList& lstMedias) = 0;

    virtual bool moveFiles(cwstr strDir, const SMap<wstring, wstring>& mapUpdateFiles) = 0;

    virtual bool removeFiles(const std::set<wstring>& setFiles) = 0;

    virtual bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) = 0;

    virtual bool updateFile(const map<wstring, wstring>& mapUpdateFiles) = 0;

    virtual void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
        , CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) = 0;

    virtual void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;
    virtual void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;

    using CB_exportorMedia = function<bool(UINT uProgressOffset, cwstr strDstFile)>;
    virtual UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) = 0;

    virtual wstring backupDB() = 0;
    virtual bool restoreDB(cwstr strTag) = 0;

    virtual bool clearData() = 0;
#endif

    virtual void close() = 0;
};

class __ModelExt CModel : public IModel
{
public:
    CModel(IModelObserver& ModelObserver, tagOption& Option);

private:
    IModelObserver& m_ModelObserver;
    tagOption& m_Option;

    XMediaLib m_MediaLib;

    CDataMgr m_DataMgr;

#if __winvc
    CBackupMgr m_BackupMgr;
#endif

    CPlaylistMgr m_PlaylistMgr;

    CSingerMgr m_SingerMgr;
    CSingerImgMgr m_SingerImgMgr;

    CPlayMgr m_PlayMgr;

public:
    wstring medialibPath(cwstr strSubPath = L"");

    CDataMgr& getDataMgr() override
    {
            return m_DataMgr;
    }

    CPlaylistMgr& getPlaylistMgr() override
    {
        return m_PlaylistMgr;
    }

    CPlayMgr& getPlayMgr() override
    {
        return m_PlayMgr;
    }

    CSingerMgr& getSingerMgr() override
    {
        return m_SingerMgr;
    }

    CSingerImgMgr& getSingerImgMgr() override
    {
        return m_SingerImgMgr;
    }

    bool initMediaLib() override;

#if __winvc
    CBackupMgr& getBackupMgr() override
    {
        return m_BackupMgr;
    }

    void convertXmsc(const wstring& strFile) override;

    bool setupMediaLib(cwstr strRootDir) override;

    void refreshMediaLib() override;

    CMediaDir* attachDir(cwstr strDir) override;
    void detachDir(cwstr strDir) override;

    bool renameMedia(cwstr strOldOppPath, cwstr strNewOppPath, bool bDir) override;

    bool removeMedia(const TD_MediaList& lstMedias) override;

    bool moveFiles(cwstr strDir, const SMap<wstring, wstring>& mapUpdateFiles) override;

    bool removeFiles(const std::set<wstring>& setFiles) override;

    bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) override;

    bool updateFile(const map<wstring, wstring>& mapUpdateFiles) override;

    void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
        , CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) override;

    void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;
    void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;

    UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) override;

    wstring backupDB() override;
    bool restoreDB(cwstr strTag) override;

    bool clearData() override;

#else
    E_UpgradeResult upgradeMdl(cbyte_p lpMdlConf, size_t size, const bool& bRunSignal, UINT& uAppUpgradeProgress, wstring& strAppVersion) override;
#endif

    void close() override;

private:
    bool _initData(cwstr strDBFile);

#if __winvc
    bool _updateDir(cwstr strOldPath, cwstr strNewPath);

    bool _exportDB(cwstr strExportDir, bool bExportXmsc);

    void _clear();

#else
    bool _upgradeApp(const bool& bRunSignal, UINT& uAppUpgradeProgress);
    E_UpgradeResult _loadMdl(CByteBuffer& bbfMdl, bool bUpgradeDB);
#endif

    void _close();
};
