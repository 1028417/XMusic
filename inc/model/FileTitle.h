#pragma once

class __ModelExt CSingerMatcher
{
public:
	CSingerMatcher(class CSingerMgr& SingerMgr);
	
private:
    PairList<wstring, wstring> m_plSingerInfo;

public:
    wstring matchSinger(cwstr strCollateTitle, cwstr strPath) const;
};


class __ModelExt CFileTitle
{
public:
    static bool simplifiedTrans(wstring& strTitle);

	static wstring genCollateSingerName(cwstr strSingerName);

    static bool genCollateTitle(wstring& strFileTitle);
	static wstring genCollateTitle_r(cwstr strFileTitle);

    static bool formatFileTitle(wstring& strFileTitle, const wstring *pstrSingerName=NULL);

#if !__winvc
    static void genDisplayTitle(wstring& strFileTitle, const wstring *pstrSingerName=NULL);
#endif

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
