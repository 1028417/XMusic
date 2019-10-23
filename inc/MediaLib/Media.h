
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

	int m_nFileSize = 0;

	int m_nDuration = -1;

public:
	int index() const;
	
	wstring GetBaseDir() const;
	
	wstring GetDir() const
	{
		return GetBaseDir() + m_strParentDir;
	}
	
	wstring GetPath() const override
	{
		return GetDir() + __wcFSSlant + m_strName;
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

	void SetDuration(UINT uDuration)
	{
		m_nDuration = uDuration;
	}

	int GetDuration() const
	{
		return m_nDuration;
	}

	wstring GetDurationString() const;

	static wstring GetDurationString(int nDuration);

	CMediaSet *GetMediaSet() override
	{
		return m_pParent;
	}

	virtual wstring GetExportFileName() override
	{
		return GetTitle();
	}
	
	void UpdatePath(const wstring& strPath);

    CMediaRes *GetMediaRes() const override;

	CRCueFile getCueFile() const;

	virtual void AsyncTask() override;

private:
	void _UpdatePath(const wstring& strPath);

	bool GetRenameText(wstring& stRenameText) const override
	{
		stRenameText = GetTitle();
		return true;
	}

	void OnListItemRename(const wstring& strNewName) override;
};
