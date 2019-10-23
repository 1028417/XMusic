#pragma once

#include "IDB.h"

class CSQLiteDB;
class __UtilExt CSQLiteDBResult : public IDBResult
{
public:
	CSQLiteDBResult(UINT uColumnCount, UINT uRowCount, char ** pData)
		: m_uColumnCount(uColumnCount)
		, m_uRowCount(uRowCount)
		, m_pData(pData)
	{
	}

	~CSQLiteDBResult();

private:
	UINT m_uColumnCount = 0;
	UINT m_uRowCount = 0;

	char ** m_pData = NULL;

private:
	bool _getData(UINT uRow, cfn_void_t<const string&> cb);

public:
	const UINT& GetColumnCount() override
	{
		return m_uColumnCount;
	}

	const UINT& GetRowCount() override
	{
		return m_uRowCount;
	}

	bool GetData(UINT uRow, UINT uColumn, wstring& strData) override;
	bool GetData(UINT uRow, UINT uColumn, string& strData) override;
	bool GetData(UINT uRow, UINT uColumn, int& nValue) override;
	bool GetData(UINT uRow, UINT uColumn, UINT& uValue) override;
    bool GetData(UINT uRow, UINT uColumn, bool& bValue) override;
	bool GetData(UINT uRow, UINT uColumn, double& dbValue) override;

	bool GetData(UINT uRow, SArray<wstring>& arrData) override;
	bool GetData(UINT uRow, SArray<string>& arrData) override;
	bool GetData(UINT uRow, SArray<int>& arrData) override;
	bool GetData(UINT uRow, SArray<UINT>& arrData) override;
	bool GetData(UINT uRow, SArray<double>& arrData) override;
};


class __UtilExt CSQLiteDB : public IDB
{
public:
	CSQLiteDB(const wstring& strFile=L"");

    ~CSQLiteDB()
    {
        Disconnect();
    }

private:
	void *m_hDB = NULL;

	wstring m_strFile;

    int m_nRetCode = 0;
	string m_strError;
	
	bool m_bInTrans = false;

public:
	const wstring& file() const
	{
		return m_strFile;
	}

    int GetErrCode() const
    {
        return m_nRetCode;
    }

    const string& GetErrMsg() const
    {
        return m_strError;
    }

    int GetStatus() const override
    {
        return (NULL != m_hDB);
    }

	bool Connect(const wstring& strPara=L"") override;

	bool Disconnect() override;

	bool Execute(const string& strSql) override;
	bool Execute(const wstring& strSql) override;

	IDBResult* Query(const string& strSql) override;
	IDBResult* Query(const wstring& strSql) override;

	bool BeginTrans() override;
	bool CommitTrans() override;
	bool RollbakTrans() override;
};
