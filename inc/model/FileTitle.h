#pragma once

class __ModelExt CSingerMatcher
{
public:
	CSingerMatcher(class CSingerMgr& SingerMgr);
	
private:
	class CSingerMgr& m_SingerMgr;

    PairList<wstring, wstring> m_plSingerInfo;
    map<wstring, wstring> m_mapSingerName;

private:
	wstring _matchSinger(wstring& strFileTitle) const;

public:
    wstring checkSinger(const wstring& strSingerName) const;
	
    wstring matchSinger(cwstr strFileTitle) const;
	wstring matchSinger(cwstr strFileTitle, cwstr strPath) const;
};


class __ModelExt CFileTitle
{
public:
	static wstring genCollateSingerName(cwstr strSingerName);

	static void genCollateTitle(wstring& strFileTitle);
	static wstring genCollateTitle_r(cwstr strFileTitle);

    static bool formatFileTitle(wstring& strFileTitle, const wchar_t *pszSingerName=NULL);
    static void genDisplayName(wstring& strFileName, const wchar_t *pszSingerName=NULL);

private:
	void _parseTitle(const CSingerMatcher& SingerMatcher);

public:
    CFileTitle() = default;
	CFileTitle(cwstr strPath, const CSingerMatcher& SingerMatcher);
	CFileTitle(IMedia& media, const CSingerMatcher& SingerMatcher);

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
