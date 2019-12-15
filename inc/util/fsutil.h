
#pragma once

#define MAX_PATH 260

#if !__winvc
#include <QFileInfo>
#include <QFile>
#include <QDir>

#include <QTime>
#endif

#if __windows
#include <direct.h>

#include <sys/utime.h>
#else
#include <utime.h>

using _utimbuf = struct utimbuf;
#define _wutime(f, t) utime(strutil::toStr(f).c_str(), t)
#endif

#include <sys/stat.h>

#if __windows
using tagFileStat64 = struct _stat64;

#else
#if __android
using tagFileStat64 = struct stat64;
#else
using tagFileStat64 = struct stat;
#endif

#define _fileno fileno
#endif

enum class E_FindFindFilter
{
	FFP_None
	, FFP_ByPrefix
	, FFP_ByExt
};

struct __UtilExt tagFileInfo
{
	tagFileInfo() {}

	tagFileInfo(class CPath *t_pParent, bool t_bDir, const wstring& t_strName)
		: pParent(t_pParent)
		, bDir(t_bDir)
		, strName(t_strName)
	{
	}

	tagFileInfo(bool t_bDir, const wstring& t_strName)
		: bDir(t_bDir)
		, strName(t_strName)
	{
	}

    class CPath *pParent = NULL;

	bool bDir = true;

	wstring strName;

    uint64_t uFileSize = 0;

	time64_t tCreateTime = 0;
	time64_t tModifyTime = 0;
};


#define __wcSlant L'/'
#define __wcBackSlant L'\\'

#if __windows
    #define __wcDirSeparator	__wcBackSlant
#else
    #define __wcDirSeparator	__wcSlant
#endif

#define __wcDot L'.'

class __UtilExt fsutil
{
public:
    inline static bool checkPathTail(wchar_t wch)
    {
        return __wcBackSlant == wch || __wcSlant == wch;
    }

	static void trimPathTail(wstring& strPath);
	static wstring trimPathTail_r(const wstring& strPath)
	{
		wstring t_strPath(strPath);
		trimPathTail(t_strPath);
		return t_strPath;
	}

	static void transFSSlant(wstring& strPath)
	{
#if __windows
        strutil::replaceChar(strPath, __wcSlant, __wcBackSlant);
#else
        strutil::replaceChar(strPath, __wcBackSlant, __wcSlant);
#endif
	}

    static FILE* fopen(const wstring& strFile, const string& strMode);
	static FILE* fopen(const string& strFile, const string& strMode);

	static bool copyFile(const wstring& strSrcFile, const wstring& strDstFile);
	static bool copyFile(const string& strSrcFile, const string& strDstFile);

	using CB_CopyFile = function <bool(char *lpData, size_t size)>;
    static bool copyFileEx(const wstring& strSrcFile, const wstring& strDstFile
		, const CB_CopyFile& cb = NULL, const string& strHead = "");

    static bool fStat64(FILE *pf, tagFileStat64& stat);
    static bool lStat64(const wstring& strFile, tagFileStat64& stat);

    static long long GetFileSize64(const wstring& strFile);

    static time64_t GetFileModifyTime64(FILE *pf);
	static time64_t GetFileModifyTime64(const wstring& strFile);

    static void SplitPath(const wstring& strPath, wstring *pstrDir, wstring *pstrFile);
    static void SplitPath(const string& strPath, string *pstrDir, string *pstrFile);

	static wstring GetRootDir(const wstring& strPath);
	static wstring GetParentDir(const wstring& strPath);

	static wstring GetFileName(const wstring& strPath);

    static wstring getFileTitle(const wstring& strPath);
    static string getFileTitle(const string& strPath);

	static wstring GetFileExtName(const wstring& strPath);

	static bool CheckSubPath(const wstring& strDir, const wstring& strSubPath);

	static wstring GetOppPath(const wstring& strPath, const wstring strBaseDir);

	static bool existPath(const wstring& strPath, bool bDir);
    static bool existDir(const wstring& strDir);
	static bool existFile(const wstring& strFile);

    static bool createDir(const wstring& strDir);
    static bool removeDir(const wstring& strDir);
    static bool removeFile(const wstring& strFile);

	static bool moveFile(const wstring& strSrcFile, const wstring& strDstFile);

    static long lSeek(FILE *pf, long offset, int origin);
    static long long lSeek64(FILE *pf, long long offset, int origin);

    static wstring workDir();
    static bool setWorkDir(const wstring& strWorkDir);

#if __windows
    static wstring getModuleDir(wchar_t *pszModuleName = NULL);
#endif

#if !__winvc
    static wstring getHomeDir();
#endif

	using CB_FindFile = cfn_void_t<tagFileInfo&>;
	static bool findFile(const wstring& strDir, CB_FindFile cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wchar_t *pstrFilter = NULL);
	
	static bool findSubDir(const wstring& strDir, cfn_void_t<const wstring&> cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wchar_t *pstrFilter = NULL)
	{
		return findFile(strDir, [&](tagFileInfo& fileInfo) {
			if (fileInfo.bDir)
			{
				cb(fileInfo.strName);
			}
		}, eFilter, pstrFilter);
	}

	static bool findSubFile(const wstring& strDir, cfn_void_t<const wstring&> cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wchar_t *pstrFilter = NULL)
	{
		return findFile(strDir, [&](tagFileInfo& fileInfo) {
			if (!fileInfo.bDir)
			{
				cb(fileInfo.strName);
			}
		}, eFilter, pstrFilter);
	}
};

#include "Path.h"

#include "fstream.h"

#include "TxtWriter.h"

#include "SQLiteDB.h"

#include "jsonutil.h"

#include "xmlutil.h"

#include "ziputil.h"
