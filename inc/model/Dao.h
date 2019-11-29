#pragma once

#include "util/util.h"

#ifdef __DalPrj
#define __DalExt __dllexport
#else
#define __DalExt __dllimport
#endif

using dbtime_t = int32_t;

#define __ID_GROUP_ROOT 0

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

	int fileSize = 0;
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

struct tagModifyedMedia
{
	wstring strOldPath;

	wstring strNew;

	wstring strMediaSetName;

	wstring strSingerName;
};

typedef struct tagNewPlayItem
{
	wstring strPath;

	wstring strPlaylistName;
} tagDeletedPlayItem;

typedef struct tagNewAlbumItem
{
	wstring strPath;

	wstring strAlbumName;

	wstring strSingerName;

	wstring strSingerDir;

	int iSingerID = 0;

} tagDeletedAlbumItem;

struct tagMovedMedia
{
	wstring strPath;

	wstring strOldMediaSetName;
	wstring strNewMediaSetName;

	wstring strSingerName;
};

struct tagCompareBackupResult
{
	tagCompareBackupResult()
	{
	}

	tagCompareBackupResult(const wstring t_strSrcTag, const wstring t_strDstTag=L"")
		: strSrcTag(t_strSrcTag)
		, strDstTag(t_strDstTag)
	{
	}

	wstring strSrcTag;
	wstring strDstTag;

	SArray<tagNewPlayItem> arrNewPlayItem;
	SArray<tagNewAlbumItem> arrNewAlbumItem;

	SArray<tagDeletedPlayItem> arrDeletedPlayItem;
	SArray<tagDeletedAlbumItem> arrDeletedAlbumItem;

	SArray<tagModifyedMedia> arrModifyedMedia;

	SArray<tagMovedMedia> arrMovedMedia;
};

class __DalExt CDao
{
public:
	class __DalExt CTransGuard : public CDBTransGuard
	{
	public:
		CTransGuard(CDao& dao)
			: CDBTransGuard(&dao.m_db)
		{
		}

		CTransGuard(CDao *pdao)
			: CDBTransGuard(NULL!=pdao?&pdao->m_db:NULL)
		{
		}
	};

public:
	CDao(IDB& db)
		: m_db(db)
	{
	}

	~CDao()
	{
		if (NULL != m_pRst)
		{
			delete m_pRst;
		}
	}

private:
	IDB& m_db;

	IDBResult *m_pRst = NULL;

private:
	wstring _getNextName(const wstring& strTableName, const wstring& strNameColumn, const wstring& strBaseName);

	bool _getMaxValue(const wstring& strTableName, const wstring& strNameColumn, int& iRet);

	int _queryMaxAlbumItemPos(int nAlbumID);

	bool _queryDifferent(tagCompareBackupResult& result);
	
public:
	bool execute(const string& strSql);
	bool execute(const wstring& strSql);

	IDBResult *query(const string& strSql);
	IDBResult *query(const wstring& strSql);

	wstring getNewPlaylistName(const wstring& strBaseName);
	wstring getNewSingerGroupName(const wstring& strBaseName);
	wstring getNewAlbumName(UINT uSingerID, const wstring& strBaseName);

	bool queryAlarmmedia(vector<wstring>& vecAlarmmedia);
	bool addAlarmmedia(const SArray<wstring>& vecAlarmmedia);
	bool deleteAlarmmedia(const wstring& strPath);
	bool clearAlarmmedia();

	bool queryPlayRecordMaxTime(dbtime_t& tTime);
	bool queryPlayRecord(dbtime_t tTime, vector<pair<wstring, int>>& vecPlayRecord);
	bool clearPlayRecord();

	bool updateFiles(const wstring& strOldOppPath, const wstring& strNewOppPath, bool bDir);
	bool deleteFiles(const set<wstring>& setDeleteFiles);

	bool compare(const wstring& strFile, tagCompareBackupResult& result);

	bool exportData(const wstring& strDstFile, const list<wstring>& lstSingerImg);

	bool clearAll();

public:
	bool queryPlaylist(SArray<tagPlaylistInfo>& arrPlaylistInfo);

	int addPlaylist(const wstring& strName);

