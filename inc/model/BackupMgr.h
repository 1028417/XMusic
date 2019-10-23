#pragma once

using CB_BackupCompare = fn_void_t <const tagCompareBackupResult&>;

class __ModelExt CBackupMgr
{
friend class CModel;

public:
	CBackupMgr(CDao& dao, class CModel& model)
		: m_dao(dao)
		, m_model(model)
	{
	}

private:
	CDao& m_dao;

	class CModel& m_model;
	
	wstring m_strBackupDir;

	SArray<wstring> m_arrBackupTag;

	mutex m_mutex;
	using TD_CompareResultMap = SHashMap<wstring, tagCompareBackupResult>;
	SHashMap<wstring, TD_CompareResultMap> m_mapCompareResult;

	CB_BackupCompare m_cb;
	pair<wstring, wstring> m_prWaitTask;

private:
	void _onCompareFinish(const tagCompareBackupResult& result);

	void _saveCompareResult(const tagCompareBackupResult& result);

	bool _getCompareResult(const wstring& strSrcTag, const wstring& strDstTag, const CB_BackupCompare& cb);

private:
    wstring backup(const wstring& strDBFile);

    bool restore(const wstring& strTag, const wstring& strDBFile);

public:
	const SArray<wstring>& getBackupTags()
	{
		return m_arrBackupTag;
	}

    void init();

	void close();

	void removeBackup(const wstring& strTag);

	void getCompareResult(const SList<pair<wstring, wstring>>& lstTags, SList<tagCompareBackupResult>& lstResult);

	void compareBackup(const SList<pair<wstring, wstring>>& lstTags, SList<tagCompareBackupResult>& lstResult);

	void compareBackup(const wstring& strSrcTag, const wstring& strDstTag, const CB_BackupCompare& cb=NULL);

	bool compareBackup(const wstring& strSrcTag, tagCompareBackupResult& result);
};
