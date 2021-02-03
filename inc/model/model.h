
#pragma once

#include "../MediaLib/MediaDef.h"

#ifdef __ModelPrj
#define __ModelExt __dllexport
#else
#define __ModelExt __dllimport
#endif

#include "XMediaLib.h"

#define __pkgName L"com.musicrossoft.xmusic"
#define __pkgDir L"/data/data/" __pkgName

struct tagAddPlayItem
{
    tagAddPlayItem() = default;

    tagAddPlayItem(cwstr strPath) : strPath(strPath)
    {
    }

    tagAddPlayItem(cwstr strPath, uint64_t uFileSize, UINT uDuration)
        : strPath(strPath)
        , uFileSize(uFileSize)
        , uDuration(uDuration)
    {
    }

    wstring strPath;
    uint64_t uFileSize = 0;
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
	, CDM_SimilarTitle
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
class CUpgradeUrl
{
public:
    CUpgradeUrl(const string& strBaseUrl, const string& strPostFix)
        : m_strBaseUrl(strBaseUrl)
        , m_strPostFix(strPostFix)
    {
    }

private:
    string m_strBaseUrl;
    string m_strPostFix;

public:
    string mdlconf() const;

    string mdl() const;

    string singerimg(const string& strFile) const;

    string app() const;
};

struct tagMdlConf
{
    string strAppVersion;

    UINT uMdlVersion = 0;

    UINT uCompatibleCode = 0;

    list<CUpgradeUrl> lstUpgradeUrl;

    //list<string> lstOnlineHBkg;
    //list<string> lstOnlineVBkg;
};

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
    UR_AppUpgraded,

    UR_InitMediaLibFail
};

struct tagOlBkg
{
    wstring catName;
    string accName;
    string prjName;
    string artiName;

    list<string> lstFiles;
};
#endif

#include "DataMgr.h"

#include "FileTitle.h"

#if __winvc
#include "AutoMatch.h"

#include "MediaMixer.h"

#include "BackupMgr.h"
#endif

#include "MediaOpaque.h"

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

	virtual bool renameMedia(IMedia& media, cwstr strNewName)
    {
        (void)media;
        (void)strNewName;
		return false;
    }

    virtual void onPlayingListUpdated(int nPlayingItem = -1, bool bSetActive = false) = 0;

    virtual void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) = 0;
    virtual void onPlayStop(bool bOpenSuccess, bool bPlayFinish) = 0;

    virtual UINT GetSingerImgPos(UINT uSingerID)
    {
        (void)uSingerID;
        return 0;
    }

    virtual void onSingerImgDownloaded(cwstr, const struct tagSingerImg&) {}

    virtual bool installApp(const CByteBuffer&) {return true;}
};

using CB_DeployArti = const function<bool(cwstr strTip, UINT uProgress, bool bFail)>&;

class IModel
{
public:
    virtual bool initMediaLib(bool bNotify = true) = 0;

    virtual CDataMgr& getDataMgr() = 0;

    virtual CPlaylistMgr& getPlaylistMgr() = 0;
    virtual CPlayMgr& getPlayMgr() = 0;

    virtual CSingerMgr& getSingerMgr() = 0;
    virtual CSingerImgMgr& getSingerImgMgr() = 0;

#if __winvc
    virtual CBackupMgr& getBackupMgr() = 0;

	virtual void convertXPkg(cwstr strFile) = 0;
    virtual void convertXmsc(cwstr strFile) = 0;

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

    using CB_ExportMedia = const function<bool(UINT uProgressOffset, cwstr strTip)>&;
	virtual UINT exportMedia(const tagExportOption& ExportOption, CB_ExportMedia cb) = 0;

	using CB_DeployMdl = const function<bool(cwstr strTip)>&;
	virtual bool deployMdl(bool bDeploySingerImg, CB_DeployMdl cb) = 0;

	virtual UINT deployXmsc(const TD_IMediaList& paMedias, CB_DeployArti cb) = 0;
	virtual UINT deployXmsc(CMediaDir& dir, CB_DeployArti cb) = 0;

    virtual wstring backupDB() = 0;
    virtual bool restoreDB(cwstr strTag) = 0;

    virtual bool clearData() = 0;

#else
    virtual E_UpgradeResult upgradeMdl(CByteBuffer& bbfConf, signal_t bRunSignal, UINT& uAppUpgradeProgress, wstring& strAppVersion) = 0;

    virtual const list<tagOlBkg>& getOlBkgList() const = 0;

    virtual void localScan(cwstr strDir, E_AttachDirType eType) = 0;
#endif

	virtual bool init(
#if !__winvc
		cwstr strWorkDir, cwstr strMedialibDir
#endif
	) = 0;

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
#else
    list<tagOlBkg> m_lstOlBkg;
#endif

	CSingerMgr m_SingerMgr;
	CSingerImgMgr m_SingerImgMgr;

	CPlaylistMgr m_PlaylistMgr;
	CPlayMgr m_PlayMgr;

public:
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

	bool initMediaLib(bool bNotify = true) override;

#if __winvc
	CBackupMgr& getBackupMgr() override
	{
		return m_BackupMgr;
	}

	void convertXPkg(cwstr strFile) override;
	void convertXmsc(cwstr strFile) override;

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

	UINT exportMedia(const tagExportOption& ExportOption, CB_ExportMedia cb) override;

	bool deployMdl(bool bDeploySingerImg, CB_DeployMdl cb) override;

	UINT deployXmsc(const TD_IMediaList& paMedias, CB_DeployArti cb) override;
	UINT deployXmsc(CMediaDir& dir, CB_DeployArti cb) override;

	wstring backupDB() override;
	bool restoreDB(cwstr strTag) override;

	bool clearData() override;

#else
	E_UpgradeResult upgradeMdl(CByteBuffer& bbfConf, signal_t bRunSignal, UINT& uAppUpgradeProgress, wstring& strAppVersion) override;

    const list<tagOlBkg>& getOlBkgList() const override
    {
        return m_lstOlBkg;
    }

	void localScan(cwstr strDir, E_AttachDirType eType) override;
#endif

	bool init(
#if !__winvc
		cwstr strWorkDir, cwstr strMedialibDir
#endif
	) override;

	void close() override;

private:
	bool _initMediaLib(cwstr strDBFile);

	bool _initData(cwstr strDBFile);

	inline cwstr _medialibPath()
	{
		return m_Option.strRootDir;
	}
	wstring _medialibPath(cwstr strSubPath);

#if __winvc
	wstring _mdlPath();
	wstring _mdlPath(cwstr strSubPath);

	wstring _deployPath();
	wstring _deployPath(cwstr strSubPath);

    bool _updateDir(cwstr strOldPath, cwstr strNewPath);
	
	bool _deployXmsc(CMediaDir& dir, CB_DeployArti cb, UINT& uCount);

	bool _deploySingerImg(CB_DeployMdl cb, list<tagSingerImg>& lstSingerImg);

	bool _deployBkg(cwstr strDstFile);

    void _clear();

#else
    bool _upgradeApp(const list<CUpgradeUrl>& lstUpgradeUrl, signal_t bRunSignal, UINT& uAppUpgradeProgress);

    E_UpgradeResult _loadMdl(const tagMdlConf& MdlConf, CByteBuffer& bbfMdl, bool bUpgradeDB);
#endif

    void _close();
};
