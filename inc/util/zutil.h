
#pragma once

class __UtilExt zutil
{
public:
    static bool unzipFile(const string& strZipFile, const wstring& strDstDir);
    static bool unzipFile(Instream& ins, const wstring& strDstDir);

    static int zCompress(const void* pData, size_t len, CByteBuff& btbBuff, int level=0);
    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);
    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

#if !__winvc
	static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel = -1);
	static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif
};
