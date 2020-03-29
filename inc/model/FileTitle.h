#pragma once

class __ModelExt CFileTitleGuard
{
public:
	CFileTitleGuard(class CSingerMgr& SingerMgr);
	
private:
	class CSingerMgr& m_SingerMgr;

	PairList<wstring, wstring> m_plSingerInfo;

private:
	wstring _matchSinger(wstring& strFileTitle);

public:
	static wstring genCollateSingerName(const wstring& strSingerName);

	static void genCollateTitle(wstring& strFileTitle);
	static wstring genCollateTitle_r(const wstring& strFileTitle);

	static void genDisplayTitle(wstring& strPath);
	static wstring genDisplayTitle_r(const wstring& strFileTitle);

	wstring matchSinger(const wstring& strFileTitle);
	wstring matchSinger(IMedia& media);
};


class __ModelExt CFileTitle
{
private:
	void _parseTitle();

public:
    CFileTitle() = default;
	CFileTitle(const wstring& strPath, const wstring& strSingerName);
	CFileTitle(IMedia& media, const wstring& strSingerName);

public:
	wstring m_strPath;

	wstring m_strTitle;

	SSet<wstring> m_setTitle;

	wstring m_strSingerName;
	bool m_bContainsSingerName = false;

public:
	wstring dir() const
	{
		return fsutil::GetParentDir(m_strPath);
	}

	wstring fileName() const
	{
		return fsutil::GetFileName(m_strPath);
	}

	bool match(const CFileTitle& other) const;
	UINT compare(const CFileTitle& other) const;
};
