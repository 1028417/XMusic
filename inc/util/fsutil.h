
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

#if !__winvc
#define _SH_DENYRW      0x10    // deny read/write mode
#define _SH_DENYWR      0x20    // deny write mode
#define _SH_DENYRD      0x30    // deny read mode
#define _SH_DENYNO      0x40    // deny none mode
#define _SH_SECURE      0x80    // secure mode
#endif
enum class E_ShareFalg
{
	 deny = _SH_DENYRW,
	 denyW = _SH_DENYWR,
	 denyR = _SH_DENYRD,
	 share = _SH_DENYNO,
	 secure = _SH_SECURE
};

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
#define __sdcardDir L"/sdcard/"

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
//#define lseek(fno, offset, origin) (long)lseek64(fno, offset, origin)

#else
#define fseek64 fseeko
#define ftell64 ftello
#endif

#define __fileTitle(file) file.erase(file.rfind('.'))
#define __fileTitle_r(file) file.substr(0, file.rfind('.'))

enum class E_FindFindFilter
{
	FFP_None
	, FFP_ByPrefix
	, FFP_ByExt
};

struct __UtilExt tagFileInfo
{
    tagFileInfo() = default;

    tagFileInfo(cwstr strDir, class CPath *pParent = NULL)
        : pParent(pParent)
        , bDir(true)
        , strName(strDir)
	{
	}

    tagFileInfo(class CPath& parent, cwstr strName, uint64_t uFileSize = 0)
        : pParent(&parent)
        , bDir(false)
        , strName(strName)
        , uFileSize(uFileSize)
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
#define __cPathSeparator    __cBackSlant
#else
#define __cPathSeparator    __cSlant
#endif
#define __wcPathSeparator   ((wchar_t)__cPathSeparator)

#define __wcDot L'.'

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
    inline static void appendPathTail(S& strPath)
    {
        if (!checkPathTail(strPath))
        {
            strPath.push_back(__cPathSeparator);
        }
    }
    template <class S>
    inline static S appendPathTail_r(const S& strPath)
    {
        if (!checkPathTail(strPath))
        {
            auto strRet = strPath;
            strRet.push_back(__cPathSeparator);
            return strRet;
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

#if __windows
	static FILE* fsopen(cwstr strFile, const string& strMode, E_ShareFalg _ShFlag = E_ShareFalg::denyW)
	{
		auto t_strMode = strutil::fromAsc(strMode);
		return _wfsopen(strFile.c_str(), t_strMode.c_str(), (int)_ShFlag);
	}
	static FILE* fsopen(const string& strFile, const string& strMode, E_ShareFalg _ShFlag = E_ShareFalg::denyW)
	{
		return _fsopen(strFile.c_str(), strMode.c_str(), (int)_ShFlag);
	}
#endif

    static FILE* fopen(cwstr strFile, const string& strMode);
	static FILE* fopen(const string& strFile, const string& strMode);

    static bool copyFile(cwstr strSrcFile, cwstr strDstFile);

#if __windows
    static bool copyFile(const string& strSrcFile, const string& strDstFile);
#endif

#if !__winvc
    static bool copyFile(cqstr qsSrcFile, cqstr qsDstFile)
    {
        return QFile::copy(qsSrcFile, qsDstFile);
    }
#endif

	using CB_CopyFile = function <bool(char *lpData, size_t size)>;
    static bool copyFileEx(cwstr strSrcFile, cwstr strDstFile
		, const CB_CopyFile& cb = NULL, const string& strHeadData = "");

    static bool fStat64(FILE *pf, tagFileStat64& stat);
    static bool lStat64(cwstr strFile, tagFileStat64& stat);

    static int64_t GetFileSize64(cwstr strFile);

    static time64_t GetFileModifyTime64(FILE *pf);
	static time64_t GetFileModifyTime64(cwstr strFile);

    static void SplitPath(cwstr strPath, wstring *pstrDir, wstring *pstrFile);
    static void SplitPath(const string& strPath, string *pstrDir, string *pstrFile);
	
    static wstring GetParentDir(cwstr strPath);
    static string GetParentDir(const string& strPath);

	static wstring GetFileName(cwstr strPath);
	static string GetFileName(const string& strPath);

	template <class S>
	static S GetFileExtName(const S& strFile)
	{
		auto pos = strFile.rfind('.');
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

	static bool MatchPath(cwstr strPath1, cwstr strPath2);
	static bool MatchPath(const string& strPath1, const string& strPath2);

	static bool CheckSubPath(cwstr strDir, cwstr strSubPath);
	static wstring GetOppPath(const wstring strBaseDir, cwstr strSubPath);

    static bool existPath(cwstr strPath, bool bDir);
    static bool existPath(const string& strPath, bool bDir);

    template <class S>
    static bool existDir(const S& strDir)
    {
        return existPath(strDir, true);
    }

    template <class S>
    static bool existFile(const S& strFile)
    {
        return existPath(strFile, false);
    }

    static bool createDir(cwstr strDir);
	static bool createDir(const string& strDir);

	static bool removeDirTree(cwstr strDir);

    static bool removeDir(cwstr strDir);
    static bool removeDir(const string& strDir);

    static bool removeFile(const string& strFile);
    static bool removeFile(cwstr strFile);

	static bool moveFile(cwstr strSrcFile, cwstr strDstFile, bool bReplaceExisting=false);

    static long fSeekTell(FILE *pf, long offset, int origin);
    static long long fSeekTell64(FILE *pf, long long offset, int origin);

    static bool setWorkDir(const string& strWorkDir);
    static string workDir();

#if __windows
	static string getModuleDir();
	static string getModuleDir(const char *pszModuleName);
	static wstring getModuleDir(const wchar_t *pszModuleName);

	static string getModuleSubPath(const string& strSubPath, const char *pszModuleName = NULL);
	static wstring getModuleSubPath(cwstr strSubPath, const wchar_t *pszModuleName = NULL);
#endif

#if !__winvc
    static QString getHomeDir();
#endif

	using CB_FindFile = cfn_void_t<tagFileInfo&>;
	static bool findFile(cwstr strDir, CB_FindFile cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wchar_t *pstrFilter = NULL);
	
        static bool findSubDir(cwstr strDir, CB_FindFile cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wchar_t *pstrFilter = NULL)
	{
		return findFile(strDir, [&](tagFileInfo& fileInfo) {
			if (fileInfo.bDir)
			{
                                cb(fileInfo);
			}
		}, eFilter, pstrFilter);
	}

        static bool findSubFile(cwstr strDir, CB_FindFile cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wchar_t *pstrFilter = NULL)
	{
		return findFile(strDir, [&](tagFileInfo& fileInfo) {
			if (!fileInfo.bDir)
			{
                                cb(fileInfo);
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
