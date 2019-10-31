
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

	list<tagUnzFileInfo> m_lstSubFileInfo;
	list<tagUnzFileInfo> m_lstSubDirInfo;

	list<tagUnzFileInfo*> m_lstSubInfo;

private:
	bool _open(const char *szFile, void* pzlib_filefunc_def = NULL);
	
	long _readCurrent(void *buf, size_t len);

public:
	operator bool() const
	{
		return m_unzfile != NULL;
	}

	const list<tagUnzFileInfo>& subFileInfo() const
	{
		return m_lstSubFileInfo;
	}

	bool open(const string& strFile)
	{
		return _open(strFile.c_str());
	}

	bool open(Instream& ins);

	bool unzip(const wstring& strDstDir);

	long read(const tagUnzFileInfo& unzFileInfo, void *buf, size_t len);

	bool readex(const tagUnzFileInfo& unzFileInfo, void *buf, size_t len)
	{
        return read(unzFileInfo, buf, len) == (long)len;
	}

	template <typename T>
	long read(const tagUnzFileInfo& unzFileInfo, TBuffer<T>& buff)
	{
		long size = read(unzFileInfo, buff.ptr(), buff.size());
		if (size > 0)
		{
			size /= sizeof(T);
		}

		return size;
	}
	
	long read(const tagUnzFileInfo& unzFileInfo, CByteBuffer& bbfBuff, size_t uReadSize = 0)
	{
		return _read(unzFileInfo, bbfBuff, uReadSize);
	}
	long read(const tagUnzFileInfo& unzFileInfo, CCharBuffer& cbfBuff, size_t uReadSize = 0)
	{
		return _read(unzFileInfo, cbfBuff, uReadSize);
	}

	void close();
	
private:
	template <class T>
	long _read(const tagUnzFileInfo& unzFileInfo, T& buff, size_t uReadSize)
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

    static int zCompress(const void* pData, size_t len, CByteBuffer& bbfBuff, int level=0);
    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);
    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

#if !__winvc
	static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel = -1);
	static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif
};
