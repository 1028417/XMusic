
#pragma once

struct __UtilExt tagUnzFileInfo
{
	string strPath;
	bool bDir = false;

	UINT uFileSize = 0;

	UINT pos_in_zip_directory = 0;
	UINT num_of_file = 0;
};

class __UtilExt CZipFile
{
public:
	CZipFile() {}

	CZipFile(const string& strFile)
	{
		(void)open(strFile);
	}

	CZipFile(Instream& ins)
	{
		(void)open(ins);
	}

	virtual ~CZipFile()
	{
		close();
	}

private:
	void* m_unzfile = NULL;

	list<tagUnzFileInfo> m_lstUnzFileInfo;

private:
	int _readCurrent(void *buf, UINT len);
	
public:
	operator bool() const
	{
		return m_unzfile != NULL;
	}

	const list<tagUnzFileInfo>& fileList() const
	{
		return m_lstUnzFileInfo;
	}

	bool open(const string& strFile);
	bool open(Instream& ins);

	bool unzip(const wstring& strDstDir);

	int read(const tagUnzFileInfo& unzFileInfo, void *buf, UINT len);

	void close();
};

class __UtilExt zutil
{
public:
	static bool unzFile(const string& strZipFile, const wstring& strDstDir)
	{
		return CZipFile(strZipFile).unzip(strDstDir);
	}
    static bool unzFile(Instream& ins, const wstring& strDstDir)
	{
		return CZipFile(ins).unzip(strDstDir);
	}

    static int zCompress(const void* pData, size_t len, CByteBuff& btbBuff, int level=0);
    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);
    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

#if !__winvc
	static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel = -1);
	static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif
};
