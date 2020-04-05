
#pragma once

#include "modelDef.h"

#ifdef __ModelPrj
extern ITxtWriter& g_modelLogger;

#define __ModelExt __dllexport
#else
#define __ModelExt __dllimport
#endif

#if !__winvc
#define __OnlineMediaLib 1
#endif

#define __medialibDir L".xmusic"
#define __DBFile L"/medialib"

#include "modelDef.h"

#include "XMediaLib.h"

#include "DataMgr.h"

#include "OptionMgr.h"

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

	virtual void renameMedia(IMedia& media, const wstring& strNewName)
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
    virtual bool initMediaLib() = 0;
	
    virtual CDataMgr& getDataMgr() = 0;

    virtual CPlaylistMgr& getPlaylistMgr() = 0;
    virtual CPlayMgr& getPlayMgr() = 0;

    virtual CSingerMgr& getSingerMgr() = 0;
    virtual CSingerImgMgr& getSingerImgMgr() = 0;

#if __winvc
    virtual CBackupMgr& getBackupMgr() = 0;\
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

    virtual void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
        , CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) = 0;

    virtual void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;
    virtual void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) = 0;

    using CB_exportorMedia = function<bool(UINT uProgressOffset, const wstring& strDstFile)>;
    virtual UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) = 0;

    virtual wstring backupDB() = 0;
    virtual bool restoreDB(const wstring& strTag) = 0;
#endif

    virtual bool clearData() = 0;

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
	wstring medialibPath(const wstring& strSubPath = L"");

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

    void checkDuplicateMedia(E_CheckDuplicateMode eMode, const TD_MediaList& lstMedias
        , CB_checkDuplicateMedia cb, SArray<TD_MediaList>& arrResult) override;

    void checkSimilarFile(TD_MediaResList& lstMediaRes, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;
    void checkSimilarFile(TD_MediaResList& lstMediaRes1, TD_MediaResList& lstMediaRes2, CB_checkSimilarFile cb, TD_SimilarFile& arrResult) override;

    UINT exportMedia(const tagExportOption& ExportOption, const CB_exportorMedia& cb) override;

    wstring backupDB() override;
	bool restoreDB(const wstring& strTag) override;
#endif

    bool clearData() override;

    void close() override;

private:
#if __winvc
	wstring _scanXMusicDir();
	wstring _scanXMusicDir(PairList<wstring, E_AttachDirType>& plAttachDir);

	bool _exportDB(const wstring& strExportDir, bool bExportXmsc);
#endif

    bool _initData(const wstring& strDBFile);
	
	bool _updateDir(const wstring& strOldPath, const wstring& strNewPath);

	void _clear();

	void _close();
};
