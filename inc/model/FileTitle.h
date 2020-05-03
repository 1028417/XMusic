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
	static wstring genCollateSingerName(cwstr strSingerName);

	static void genCollateTitle(wstring& strFileTitle);
	static wstring genCollateTitle_r(cwstr strFileTitle);

	static void genDisplayTitle(wstring& strPath);
	static wstring genDisplayTitle_r(cwstr strFileTitle);

	wstring matchSinger(cwstr strFileTitle);
	wstring matchSinger(IMedia& media);
};


class __ModelExt CFileTitle
{
private:
	void _parseTitle();

public:
    CFileTitle() = default;
	CFileTitle(cwstr strPath, cwstr strSingerName);
	CFileTitle(IMedia& media, cwstr strSingerName);

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
