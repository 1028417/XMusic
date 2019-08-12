
#pragma once

#define MAX_PATH 260

#if !__winvc
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTime>
#endif

#include <sys/utime.h>
#include <sys/stat.h>

#if __android
using tagFileStat = struct stat;
using tagFileStat32 = struct stat;
using tagFileStat32_64 = struct stat;
using tagFileStat64_32 = struct stat;
using tagFileStat64 = struct stat;

#define _fileno fileno
#else
using tagFileStat = struct _stat;
using tagFileStat32 = struct _stat32;
using tagFileStat32_64 = struct _stat32i64;
using tagFileStat64_32 = struct _stat64i32;
using tagFileStat64 = struct _stat64;
#endif

enum class E_SeekFileFlag
{
	SFF_Set = SEEK_SET,
	SFF_Cur = SEEK_CUR,
	SFF_End = SEEK_END
};

struct tagFileInfo
{
	tagFileInfo()
	{
	}

	tagFileInfo(const tagFileInfo& FileInfo)
	{
		*this = FileInfo;
	}

	bool m_bDir = false;

	wstring m_strName;

	unsigned long m_uFileSize = 0;

	time64_t m_tCreateTime = 0;
	time64_t m_tModifyTime = 0;
};

#if __android
#define __wcFSSlant fsutil::wcSlant
#else
#define __wcFSSlant fsutil::wcBackSlant
#endif

#define __wcDot fsutil::wcDot

class __UtilExt fsutil
{
public:
	static const wchar_t wcDot = L'.';
	static const wchar_t wcBackSlant = L'\\';
	static const wchar_t wcSlant = L'/';

	static bool loadBinary(const wstring& strFile, vector<char>& vecData, UINT uReadSize = 0);

	static bool loadTxt(const wstring& strFile, string& strText);
	static bool loadTxt(const wstring& strFile, const function<bool(const string&)>& cb);
	static bool loadTxt(const wstring& strFile, SVector<string>& vecLineText);

    static bool copyFile(const wstring& strSrcFile, const wstring& strDstFile);
    static bool copyFileEx(const wstring& strSrcFile, const wstring& strDstFile
		, bool bSyncModifyTime = false, const char *lpFileHead = NULL, size_t uHeadSize = 0);

	static bool fileStat(FILE *lpFile, tagFileStat& stat);
	static bool fileStat(const wstring& strFile, tagFileStat& stat);

	static bool fileStat32(FILE *lpFile, tagFileStat32& stat);
	static bool fileStat32(const wstring& strFile, tagFileStat32& stat);
	static bool fileStat32_64(FILE *lpFile, tagFileStat32_64& stat);
	static bool fileStat32_64(const wstring& strFile, tagFileStat32_64& stat);

	static bool fileStat64(FILE *lpFile, tagFileStat64& stat);
	static bool fileStat64(const wstring& strFile, tagFileStat64& stat);
	static bool fileStat64_32(FILE *lpFile, tagFileStat64_32& stat);
	static bool fileStat64_32(const wstring& strFile, tagFileStat64_32& stat);

	static int GetFileSize(FILE *lpFile);
	static int GetFileSize(const wstring& strFile);
	static int64_t GetFileSize64(FILE *lpFile);
	static int64_t GetFileSize64(const wstring& strFile);

	static time32_t GetFileModifyTime(FILE *lpFile);
	static time32_t GetFileModifyTime(const wstring& strFile);
	static time64_t GetFileModifyTime64(FILE *lpFile);
	static time64_t GetFileModifyTime64(const wstring& strFile);

	static void SplitPath(const wstring& strPath, wstring *pstrDir, wstring *pstrFile);

	static wstring GetRootDir(const wstring& strPath);
	static wstring GetParentDir(const wstring& strPath);

	static wstring GetFileName(const wstring& strPath);
	static wstring getFileTitle(const wstring& strPath);
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

    static int64_t seekFile(FILE *lpFile, int64_t offset, E_SeekFileFlag eFlag=E_SeekFileFlag::SFF_Set);

#if !__android
	static wstring getModuleDir(wchar_t *pszModuleName = NULL);
#endif

    static wstring workDir();
    static bool setWorkDir(const wstring& strWorkDir);

	enum class E_FindFindFilter
	{
		FFP_None
		, FFP_ByPrefix
		, FFP_ByExt
	};

	template <typename CB, typename = checkCBVoid_t<CB, const tagFileInfo&>>
	static bool findFile(const wstring& strDir, const CB& cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wstring& strFilter = L"")
	{
		return _findFile(strDir, [&](const tagFileInfo& fileInfo) {
			cb(fileInfo);
			return true;
		}, eFilter, strFilter);
	}

	template <typename CB, typename = checkCBBool_t<CB, const tagFileInfo&>, typename=void>
	static bool findFile(const wstring& strDir, const CB& cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wstring& strFilter = L"")
	{
		return _findFile(strDir, [&](const tagFileInfo& fileInfo) {
            return cb(fileInfo);
		}, eFilter, strFilter);
	}

private:
    using CB_FindFile = const function<bool(const tagFileInfo&)>&;
    static bool _findFile(const wstring& strDir, CB_FindFile cb, E_FindFindFilter eFilter, const wstring& strFilter);
};

#include <fstream>

class ibstream : public ifstream
{
public:
	~ibstream()
	{
		close();
	}

	ibstream() {}

	ibstream(const wstring& strFile)
	{
		open(strFile);
	}

	void open(const wstring& strFile)
	{
		ifstream::open(
#if __winvc
			strFile
#else
			wsutil::toStr(strFile)
#endif
			, ios_base::binary);
	}
};

class obstream : public ofstream
{
public:
	~obstream()
	{
		close();
	}

	obstream() {}

	obstream(const wstring& strFile, bool bTrunc)
	{
		open(strFile, bTrunc);
	}

	void open(const wstring& strFile, bool bTrunc)
	{
        auto mode = bTrunc ? ios_base::trunc | ios_base::binary : ios_base::binary;

		ofstream::open(
#if __winvc
			strFile
#else
			wsutil::toStr(strFile)
#endif
			, mode);
	}
};
