
#pragma once

class __UtilExt zutil
{
public:
#if !__winvc
    static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel=-1);
    static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif

    static int zCompress(const void* pData, size_t len, CByteBuff& btbBuff, int level=0);
    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);

    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

    static bool zipUncompress(const string& strZipFile, const string& strDstDir);
};
