
#pragma once

struct __UtilExt tagUnzFileInfo
{
	string strPath;
	bool bDir = false;

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

	list<tagUnzFileInfo> m_lstSubDirInfo;

    map<string, tagUnzFileInfo> m_mapSubFileInfo;

    list<tagUnzFileInfo*> m_lstSubInfo;

private:
	bool _open(const char *szFile, void* pzlib_filefunc_def = NULL);
	
    long _readCurrent(void *buf, size_t len) const;

public:
	operator bool() const
	{
		return m_unzfile != NULL;
	}

    const map<string, tagUnzFileInfo>& fileMap() const
    {
        return m_mapSubFileInfo;
    }

	bool open(const string& strFile, const string& strPwd = "")
	{
		m_strPwd = strPwd;

		return _open(strFile.c_str());
	}

	bool open(Instream& ins, const string& strPwd = "");

    bool unzip(const wstring& strDstDir) const;

    long read(const tagUnzFileInfo& unzFileInfo, void *buf, size_t len) const;
    long read(const string& strPath, void *buf, size_t len) const
    {
        auto itr = m_mapSubFileInfo.find(strPath);
        if (itr == m_mapSubFileInfo.end())
        {
            return -1;
        }

        return read(itr->second, buf, len);
    }

	template <typename T>
    long read(const tagUnzFileInfo& unzFileInfo, TBuffer<T>& buff) const
	{
		long size = read(unzFileInfo, buff.ptr(), buff.size());
		if (size > 0)
		{
			size /= sizeof(T);
		}

		return size;
	}
    template <typename T>
    long read(const string& strPath, TBuffer<T>& buff) const
    {
        auto itr = m_mapSubFileInfo.find(strPath);
        if (itr == m_mapSubFileInfo.end())
        {
            return -1;
        }

        return read(itr->second, buff);
    }
	
    long read(const tagUnzFileInfo& unzFileInfo, CByteBuffer& bbfBuff, size_t uReadSize = 0) const
	{
		return _read(unzFileInfo, bbfBuff, uReadSize);
	}
    long read(const string& strPath, CByteBuffer& bbfBuff, size_t uReadSize = 0) const
    {
        auto itr = m_mapSubFileInfo.find(strPath);
        if (itr == m_mapSubFileInfo.end())
        {
            return -1;
        }

        return read(itr->second, bbfBuff, uReadSize);
    }

    long read(const tagUnzFileInfo& unzFileInfo, CCharBuffer& cbfBuff, size_t uReadSize = 0) const
	{
		return _read(unzFileInfo, cbfBuff, uReadSize);
	}
    long read(const string& strPath, CCharBuffer& cbfBuff, size_t uReadSize = 0) const
    {
        auto itr = m_mapSubFileInfo.find(strPath);
        if (itr == m_mapSubFileInfo.end())
        {
            return -1;
        }

        return read(itr->second, cbfBuff, uReadSize);
    }

	void close();
	
private:
	template <class T>
    long _read(const tagUnzFileInfo& unzFileInfo, T& buff, size_t uReadSize) const
	{
		if (0 == uReadSize)
		{
			uReadSize = unzFileInfo.uFileSize;
		}
		else
		{
			uReadSize = MIN(uReadSize, unzFileInfo.uFileSize);
		}
		if (0 == uReadSize)
		{
			return 0;
		}

		auto ptr = buff.resizeMore(uReadSize);
		long size = read(unzFileInfo, ptr, uReadSize);
		if (size >= 0 && size < (long)uReadSize)
		{
			buff.resizeLess(uReadSize - size);
		}
		
		return size;
	}
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
