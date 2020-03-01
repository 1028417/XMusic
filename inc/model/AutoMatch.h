#pragma once

class __ModelExt CSingerNameGuard
{
public:
	CSingerNameGuard(class CSingerMgr& SingerMgr);

	class CSingerMgr& m_SingerMgr;

private:
	list<wstring> m_lstSingerName;

public:
	wstring matchSingerName(const wstring& strTitle);
};

struct __ModelExt tagFileTitle
{
	tagFileTitle() {}

	tagFileTitle(const wstring& t_strFileTitle)
		: m_strFileTitle(IMedia::transTitle(t_strFileTitle))
	{
		vector<wstring> vecFileTitle;
		strutil::split(m_strFileTitle, L'-', vecFileTitle, true);
		m_setFileTitle.add(vecFileTitle);
	}

	wstring m_strFileTitle;
	SSet<wstring> m_setFileTitle;
};

struct __ModelExt tagMediaResInfo
{
    tagMediaResInfo(){}
	
	tagMediaResInfo(const wstring& strPath, const wstring& strFileSize = L"")
		: m_strPath(strPath)
		, m_FileTitle(fsutil::getFileTitle(m_strPath))
	{
		if (strFileSize.empty())
		{
			m_strFileSize = IMedia::genFileSizeString(fsutil::GetFileSize64(m_strPath), false);
		}
		else
		{
			m_strFileSize = strFileSize;
		}

		auto fileTime = fsutil::GetFileModifyTime64(m_strPath);
		if (-1 != fileTime)
		{
			m_strFileTime = __mediaTimeFormat(fileTime);
		}
	}

    bool operator==(const tagMediaResInfo& other) const
    {
        return &other == this;
    }

	wstring m_strPath;

	tagFileTitle m_FileTitle;

	wstring m_strFileSize;
	wstring m_strFileTime;

	int m_nDuration = -1;
};

typedef map<wstring, class CSearchMediaInfo> TD_SearchMediaInfoMap;

class __ModelExt CSearchMediaInfo
{
public:
	static void genSearchMediaInfo(CSingerNameGuard& SingerNameGuard, CMedia& media, TD_SearchMediaInfoMap& mapSearchMediaInfo);

	CSearchMediaInfo() {}

	CSearchMediaInfo(CMedia& media, CSingerNameGuard& SingerNameGuard, CSinger *pSinger);

private:
	tagFileTitle m_FileTitle;

	wstring m_strFileSize;
	wstring m_strFileTime;

	wstring m_strSingerName;

public:
	wstring m_strPath;

	TD_MediaList m_lstMedias;

public:
	wstring GetFileSize();
	wstring GetFileTime();

	bool matchMediaRes(tagMediaResInfo& MediaResInfo);

private:
	bool matchTitle(const wstring& strTitle1, const wstring& strTitle2);
	UINT matchTitle(const SSet<wstring>& setFileTitle1, const SSet<wstring>& setFileTitle2);
	UINT matchTitle(const wstring& strFileTitle1, const SSet<wstring>& setFileTitle2);
};

enum class E_MatchResult
{
	MR_Yes
	, MR_No
	, MR_Ignore
};

using CB_AutoMatchConfirm = function<E_MatchResult(CSearchMediaInfo&, tagMediaResInfo&)>;

using CB_AutoMatchProgress = function<bool(const wstring& strDir)>;

class __ModelExt CAutoMatch
{
public:
	CAutoMatch(CModel& model, const CB_AutoMatchProgress& cbProgress, const CB_AutoMatchConfirm& cbConfirm)
		: m_model(model)
		, m_cbProgress(cbProgress)
		, m_cbConfirm(cbConfirm)
	{
	}

private:
	CModel& m_model;

	CB_AutoMatchProgress m_cbProgress;
	CB_AutoMatchConfirm m_cbConfirm;

	map<wstring, TD_SearchMediaInfoMap> m_mapSearchMedia;
	
public:
	void autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias);

private:
	void FilterBasePath(CMediaRes& SrcPath, CMediaRes *pDir, list<wstring>& lstSubPaths
		, list<wstring>& lstPaths, list<wstring>& lstNewSubPaths);

	void enumMediaRes(CMediaRes& SrcPath, CMediaRes *pDir, list<wstring>& lstPaths, list<wstring>& lstSubPaths);

    void matchMedia(tagMediaResInfo& MediaResInfo, TD_SearchMediaInfoMap& mapSearchMedias);
};