	bool updatePlaylistName(UINT uID, const wstring& strName);
	bool updatePlaylistPos(UINT uID, UINT uOldPos, UINT uNewPos);
	bool updatePlaylistProperty(UINT uID, UINT uLanguage, bool bDisableDemand, bool bDisableExport);

	bool deletePlaylist(UINT uID);

	int GetMaxPlayItemPos(UINT uPlaylistID);

	using CB_addPlayItem = function<void(UINT uPlayItemID, wstring strPath, dbtime_t)>;
    bool addPlayItem(const SArray<wstring>& lstPaths, UINT uPlaylistID, const CB_addPlayItem& cb);

	bool deletePlayItem(const list<UINT>& lstIDs);
	bool deletePlayItem(UINT uPlaylistID);

	bool updatePlayItemPath(UINT uPlayItemID, const wstring& strPath);

	bool setbackPlayItem(UINT uPlaylistID, const list<UINT>& lstIDs);

public:
	bool querySimgerImg(list<wstring>& lstSingerImg);

	bool querySinger(SArray<tagSingerGroupInfo>& arrSingerGroupInfo);

	int addSingerGroup(const wstring& strName);

	bool updateSingerGroupName(UINT uSingerGroupID, const wstring& strName);
	bool updateSingerGroupProperty(UINT uSingerID, UINT uLanguage, bool bDisableDemand, bool bDisableExport);

	bool deleteSingerGroup(UINT uSingerGroupID);

	int addSinger(UINT uGroupID, const wstring& strSingerName, const wstring& strPath, int nPos);

	bool updateSingerName(UINT uSingerID, const wstring& strName);
	bool updateSingerPos(UINT uSingerID, int nPos, int nDstGroupID);
	bool updateSingerProperty(UINT uSingerID, UINT uLanguage, bool bDisableDemand, bool bDisableExport);
	bool updateSingerPath(const map<UINT, wstring>& mapSingerDirChanged);
	
	bool deleteSinger(UINT uSingerID);

	int addAlbum(const wstring& strName, UINT uSingerID, UINT pos);

	bool updateAlbumName(UINT uAlbumID, const wstring& strName);
	bool updateAlbumPos(UINT uAlbumID, int nOldPos, int nNewPos, UINT uSingerID);
	bool updateAlbumProperty(UINT uAlbumID, UINT uLanguage, bool bDisableDemand, bool bDisableExport);

	bool deleteAlbum(UINT uAlbumID, UINT uSingerID);

    using CB_addAlbumItem = function<void(UINT uPlayItemID, const wstring& strPath, dbtime_t)>;
    bool addAlbumItem(const list<wstring>& lstPaths, UINT uAlbumID, const CB_addAlbumItem& cb);

	bool updateAlbumItemPath(UINT uAlbumItemID, const wstring& strPath);

	bool deleteAlbumItem(UINT uAlbumItem);
	bool deleteAlbumItem(const list<UINT>& lstAlbumItemIDs);

	bool setbackAlbumItem(UINT uAlbumID, const list<UINT>& lstAlbumItemID);

/*public:
	bool GetOption(E_OptionIndex eOptionIndex, bool& bOptionValue)
	{
		int iOptionValue = 0;
		if (!GetOption(eOptionIndex, iOptionValue))
		{
			return false;
		}

		bOptionValue = 1 == iOptionValue;

		return true;
	}

	template <typename T>
	bool GetOption(E_OptionIndex eOptionIndex, T& OptionValue)
	{
		wstring strSql = L"SELECT value FROM tbl_option WHERE id = " + to_wstring((UINT)eOptionIndex);
		IDBResult *pRst = query(strSql);
		__EnsureReturn(pRst, false);

		if (1 == pRst->GetRowCount())
		{
			__EnsureReturn(pRst->GetData(0, 0, OptionValue), false);
		}

		return true;
	}

	template <typename T>
	bool SetOption(E_OptionIndex eOptionIndex, const T& OptionValue)
	{
		wstringstream ssSql;
		ssSql << L"INSERT OR REPLACE INTO tbl_option \
				   VALUES(" << (UINT)eOptionIndex << L", \"" << OptionValue << L"\")";

		return m_db.Execute(ssSql.str());
	}*/
};

using CDaoTransGuard = CDao::CTransGuard;
