
#pragma once

#define MAX_PATH 260

#ifndef _MSC_VER
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QTime>
#endif

#ifdef __ANDROID__
#include <unistd.h>
#define __fseek(f, offset, whence) lseek64((int)f, offset, whence)
#else
#define __fseek _fseeki64
#endif

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

#define __wcDot fsutil::wcDot
#define __wcBackSlant fsutil::wcBackSlant
#define __wcSlant fsutil::wcSlant

class __UtilExt fsutil
{
public:
	static const wchar_t wcDot = L'.';
	static const wchar_t wcBackSlant = L'\\';
	static const wchar_t wcSlant = L'/';

	static bool loadBinary(const wstring& strFile, vector<char>& vecData, UINT uReadSize = 0);

	static bool loadTxt(const wstring& strFile, string& strText);
	static bool loadTxt(const wstring& strFile, const function<bool(const string&)>& cb, char cdelimiter = '\n');
	static bool loadTxt(const wstring& strFile, SVector<string>& vecLineText, char cdelimiter = '\n');

	static bool copyFile(const wstring& strSrcFile, const wstring& strDstFile, bool bSyncModifyTime = false);

	static int GetFileSize(const wstring& strFile);
	static time64_t GetFileModifyTime(const wstring& strFile);

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

#ifndef __ANDROID__
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
    using CB_FindFile = const function<bool(const tagFileInfo&)>&;
    static bool findFile(const wstring& strDir, CB_FindFile cb
		, E_FindFindFilter eFilter = E_FindFindFilter::FFP_None, const wstring& strFilter = L"");
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
#ifdef _MSC_VER
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
#ifdef _MSC_VER
			strFile
#else
			wsutil::toStr(strFile)
#endif
			, mode);
	}
};
