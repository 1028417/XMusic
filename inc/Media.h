
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
	//friend class __model;

public:
	CMedia(CMediaSet *pParent = NULL, int nID = 0, const wstring& strPath = L"", mediatime_t tTime = 0)
		: IMedia(strPath)
		, tagMediaInfo(pParent, L"", nID)
		, m_addTime(tTime)
	{
		this->_UpdatePath(strPath);
	}

	virtual ~CMedia()
	{
	}

protected:
	wstring m_strParentDir;
	
	CMediaTime m_addTime;

	int m_nFileSize = 0;

	UINT m_uDuration = 0;

public:
	wstring GetBaseDir() const;
	
	wstring GetDir() const
	{
		return GetBaseDir() + m_strParentDir;
	}
	
	wstring GetPath() const override
	{
		return GetDir() + __wcBackSlant + m_strName;
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

	int CheckFileSize()
	{
		return m_nFileSize = fsutil::GetFileSize(this->GetAbsPath());
	}

	int GetFileSize() const override
	{
		return m_nFileSize;
	}

	void SetDuration(UINT uDuration)
	{
		m_uDuration = uDuration;
	}

	UINT GetDuration() const
	{
		return m_uDuration;
	}

	wstring GetDurationString() const;

	static wstring GetDurationString(UINT uDuration);

	//void UpdateFileInfo(int iFileSize, int nDuration);

	CMediaSet *GetMediaSet() override
	{
		return m_pParent;
	}

	virtual wstring GetExportFileName() override
	{
		return GetTitle();
	}
	
	void UpdatePath(const wstring& strPath);
	
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
