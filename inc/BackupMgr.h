#pragma once

using CB_BackupCompare = function<void(const tagCompareBackupResult&)>;

class __ModelExt CBackupMgr
{
public:
	CBackupMgr(CDao& dao)
		: m_dao(dao)
	{
	}

private:
	CDao& m_dao;
	
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

public:
	const SArray<wstring>& getBackupTags()
	{
		return m_arrBackupTag;
	}

    void init();

	void close();
	
	wstring backup();

	void removeBackup(const wstring& strTag);
	
	bool restore(const wstring& strTag);

	void getCompareResult(const SList<pair<wstring, wstring>>& lstTags, SList<tagCompareBackupResult>& lstResult);

	void compareBackup(const SList<pair<wstring, wstring>>& lstTags, SList<tagCompareBackupResult>& lstResult);

	void compareBackup(const wstring& strSrcTag, const wstring& strDstTag, const CB_BackupCompare& cb=NULL);

	bool compareBackup(const wstring& strSrcTag, tagCompareBackupResult& result);
};
