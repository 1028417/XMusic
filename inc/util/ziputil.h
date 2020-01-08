
#pragma once

struct __UtilExt tagUnzfile
{
    string strPath;

	size_t uFileSize = 0;

	size_t pos_in_zip_directory = 0;
	size_t num_of_file = 0;
};

class __UtilExt CZipFile
{
public:
	CZipFile() {}

	CZipFile(const string& strFile, const string& strPwd = "")
	{
		(void)open(strFile, strPwd);
	}

	CZipFile(Instream& ins, const string& strPwd = "")
	{
		(void)open(ins, strPwd);
	}

	virtual ~CZipFile()
	{
		close();
	}

private:
	string m_strPwd;

	void* m_unzfile = NULL;

    list<tagUnzfile> m_lstUnzdirInfo;

    map<string, tagUnzfile> m_mapUnzfileInfo;

    list<pair<bool, tagUnzfile*>> m_lstUnzfileInfo;

private:
	bool _open(const char *szFile, void* pzlib_filefunc_def = NULL);
	
    long _readCurrent(void *buf, size_t len) const;

public:
	operator bool() const
	{
		return m_unzfile != NULL;
	}

    const map<string, tagUnzfile>& unzfileMap() const
    {
        return m_mapUnzfileInfo;
    }

    const list<tagUnzfile>& unzdirList() const
    {
        return m_lstUnzdirInfo;
    }

    const list<pair<bool, tagUnzfile*>>& unzfileList() const
    {
        return m_lstUnzfileInfo;
    }

    bool open(const string& strFile, const string& strPwd = "")
	{
		m_strPwd = strPwd;

		return _open(strFile.c_str());
	}

	bool open(Instream& ins, const string& strPwd = "");

    bool unzip(const wstring& strDstDir) const;

    long read(const tagUnzfile& unzFileInfo, CByteBuffer& bbfBuff) const
    {
        if (0 == unzFileInfo.uFileSize)
        {
            return 0;
        }

        auto ptr = bbfBuff.resizeMore(unzFileInfo.uFileSize);
        return _read(unzFileInfo, ptr, unzFileInfo.uFileSize);
	}
    long read(const string& strPath, CByteBuffer& bbfBuff) const
    {
        auto itr = m_mapUnzfileInfo.find(strPath);
        if (itr == m_mapUnzfileInfo.end())
        {
            return -1;
        }

        return read(itr->second, bbfBuff);
    }

    long read(const tagUnzfile& unzFileInfo, CCharBuffer& cbfBuff) const
    {
        if (0 == unzFileInfo.uFileSize)
        {
            return 0;
        }

        auto ptr = cbfBuff.resizeMore(unzFileInfo.uFileSize);
        return _read(unzFileInfo, ptr, unzFileInfo.uFileSize);
	}
    long read(const string& strPath, CCharBuffer& cbfBuff) const
    {
        auto itr = m_mapUnzfileInfo.find(strPath);
        if (itr == m_mapUnzfileInfo.end())
        {
            return -1;
        }

        return read(itr->second, cbfBuff);
    }

	void close();
	
private:
    long _read(const tagUnzfile& unzFileInfo, void *buf, size_t len) const;
};

class __UtilExt ziputil
{
public:
	static bool unzFile(const string& strZipFile, const wstring& strDstDir, const string& strPwd = "")
	{
		return CZipFile(strZipFile, strPwd).unzip(strDstDir);
	}
    static bool unzFile(Instream& ins, const wstring& strDstDir, const string& strPwd = "")
	{
		return CZipFile(ins, strPwd).unzip(strDstDir);
	}

    static int zCompress(const void* pData, size_t len, CByteBuffer& bbfBuff, int level=0);
    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);
    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

#if !__winvc
	static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel = -1);
	static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif
};
