#pragma once

using CB_BackupCompare = fn_void_t <const tagCompareBackupResult&>;

class __ModelExt CBackupMgr
{
public:
	CBackupMgr(class CModel& model)
		: m_model(model)
	{
	}

private:
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

	bool _getCompareResult(cwstr strSrcTag, cwstr strDstTag, const CB_BackupCompare& cb);
	
public:
	const SArray<wstring>& getBackupTags() const
	{
		return m_arrBackupTag;
	}

	wstring getBackupFile(cwstr strTag) const;

    void init();

	void close();

	bool exportDB(cwstr strDstFile, bool bExportXmsc, const PtrArray<const tagSingerImg>& paSingerImg);

	wstring backup();

	void removeBackup(cwstr strTag);

	void getCompareResult(const prlist<wstring, wstring>& plTags, SList<tagCompareBackupResult>& lstResult);

	void compareBackup(const prlist<wstring, wstring>& plTags, SList<tagCompareBackupResult>& lstResult);

	void compareBackup(cwstr strSrcTag, cwstr strDstTag, const CB_BackupCompare& cb=NULL);

	bool compareBackup(cwstr strSrcTag, tagCompareBackupResult& result);
};
