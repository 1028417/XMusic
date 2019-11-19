#pragma once

struct __ModelExt tagFileTitle
{
	tagFileTitle() {}

	tagFileTitle(const wstring& t_strFileTitle);

	wstring strFileTitle;
	SSet<wstring> setFileTitle;
};

struct __ModelExt tagMediaResInfo
{
    tagMediaResInfo(){}

	tagMediaResInfo(const wstring& t_strPath, const wstring& t_strFileSize)
		: strPath(t_strPath)
		, FileTitle(fsutil::getFileTitle(strPath))
		, strFileSize(t_strFileSize)
	{
	}

	tagMediaResInfo(const wstring& t_strPath)
		: strPath(t_strPath)
		, FileTitle(fsutil::getFileTitle(strPath))
		, strFileSize(IMedia::GetFileSizeString(fsutil::GetFileSize64(strPath), false))
	{
	}

    bool operator==(const tagMediaResInfo& other) const
    {
        return &other == this;
    }

	wstring strPath;

	tagFileTitle FileTitle;

	wstring strFileSize;

	int nDuration = -1;
};

class __ModelExt CSearchMediaInfo
{
private:
	tagFileTitle m_FileTitle;

	wstring m_strFileSize;

	wstring m_strSingerName;

public:
	wstring m_strAbsPath;

	TD_MediaList m_lstMedias;

public:
	CSearchMediaInfo() {}

	CSearchMediaInfo(class CSearchMediaInfoGuard& SearchMediaInfoGuard, CMedia& media, CSinger *pSinger);

	wstring GetFileSize();

	bool matchMediaRes(tagMediaResInfo& MediaResInfo);

private:
	bool matchTitle(const wstring& strTitle1, const wstring& strTitle2);
	UINT matchTitle(const SSet<wstring>& setFileTitle1, const SSet<wstring>& setFileTitle2);
	UINT matchTitle(const wstring& strFileTitle1, const SSet<wstring>& setFileTitle2);
};

typedef map<wstring, CSearchMediaInfo> TD_SearchMediaInfoMap;

class __ModelExt CSearchMediaInfoGuard
{
public:
	CSearchMediaInfoGuard(CSingerMgr& SingerMgr);

private:
	CSingerMgr& m_SingerMgr;

	SSet<wstring> m_setSingerName;

public:
	void genSearchMediaInfo(CMedia& media, TD_SearchMediaInfoMap& mapSearchMediaInfo);

	inline static wstring transSingerName(const wstring& strSingerName);
	
	inline static bool matchSingerName(const wstring& strTitle, const wstring& strSingerName);

	wstring matchSingerName(const wstring& strTitle);
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
