﻿
#pragma once

#include "modelDef.h"

#if !__winvc
#define __OnlineMediaLib 1

enum class E_UpgradeResult
{
	UR_Success,
	UR_Fail,

	UR_DownloadFail,
	UR_MedialibInvalid,
	UR_ReadMedialibFail,

	UR_MedialibUncompatible,
	UR_AppUpgradeFail,
	UR_AppUpgraded
};

class __ModelExt CUpgradeUrl
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

struct __ModelExt tagMdlConf
{
    string strAppVersion;

    UINT uMdlVersion = 0;

    UINT uCompatibleCode = 0;

    list<CUpgradeUrl> lstUpgradeUrl;

    //list<string> lstOnlineHBkg;
    //list<string> lstOnlineVBkg;

    void clear()
    {
        strAppVersion.clear();

        uMdlVersion = 0;

        uCompatibleCode = 0;

        lstUpgradeUrl.clear();

        //lstOnlineHBkg.clear();
        //lstOnlineVBkg.clear();
    }
};
#endif

#define __medialibDir L".xmusic"
#define __DBFile L"/medialib"

#include "modelDef.h"

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
    virtual void onPlayStop(bool bCanceled, bool bOpenFail) = 0;
	
	virtual UINT GetSingerImgPos(UINT uSingerID)
	{
		(void)uSingerID;
		return 0;
	}

    virtual void onSingerImgDownloaded() {}
};

class IModel
{
public:
#if __OnlineMediaLib
    virtual bool loadMdlConf(cbyte_p lpData, size_t size, tagMdlConf& mdlConf) = 0;
    virtual E_UpgradeResult upgradeMdl(const tagMdlConf& orgMdlConf, const bool&bRunSignal, UINT& uAppUpgradeProgress) = 0;
#endif

    virtual bool initMediaLib() = 0;
	
    virtual CDataMgr& getDataMgr() = 0;

    virtual CPlaylistMgr& getPlaylistMgr() = 0;
    virtual CPlayMgr& getPlayMgr() = 0;

    virtual CSingerMgr& getSingerMgr() = 0;
    virtual CSingerImgMgr& getSingerImgMgr() = 0;

#if __winvc
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

#if __OnlineMediaLib
	tagMdlConf m_newMdlConf;
#endif

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

#if __OnlineMediaLib
    bool loadMdlConf(cbyte_p lpData, size_t size, tagMdlConf& mdlConf) override;
    E_UpgradeResult upgradeMdl(const tagMdlConf& orgMdlConf, const bool& bRunSignal, UINT& uAppUpgradeProgress) override;
#endif

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
#endif

    void close() override;

private:    
#if __OnlineMediaLib
    bool _upgradeApp(const list<CUpgradeUrl>& lstUpgradeUrl, const bool& bRunSignal, UINT& uAppUpgradeProgress);
    E_UpgradeResult _loadMdl(CByteBuffer& bbfMdl, bool bUpgradeDB);
#endif

	bool _initData(cwstr strDBFile);

	bool _updateDir(cwstr strOldPath, cwstr strNewPath);

#if __winvc
	wstring _scanXMusicDir();
	wstring _scanXMusicDir(PairList<wstring, E_AttachDirType>& plAttachDir);

	bool _exportDB(cwstr strExportDir, bool bExportXmsc);

	void _clear();
#endif

	void _close();
};
