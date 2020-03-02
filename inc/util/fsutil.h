
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

#if __ios || __mac
#define fseek64 fseek
#define ftell64 ftell
#define lseek64 lseek

#elif __windows
#define fseek64 _fseeki64
#define ftell64 _ftelli64
#define lseek64 _lseeki64
#define lseek(fno, offset, origin) (long)lseek64(fno, offset, origin)

#else
#define fseek64 fseeko
#define ftell64 ftello
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


#define __chrSlant '/'
#define __chrBackSlant '\\'

#define __wchSlant ((wchar_t)__chrSlant)
#define __wchBackSlant ((wchar_t)__chrBackSlant)

#if __windows
    #define __wchDirSeparator	__wchBackSlant
#else
    #define __wchDirSeparator	__wchSlant
#endif

#define __chrDot '.'
#define __wchDot (wchar_t)__chrDot

class __UtilExt fsutil
{
public:
	inline static bool checkPathTail(wchar_t wch)
	{
		return (wchar_t)__chrBackSlant == wch || (wchar_t)__chrSlant == wch;
	}
	inline static bool checkPathTail(char chr)
	{
		return __chrBackSlant == chr || __chrSlant == chr;
	}

    template <class S>
    static void trimPathTail(S& strPath)
    {
        if (!strPath.empty())
        {
            if (checkPathTail(strPath.back()))
            {
                strPath.pop_back();
            }
        }
    }

    template <class S>
    static S trimPathTail_r(const S& strPath)
	{
        auto t_strPath = strPath;
		trimPathTail(t_strPath);
		return t_strPath;
	}

    template <class S>
    static void transFSSlant(S& strPath)
    {
#if __windows
        strutil::replaceChar(strPath, __chrSlant, __chrBackSlant);
#else
        strutil::replaceChar(strPath, __chrBackSlant, __chrSlant);
#endif
    }

    template <class S>
    static S transFSSlant_r(const S& strPath)
    {
        auto t_strPath = strPath;
        transFSSlant(t_strPath);
        return t_strPath;
    }

    static FILE* fopen(const wstring& strFile, const string& strMode);
	static FILE* fopen(const string& strFile, const string& strMode);

    static bool copyFile(const wstring& strSrcFile, const wstring& strDstFile);

#if __windows
    static bool copyFile(const string& strSrcFile, const string& strDstFile);
#endif

#if !__winvc
    static bool copyFile(const QString& qsSrcFile, const QString& qsDstFile)
    {
        return QFile::copy(qsSrcFile, qsDstFile);
    }
#endif

	using CB_CopyFile = function <bool(char *lpData, size_t size)>;
    static bool copyFileEx(const wstring& strSrcFile, const wstring& strDstFile
		, const CB_CopyFile& cb = NULL, const string& strHeadData = "");

    static bool fStat64(FILE *pf, tagFileStat64& stat);
    static bool lStat64(const wstring& strFile, tagFileStat64& stat);

    static long long GetFileSize64(const wstring& strFile);

    static time64_t GetFileModifyTime64(FILE *pf);
	static time64_t GetFileModifyTime64(const wstring& strFile);

    static void SplitPath(const wstring& strPath, wstring *pstrDir, wstring *pstrFile);
    static void SplitPath(const string& strPath, string *pstrDir, string *pstrFile);

	static wstring GetRootDir(const wstring& strPath);

    static wstring GetParentDir(const wstring& strPath);
    static string GetParentDir(const string& strPath);

	static wstring GetFileName(const wstring& strPath);
	static string GetFileName(const string& strPath);

	template <class S>
	static S getFileTitle(const S& strPath)
	{
		cauto strFileName = GetFileName(strPath);
		auto pos = strFileName.rfind(__chrDot);
		if (pos != strFileName.npos)
		{
			return strFileName.substr(0, pos);
		}

		return strFileName;
	}

	template <class S>
	static S GetFileExtName(const S& strFile)
	{
		auto pos = strFile.rfind(__chrDot);
		if (pos != S::npos)
		{
			return strFile.substr(pos+1);
		}

		return S();
	}

	static bool CheckSubPath(const wstring& strDir, const wstring& strSubPath);

	static wstring GetOppPath(const wstring& strPath, const wstring strBaseDir);

    static bool existPath(const wstring& strPath, bool bDir);
    static bool existPath(const string& strPath, bool bDir);

    static bool existDir(const wstring& strDir)
    {
        return existPath(strDir, true);
    }
    static bool existDir(const string& strDir)
    {
        return existPath(strDir, true);
    }

    static bool existFile(const wstring& strFile)
    {
        return existPath(strFile, false);
    }
    static bool existFile(const string& strFile)
    {
        return existPath(strFile, false);
    }

    static bool createDir(const wstring& strDir);
	static bool createDir(const string& strDir);

	static bool removeDirTree(const wstring& strDir);

    static bool removeDir(const wstring& strDir);
    static bool removeFile(const wstring& strFile);

	static bool moveFile(const wstring& strSrcFile, const wstring& strDstFile);

    static long fSeekTell(FILE *pf, long offset, int origin);
    static long long fSeekTell64(FILE *pf, long long offset, int origin);

    static bool setWorkDir(const string& strWorkDir);
    static string workDir();

#if __windows
	static string getModuleDir();
	static string getModuleDir(const char *pszModuleName);
	static wstring getModuleDir(const wchar_t *pszModuleName);

	static string getModuleSubPath(const string& strSubPath, const char *pszModuleName = NULL);
	static wstring getModuleSubPath(const wstring& strSubPath, const wchar_t *pszModuleName = NULL);
#endif

#if !__winvc
    static QString getHomeDir();

    static string getHomePath(const string& strSubDir);
    static wstring getHomePath(const wstring& strSubDir);
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
