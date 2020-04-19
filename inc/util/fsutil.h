
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

#define __fileTitle(fileName) fileName.erase(fileName.rfind(__cDot))
#define __fileTitle_r(fileName) fileName.substr(0, fileName.rfind(__cDot))

enum class E_FindFindFilter
{
	FFP_None
	, FFP_ByPrefix
	, FFP_ByExt
};

struct __UtilExt tagFileInfo
{
    tagFileInfo() = default;

	tagFileInfo(const wstring& strDirPath)
        : strName(strDirPath)
	{
	}

	tagFileInfo(bool t_bDir, const wstring& t_strName, class CPath *t_pParent=NULL)
		: pParent(t_pParent)
		, bDir(t_bDir)
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


#define __cSlant '/'
#define __cBackSlant '\\'

#if __windows
    #define __cPathSeparator	__cBackSlant
#else
    #define __cPathSeparator	__cSlant
#endif
#define     __wcPathSeparator	((wchar_t)__cPathSeparator)

#define __cDot '.'
#define __wcDot (wchar_t)__cDot

class __UtilExt fsutil
{
public:
	inline static bool checkSeparator(wchar_t wch)
	{
        return (wchar_t)__cBackSlant == wch || (wchar_t)__cSlant == wch;
	}
	inline static bool checkSeparator(char chr)
	{
        return __cBackSlant == chr || __cSlant == chr;
	}

	template <class S>
	inline static bool checkPathTail(const S& strPath)
	{
		if (strPath.empty())
		{
			return false;
		}

		return checkSeparator(strPath.back());
	}

    template <class S>
    inline static void trimPathTail(S& strPath)
    {
		if (checkPathTail(strPath))
		{
            strPath.pop_back();
        }
    }

    template <class S>
    inline static S trimPathTail_r(const S& strPath)
	{
		if (checkPathTail(strPath))
		{
			return strPath.substr(0, strPath.size()-1);
		}

		return strPath;
	}
	
    template <class S>
    inline static void transSeparator(S& strPath)
    {
#if __windows
        strutil::replaceChar(strPath, __cSlant, __cBackSlant);
#else
        strutil::replaceChar(strPath, __cBackSlant, __cSlant);
#endif
    }

    template <class S>
	inline static S transSeparator_r(const S& strPath)
    {
        auto t_strPath = strPath;
        transSeparator(t_strPath);
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
	
    static wstring GetParentDir(const wstring& strPath);
    static string GetParentDir(const string& strPath);

	static wstring GetFileName(const wstring& strPath);
	static string GetFileName(const string& strPath);

	template <class S>
	static S GetFileExtName(const S& strFile)
	{
		auto pos = strFile.rfind(__cDot);
		if (pos != S::npos)
		{
			return strFile.substr(pos + 1);
		}

		return S();
	}

	template <class S>
	static S getFileTitle(const S& strPath)
	{
		cauto strFileName = GetFileName(strPath);
		return __fileTitle_r(strFileName);
	}

	static bool CheckSubPath(const wstring& strDir, const wstring& strSubPath);
	static wstring GetOppPath(const wstring strBaseDir, const wstring& strSubPath);

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
    static QString getHomePath(const QString& qsSubDir);
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
