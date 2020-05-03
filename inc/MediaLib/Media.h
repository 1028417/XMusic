
#pragma once

struct __MediaLibExt tagMediaInfo
{
    tagMediaInfo() = default;

	tagMediaInfo(CMediaSet *pParent, cwstr strName, int nID = 0)
		: m_pParent(pParent)
		, m_strName(strName)
		, m_uID(nID)
	{
	}

	CMediaSet *m_pParent = NULL;
	wstring m_strName;
	UINT m_uID = 0;
};

class __MediaLibExt CMedia : public IMedia, public tagMediaInfo
#if __winvc
	, public CListObject
#endif
{
public:
	CMedia(CMediaSet *pParent = NULL, int nID = 0, cwstr strPath = L"", filetime_t tTime = 0)
		: IMedia(strPath)
		, tagMediaInfo(pParent, L"", nID)
		, m_addTime(tTime)
	{
		_UpdatePath(strPath);

#if !__windows
        fsutil::transSeparator(m_strParentDir);
#endif
	}

    virtual ~CMedia() = default;

protected:
	wstring m_strParentDir;
	
	CMediaTime m_addTime;

private:
	UINT m_uDuration = 0;
	long long m_nFileSize = 0;
	
#if __winvc
	UINT m_uDisplayDuration = 0;
	long long m_nDisplayFileSize = 0;
#endif

public:
	int index() const;
	
	wstring GetBaseDir() const;
	
	wstring GetDir() const
	{
		return GetBaseDir() + m_strParentDir;
	}
	
	wstring GetPath() const override
	{
		return GetDir() + __wcPathSeparator + m_strName;
	}

	wstring GetAbsPath() const override;

	wstring GetName() const override
	{
		return m_strName;
	}

	const CMediaTime& GetAddTime() const
	{
		return m_addTime;
	}

	UINT checkDuration();

	void SetDuration(UINT uDuration)
	{
		m_uDuration = uDuration;
	}

	void SetDuration(UINT uDuration, long long nFileSize);

	UINT duration() const override
	{
		return m_uDuration;
	}

#if __winvc
	UINT displayDuration() const
	{
		return m_uDisplayDuration;
	}
	wstring displayDurationString() const
	{
		return genDurationString(m_uDisplayDuration);
	}

	long long displayFileSize() const
	{
		return m_nDisplayFileSize;
	}
	wstring displayFileSizeString(bool bIgnoreByte) const
	{
		return genFileSizeString(m_nDisplayFileSize, bIgnoreByte);
	}
#endif

	long long fileSize() const override
	{
		return m_nFileSize;
	}

	void SetFileSize(long long nFileSize)
	{
		m_nFileSize = nFileSize;
	}

	CMediaSet *GetMediaSet() override
	{
		return m_pParent;
	}

	virtual wstring GetExportFileName() override
	{
		return GetName();
	}

    virtual void AsyncTask() {}

#if __winvc
    CRCueFile getCueFile() const;

    void UpdatePath(cwstr strPath);
#endif

private:
    inline void _UpdatePath(cwstr strPath)
    {
        fsutil::SplitPath(strPath, &m_strParentDir, &m_strName);
    }

#if __winvc
    bool GetRenameText(wstring& stRenameText) const override
	{
		stRenameText = GetTitle();
		return true;
	}

	void OnListItemRename(cwstr strNewName) override;
#endif
};
