
#pragma once

#include "util.h"

#ifdef __ModelPrj
extern const ITxtWriter& g_logWriter;

#define __ModelExt __dllexport
#else
#define __ModelExt __dllimport
#endif

#define TRUE 1
#define FALSE 0

#include "MediaDef.h"

#include "Dao.h"

#include "DataMgr.h"

#include "BackupMgr.h"

#include "PlaylistMgr.h"

#include "PlayMgr.h"

#include "SingerMgr.h"

#include "SingerImgMgr.h"

#include "AutoMatch.h"

#include "MediaMixer.h"

class IModelObserver
{
public:
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

	bool bCompareFileSize = false;
	bool bCompareFileTime = false;

	bool bDeleteOther = true;

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
	virtual CRootMediaRes& getRootMediaRes() = 0;
	virtual CMediaSet& getRootMediaSet() = 0;
	
	virtual CPlaylistMgr& getPlaylistMgr() = 0;
	virtual CPlayMgr& getPlayMgr() = 0;
	
	virtual CSingerMgr& getSingerMgr() = 0;
	virtual CSingerImgMgr& getSingerImgMgr() = 0;

	virtual CDataMgr& getDataMgr() = 0;
	virtual CBackupMgr& getBackupMgr() = 0;

	virtual bool refreshRootMediaRes(const wstring& strRootDir) = 0;
	virtual void refreshRootMediaRes() = 0;
		
	virtual void attachDir(const wstring& strDir) = 0;
	virtual void detachDir(const wstring& strDir) = 0;

	virtual bool renameMedia(const wstring& strOldOppPath, const wstring& strNewOppPath, bool bDir) = 0;

	virtual bool removeMedia(const TD_MediaList& lstMedias) = 0;

	virtual bool moveFiles(const wstring& strDir, const SMap<wstring, wstring>& mapUpdateFiles) = 0;

	virtual bool removeFiles(const std::set<wstring>& setFiles) = 0;

	virtual bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) = 0;

	virtual bool updateFile(const map<wstring, wstring>& mapUpdateFiles) = 0;

	using CB_exportorMedia = function<bool(UINT uProgressOffset, const wstring& strDstFile)>;
	virtual UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) = 0;

	virtual void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
		, CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) = 0;

	virtual void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;
	virtual void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;

	virtual bool clearData() = 0;

	virtual void close() = 0;

	virtual bool restoreDB(const wstring& strFile) = 0;
};

class __ModelExt CModel : public IModel
{
private:
	class CRootMediaSet : public CMediaSet
	{
	public:
		CRootMediaSet(CPlaylistMgr& PlaylistMgr, CSingerMgr& SingerMgr)
			: CMediaSet(L"曲库")
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
	
public:
	CModel(IModelObserver& ModelObserver);

private:
	IModelObserver& m_ModelObserver;

	CSQLiteDB m_db;
	CDao m_dao;

	CDataMgr m_DataMgr;

	CBackupMgr m_BackupMgr;

	CRootMediaRes m_RootMediaRes;

	CRootMediaSet m_RootMediaSet;

	CPlaylistMgr m_PlaylistMgr;

	CSingerMgr m_SingerMgr;
	CSingerImgMgr m_SingerImgMgr;

	CPlayMgr m_PlayMgr;

	bool m_bInited = false;

public:
	CRootMediaRes& getRootMediaRes() override
	{
		return m_RootMediaRes;
	}

	CMediaSet& getRootMediaSet() override
	{
		return m_RootMediaSet;
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
	
	CDataMgr& getDataMgr() override
	{
		return m_DataMgr;
	}

	CBackupMgr& getBackupMgr() override
	{
		return m_BackupMgr;
	}

	bool init();

	bool refreshRootMediaRes(const wstring& strRootDir) override;
	void refreshRootMediaRes() override;

	void attachDir(const wstring& strDir) override;
	void detachDir(const wstring& strDir) override;

	bool renameMedia(const wstring& strOldOppPath, const wstring& strNewOppPath, bool bDir) override;

	bool removeMedia(const TD_MediaList& lstMedias) override;

	bool moveFiles(const wstring& strDir, const SMap<wstring, wstring>& mapUpdateFiles) override;

	bool removeFiles(const std::set<wstring>& setFiles) override;

	bool updateMediaPath(const map<CMedia*, wstring>& mapUpdateMedias) override;
	
	bool updateFile(const map<wstring, wstring>& mapUpdateFiles) override;

	UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) override;

	void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
		, CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) override;

	void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;
	void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;

	bool clearData() override;

	void close() override;

	bool restoreDB(const wstring& strFile) override;

private:
	bool _updateDir(const wstring& strOldPath, const wstring& strNewPath);

	void _clear();
};
