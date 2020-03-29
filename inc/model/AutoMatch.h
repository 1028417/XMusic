#pragma once

class __ModelExt CMediaResInfo
{
public:
	CMediaResInfo() = default;

	CMediaResInfo(CMediaRes& MediaRes, const wstring& strSingerName)
		: m_FileTitle(MediaRes, strSingerName)
		, m_strFileSize(MediaRes.fileSizeString(false))
		, m_strFileTime(MediaRes.fileTimeString(false))
	{
	}

	CMediaResInfo(const wstring& strPath, const wstring& strSingerName)
		: m_FileTitle(strPath, strSingerName)
	{
		m_strFileSize = IMedia::genFileSizeString(fsutil::GetFileSize64(strPath), false);		
		m_strFileTime = CMediaTime::genFileTimeString(fsutil::GetFileModifyTime64(strPath), false);
	}

    bool operator==(const CMediaResInfo& other) const
    {
        return &other == this;
    }

	const CFileTitle* operator->() const
	{
		return &m_FileTitle;
	}

	UINT m_uDuration = 0;

private:
	CFileTitle m_FileTitle;

	wstring m_strFileSize;
	wstring m_strFileTime;

public:
	const CFileTitle& fileTitle() const
	{
		return m_FileTitle;
	}

	wstring fileSizeString() const
	{
		return m_strFileSize;
	}

	wstring fileTimeString() const
	{
		return m_strFileTime;
	}
};

class __ModelExt CMatchMediaInfo
{
public:
	CMatchMediaInfo() = default;

	CMatchMediaInfo(const wstring& strPath, CMedia& media, const wstring& strSingerName)
		: m_FileTitle(strPath, strSingerName)
		, m_lstMedias(media)
	{
	}

	CMatchMediaInfo(const wstring& strPath, TD_MediaList& lstMedias, const wstring& strSingerName)
		: m_FileTitle(strPath, strSingerName)
		, m_lstMedias(lstMedias)
	{
	}

	const CFileTitle* operator->() const
	{
		return &m_FileTitle;
	}

private:
	CFileTitle m_FileTitle;

	TD_MediaList m_lstMedias;

	wstring m_strFileSize;
	wstring m_strFileTime;

public:
	const CFileTitle& fileTitle() const
	{
		return m_FileTitle;
	}

	TD_MediaList& medias()
	{
		return m_lstMedias;
	}

	wstring fileSizeString();
	wstring fileTimeString();
};

enum class E_MatchResult
{
	MR_Yes
	, MR_No
	, MR_Ignore
};

using CB_AutoMatchConfirm = function<E_MatchResult(CMatchMediaInfo&, CMediaResInfo&)>;

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

	map<wstring, list<CMatchMediaInfo>> m_mapMatchMedia;
	
public:
	void autoMatchMedia(CMediaRes& SrcPath, const TD_MediaList& lstMedias);

private:
	void FilterBasePath(CMediaRes& SrcPath, CMediaRes *pDir, list<wstring>& lstSubPaths
		, list<wstring>& lstPaths, list<wstring>& lstNewSubPaths);

	void enumMediaRes(class CFileTitleGuard& FileTitleGuard, CMediaRes& SrcPath, CMediaRes *pDir, list<wstring>& lstPaths, list<wstring>& lstSubPaths);

    void matchMedia(CMediaResInfo& MediaResInfo, list<CMatchMediaInfo>& lstMatchMedias);
};
