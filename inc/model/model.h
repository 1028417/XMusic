
#pragma once

#include "util.h"

#ifdef __ModelPrj
extern ITxtWriter& g_logger;

#define __ModelExt __dllexport
#else
#define __ModelExt __dllimport
#endif

#include "MediaLib/MediaDef.h"

#include "Dao.h"

#include "OptionMgr.h"

#include "DataMgr.h"

#include "BackupMgr.h"

#include "PlaylistMgr.h"

#include "PlayMgr.h"

#include "SingerMgr.h"

#include "SingerImgMgr.h"

#include "AutoMatch.h"

#include "MediaMixer.h"

class XMediaLib : public CMediaLib, public CMediaSet
{
public:
	XMediaLib(IMediaObserver& MediaObserver, CPlaylistMgr& PlaylistMgr, CSingerMgr& SingerMgr)
		: CMediaLib(MediaObserver)
		, CMediaSet(L"曲库")
		, m_PlaylistMgr(PlaylistMgr)
		, m_SingerMgr(SingerMgr)
	{
	}

private:
	CPlaylistMgr& m_PlaylistMgr;
	CSingerMgr& m_SingerMgr;

	virtual void GetSubSets(TD_MediaSetList& lstSubSets)
	{
		lstSubSets.add(m_PlaylistMgr);
		lstSubSets.add(m_SingerMgr);
	}
};

class IModelObserver
{
public:
	virtual void initView() {}
	virtual void clearView() {}

	virtual void refreshMedia() {}

	virtual void refreshPlayingList(int nPlayingItem = -1, bool bSetActive = false) = 0;

	virtual void onPlay(UINT uPlayingItem, CPlayItem& PlayItem, bool bManual) = 0;
	virtual void onPlayFinish() = 0;
	
	virtual UINT GetSingerImgPos(UINT uSingerID)
	{
		(void)uSingerID;
		return 0;
	}

	virtual bool renameMedia(IMedia& media, const wstring& strNewName)
	{
		(void)media;
		(void)strNewName;
		return false;
	}
};

enum class E_CheckDuplicateMode
{
	CDM_SamePath
	, CDM_SameName
	, CDM_SameTitle
};

struct tagExportOption
{
	bool bActualMode = false;

	bool bExportXMSC = false;

	bool bCompareFileSize = false;
	bool bCompareFileTime = false;

	bool bDeleteOther = true;

	bool bExportDB = false;
};

struct tagExportMedia : tagExportOption
{
	wstring strExportPath;

	PairList<wstring, TD_IMediaList> plMedias;
};

using CB_checkDuplicateMedia = const function<bool(CMedia&)>&;

using CB_checkSimilarFile = const function<bool(CMediaRes&)>&;

using TD_SimilarFileGroup = SArray<pair<CMediaRes*, UINT>>;

using TD_SimilarFile = SArray<TD_SimilarFileGroup>;

class IModel
{
public:
	virtual bool status() const = 0;

	virtual XMediaLib& getMediaLib() = 0;
	
	virtual CPlaylistMgr& getPlaylistMgr() = 0;
	virtual CPlayMgr& getPlayMgr() = 0;
	
	virtual CSingerMgr& getSingerMgr() = 0;
	virtual CSingerImgMgr& getSingerImgMgr() = 0;

	virtual COptionMgr& getOptionMgr() = 0;
	virtual CDataMgr& getDataMgr() = 0;
	virtual CBackupMgr& getBackupMgr() = 0;

	virtual bool initMediaLib(const wstring& strRootDir) = 0;
	virtual void refreshMediaLib() = 0;
		
	virtual void attachDir(const wstring& strDir) = 0;
	virtual void detachDir(const wstring& strDir) = 0;

	virtual bool renameMedia(const wstring& strOldOppPath, const wstring& strNewOppPath, bool bDir) = 0;

	virtual bool removeMedia(const TD_MediaList& lstMedias) = 0;

	virtual bool moveFiles(const wstring& strDir, const SMap<wstring, wstring>& mapUpdateFiles) = 0;

	virtual bool removeFiles(const std::set<wstring>& setFiles) = 0;

	virtual bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) = 0;

	virtual bool updateFile(const map<wstring, wstring>& mapUpdateFiles) = 0;

	using CB_exportorMedia = function<bool(UINT uProgressOffset, const wstring& strDstFile)>;
	virtual UINT exportMedia(const tagExportMedia& ExportMedia, const CB_exportorMedia& cb) = 0;

	virtual bool exportDB(const wstring& strExportDir) = 0;

	virtual void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
		, CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) = 0;

	virtual void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;
	virtual void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;

	virtual bool clearData() = 0;

	virtual void close() = 0;

    virtual wstring backupDB() = 0;
	virtual bool restoreDB(const wstring& strTag) = 0;
};

class __ModelExt CModel : public IModel
{
public:
	CModel(IModelObserver& ModelObserver);

private:
	IModelObserver& m_ModelObserver;
	
	CSQLiteDB m_db;
	CDao m_dao;
	
	COptionMgr m_OptionMgr;

	CDataMgr m_DataMgr;

	CBackupMgr m_BackupMgr;

	XMediaLib m_MediaLib;

	CPlaylistMgr m_PlaylistMgr;

	CSingerMgr m_SingerMgr;
	CSingerImgMgr m_SingerImgMgr;

	CPlayMgr m_PlayMgr;

public:
	bool status() const override
	{
		return m_db.GetStatus();
	}

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
	
	COptionMgr& getOptionMgr() override
	{
		return m_OptionMgr;
	}

	CDataMgr& getDataMgr() override
	{
		return m_DataMgr;
	}

	CBackupMgr& getBackupMgr() override
	{
		return m_BackupMgr;
	}

	void init();

	bool start();

	bool initMediaLib(const wstring& strRootDir) override;
	void refreshMediaLib() override;

	void attachDir(const wstring& strDir) override;
	void detachDir(const wstring& strDir) override;

	bool renameMedia(const wstring& strOldOppPath, const wstring& strNewOppPath, bool bDir) override;

	bool removeMedia(const TD_MediaList& lstMedias) override;

	bool moveFiles(const wstring& strDir, const SMap<wstring, wstring>& mapUpdateFiles) override;

	bool removeFiles(const std::set<wstring>& setFiles) override;

	bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) override;
	
	bool updateFile(const map<wstring, wstring>& mapUpdateFiles) override;

	UINT exportMedia(const tagExportMedia& ExportMedia, const CB_exportorMedia& cb) override;

	bool exportDB(const wstring& strExportDir) override;

	void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
		, CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) override;

	void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;
	void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;

	bool clearData() override;

	void close() override;

    wstring backupDB() override;
	bool restoreDB(const wstring& strTag) override;

private:
    wstring _dbFile() const;

	bool _initMediaLib();

    wstring _scanXMusicDir();

    bool _initData(const wstring& strDBFile, const wstring& strSingerImgDir);

	inline void _refreshMediaLib()
    {
        m_MediaLib.setDir(m_OptionMgr.getOption().strRootDir, m_OptionMgr.getOption().plAttachDir);
	}

	bool _updateDir(const wstring& strOldPath, const wstring& strNewPath);

	void _clear();

	void _close();
};
