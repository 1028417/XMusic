
#pragma once

class ziputil
{
public:
#if !__winvc
    static long qCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int nCompressLecvel=-1);
    static long qUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);
#endif

    static long zCompressFile(const wstring& strSrcFile, const wstring& strDstFile, int level=0);
    static long zUncompressFile(const wstring& strSrcFile, const wstring& strDstFile);

    static bool zUncompressZip(const string& strZipFile, const string& strDstDir);
};
