
#pragma once

#include "../MediaLib/MediaDef.h"

#ifdef __ModelPrj
extern ITxtWriter& m_modelLogger;

#define __ModelExt __dllexport
#else
#define __ModelExt __dllimport
#endif

#define __medialibPath L"/" __medialibDir L"/"
#define __medialibFile L"medialib"

#include "Dao.h"

#include "OptionMgr.h"

#include "DataMgr.h"

#if __winvc
#include "BackupMgr.h"

#include "AutoMatch.h"

#include "MediaMixer.h"
#endif

#include "PlaylistMgr.h"

#include "PlayMgr.h"

#include "SingerMgr.h"

#include "SingerImgMgr.h"

#include "XMediaLib.h"

class IModelObserver
{
public:
	virtual void initView() {}
	virtual void clearView() {}

	virtual void refreshMedia() {}

    virtual void onPlayingListUpdated(int nPlayingItem = -1, bool bSetActive = false) = 0;

	virtual void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) = 0;
	virtual void onPlayStoped(E_DecodeStatus decodeStatus) = 0;
	
	virtual UINT GetSingerImgPos(UINT uSingerID)
	{
		(void)uSingerID;
		return 0;
	}

    virtual void onSingerImgDownloaded(const wstring& strFile)
    {
        (void)strFile;
    }

	virtual bool renameMedia(IMedia& media, const wstring& strNewName)
	{
		(void)media;
		(void)strNewName;
		return false;
	}
};

#if !__winvc
struct __ModelExt tagUpgradeConf
{
    UINT uVersion = 0;

    PairList<string, string> plUrl;
};

#else
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

	ArrList<CCueFile> alCueFiles;
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
#endif

class IModel
{
public:
    virtual bool initMediaLib() = 0;

    virtual bool status() const = 0;


    virtual CDataMgr& getDataMgr() = 0;

#if __winvc
    virtual CBackupMgr& getBackupMgr() = 0;
#endif

	virtual XMediaLib& getMediaLib() = 0;
	
	virtual CPlaylistMgr& getPlaylistMgr() = 0;
	virtual CPlayMgr& getPlayMgr() = 0;
	
	virtual CSingerMgr& getSingerMgr() = 0;
	virtual CSingerImgMgr& getSingerImgMgr() = 0;

	virtual bool setupMediaLib(const wstring& strRootDir) = 0;
	virtual void refreshMediaLib() = 0;
		
	virtual CMediaDir* attachDir(const wstring& strDir) = 0;
	virtual void detachDir(const wstring& strDir) = 0;

	virtual bool renameMedia(const wstring& strOldOppPath, const wstring& strNewOppPath, bool bDir) = 0;

	virtual bool removeMedia(const TD_MediaList& lstMedias) = 0;

	virtual bool moveFiles(const wstring& strDir, const SMap<wstring, wstring>& mapUpdateFiles) = 0;

	virtual bool removeFiles(const std::set<wstring>& setFiles) = 0;

	virtual bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) = 0;

	virtual bool updateFile(const map<wstring, wstring>& mapUpdateFiles) = 0;

    virtual bool clearData() = 0;

    virtual void close() = 0;

#if !__winvc
    virtual bool readUpgradeConf(tagUpgradeConf& upgradeConf, Instream *pins = NULL) = 0;
    virtual bool upgradeMediaLib(const tagUpgradeConf& upgradeConf, CB_DownloadProgress& cbProgress) = 0;

#else
    virtual void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
        , CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) = 0;

    virtual void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;
    virtual void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;

    using CB_exportorMedia = function<bool(UINT uProgressOffset, const wstring& strDstFile)>;
    virtual UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) = 0;

	virtual wstring backupDB() = 0;
	virtual bool restoreDB(const wstring& strTag) = 0;
#endif
};

class __ModelExt CModel : public IModel
{
public:
	CModel(IModelObserver& ModelObserver, tagOption& Option);

private:
	IModelObserver& m_ModelObserver;
	tagOption& m_Option;

	CSQLiteDB m_db;
	CDao m_dao;
	
	CDataMgr m_DataMgr;

#if __winvc
	CBackupMgr m_BackupMgr;
#endif

	XMediaLib m_MediaLib;

	CPlaylistMgr m_PlaylistMgr;

	CSingerMgr m_SingerMgr;
	CSingerImgMgr m_SingerImgMgr;

	CPlayMgr m_PlayMgr;

	string m_strSingerImgUrl;

public:
        bool status() const override
	{
		return m_db.GetStatus();
	}

        CDataMgr& getDataMgr() override
        {
                return m_DataMgr;
        }

#if __winvc
		CBackupMgr& getBackupMgr() override
		{
			return m_BackupMgr;
		}
#endif

	XMediaLib& getMediaLib() override
	{
		return m_MediaLib;
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

	bool setupMediaLib(const wstring& strRootDir) override;
	void refreshMediaLib() override;

	CMediaDir* attachDir(const wstring& strDir) override;
	void detachDir(const wstring& strDir) override;

	bool renameMedia(const wstring& strOldOppPath, const wstring& strNewOppPath, bool bDir) override;

	bool removeMedia(const TD_MediaList& lstMedias) override;

	bool moveFiles(const wstring& strDir, const SMap<wstring, wstring>& mapUpdateFiles) override;

	bool removeFiles(const std::set<wstring>& setFiles) override;

	bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) override;
	
	bool updateFile(const map<wstring, wstring>& mapUpdateFiles) override;

	bool clearData() override;

	void close() override;

#if !__winvc
    bool readUpgradeConf(tagUpgradeConf& upgradeConf, Instream *pins = NULL) override;
    bool upgradeMediaLib(const tagUpgradeConf& upgradeConf, CB_DownloadProgress& cbProgress) override;

    static string genUrl(const string& strUrl, const string& strFileTitle);

#else
    void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
        , CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) override;

    void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;
    void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;

    UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) override;

    wstring backupDB() override;
	bool restoreDB(const wstring& strTag) override;

private:
    bool _exportDB(const wstring& strExportDir, bool bExportXmsc);
#endif

private:
    bool _upgradeMediaLib(CZipFile& zipFile, UINT uPrevVersion);
    bool _loadShareLib(CZipFile& zipFile);

    wstring _scanXMusicDir();

    bool _initData(const wstring& strDBFile);

	inline void _refreshMediaLib()
    {
        m_MediaLib.setDir(m_Option.strRootDir, m_Option.plAttachDir);
	}

	bool _updateDir(const wstring& strOldPath, const wstring& strNewPath);

	void _clear();

	void _close();
};
