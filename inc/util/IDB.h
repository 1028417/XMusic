#pragma once

class __UtilExt IDBResult
{
public:
	virtual ~IDBResult(){}

	virtual const UINT& GetColumnCount() = 0;
	virtual const UINT& GetRowCount() = 0;

	virtual bool GetData(UINT uRow, UINT uColumn, wstring& strValue) = 0;
	virtual bool GetData(UINT uRow, UINT uColumn, string& strValue) = 0;
	virtual bool GetData(UINT uRow, UINT uColumn, int& nValue) = 0;
	virtual bool GetData(UINT uRow, UINT uColumn, UINT& uValue) = 0;
    virtual bool GetData(UINT uRow, UINT uColumn, bool& bValue) = 0;
	virtual bool GetData(UINT uRow, UINT uColumn, double& dbValue) = 0;

	virtual bool GetData(UINT uRow, SArray<wstring>& arrData) = 0;
	virtual bool GetData(UINT uRow, SArray<string>& arrData) = 0;
	virtual bool GetData(UINT uRow, SArray<int>& arrData) = 0;
	virtual bool GetData(UINT uRow, SArray<UINT>& arrData) = 0;
	virtual bool GetData(UINT uRow, SArray<double>& arrData) = 0;
};

class IDB
{
public:
    virtual int GetStatus() const = 0;

	virtual bool Connect(const wstring& strPara=L"") = 0;
	virtual bool Disconnect() = 0;

	virtual bool Execute(const string& strSql) = 0;
	virtual bool Execute(const wstring& strSql) = 0;

	virtual IDBResult* Query(const string& strSql) = 0;
	virtual IDBResult* Query(const wstring& strSql) = 0;

	virtual bool BeginTrans() = 0;
	virtual bool RollbakTrans() = 0;
	virtual bool CommitTrans() = 0;
};

class __UtilExt CDBTransGuard
{
public:
	CDBTransGuard(IDB& db, bool bAutoCommit = true)
	{
		if (db.BeginTrans())
		{
			m_pDB = &db;

			m_bInTrans = true;

			m_bAutoCommit = bAutoCommit;
		}
	}

	CDBTransGuard(IDB *pDB, bool bAutoCommit = true)
	{
		if (NULL != pDB && pDB->BeginTrans())
		{
			m_pDB = pDB;

			m_bInTrans = true;

			m_bAutoCommit = bAutoCommit;
		}
	}

	~CDBTransGuard()
	{
		if (m_bInTrans)
		{
			if (m_bAutoCommit)
			{
				(void)Commit();
			}
			else
			{
				(void)Rollback();
			}
		}
	}

private:
	IDB *m_pDB = NULL;

	bool m_bAutoCommit = true;

	bool m_bInTrans = false;

public:
	bool Commit()
	{
		m_bInTrans = false;

		if (NULL != m_pDB)
		{
			try
			{
				return m_pDB->CommitTrans();
			}
			catch (...)
			{
			}
		}
		
		return false;
	}

	bool Rollback()
	{
		m_bInTrans = false;

		if (NULL != m_pDB)
		{
			try
			{
				return m_pDB->RollbakTrans();
			}
			catch (...)
			{
			}
		}

		return false;
	}
};
