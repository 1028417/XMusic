
#pragma once

class __UtilExt ZInopaque
{
public:
	ZInopaque(Instream& stream)
		: m_stream(stream)
	{
	}

private:
	Instream& m_stream;

public:
	Instream& stream()
	{
		return m_stream;
	}

	bool unzip(const wstring& strDstDir);
};

class __UtilExt zutil
{
public:
	static bool unzipFile(const string& strZipFile, const wstring& strDstDir);

    static int zCompress(const void* pData, size_t len, CByteBuff& btbBuff, int level=0);
    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);
    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

#if !__winvc
	static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel = -1);
	static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif
};
