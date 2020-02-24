
#pragma once

struct __MediaLibExt tagMediaInfo
{
	tagMediaInfo() {}

	tagMediaInfo(CMediaSet *pParent, const wstring& strName, int nID = 0)
		: m_pParent(pParent)
		, m_strName(strName)
		, m_uID(nID)
	{
	}

	CMediaSet *m_pParent = NULL;
	wstring m_strName;
	UINT m_uID = 0;
};

class __MediaLibExt CMedia : public IMedia, public tagMediaInfo, public CListObject
{
public:
	CMedia(CMediaSet *pParent = NULL, int nID = 0, const wstring& strPath = L"", mediatime_t tTime = 0)
		: IMedia(strPath)
		, tagMediaInfo(pParent, L"", nID)
		, m_addTime(tTime)
	{
		_UpdatePath(strPath);
	}

	virtual ~CMedia()
	{
	}

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
		return GetDir() + __wchDirSeparator + m_strName;
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

	static wstring genDurationString(int nDuration);
	wstring durationString() const;

#if __winvc
	wstring displayDurationString()
	{
		return genDurationString(m_uDisplayDuration);
	}

	wstring displayFileSizeString()
	{
		return genFileSizeString(m_nDisplayFileSize);
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
		return GetTitle();
	}
	
	void UpdatePath(const wstring& strPath);
	
	virtual void AsyncTask() {}

#if __winvc
	CRCueFile getCueFile() const;
#endif

private:
	void _UpdatePath(const wstring& strPath);

	bool GetRenameText(wstring& stRenameText) const override
	{
		stRenameText = GetTitle();
		return true;
	}

	void OnListItemRename(const wstring& strNewName) override;
};
