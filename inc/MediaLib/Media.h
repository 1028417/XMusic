
#pragma once

#if __windows
#define __transSeparator(path)
#else
#define __transSeparator(path) fsutil::transSeparator(path)
#endif

struct __MediaLibExt tagMediaInfo
{
    tagMediaInfo() = default;

    tagMediaInfo(CMediaSet *pParent, cwstr strName, UINT uID = 0)
		: m_pParent(pParent)
		, m_strName(strName)
        , m_uID(uID)
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
    CMedia() = default;

    CMedia(CMediaSet *pParent, UINT uID, cwstr strPath, mediatime_t tTime
        , uint64_t uFileSize = 0, UINT uDuration = 0)
        : tagMediaInfo(pParent, L"", uID)
		, m_addTime(tTime)
		, m_nFileSize(uFileSize)
		, m_uDuration(uDuration)
	{
#if __winvc
		m_uDisplayFileSize = uFileSize;
		m_uDisplayDuration = uDuration;
#endif

        _UpdatePath(strPath);
	}

    virtual ~CMedia() = default;

protected:
	wstring m_strParentDir;
	
	CMediaTime m_addTime;

private:
        int64_t m_nFileSize = 0;
        UINT m_uDuration = 0;

#if __winvc
        uint64_t m_uDisplayFileSize = 0;
        UINT m_uDisplayDuration = 0;

#else
public:
        bool available = false;
#endif

public:
	int index() const;
	
	virtual wstring GetBaseDir() const
	{
		return L"";
	}
	
	wstring GetDir() const
	{
		return GetBaseDir() + m_strParentDir;
	}
	
	wstring GetPath() const override
	{
		return GetDir() + __wcPathSeparator + m_strName;
	}

    virtual wstring GetAbsPath() const override;

	wstring GetName() const override
	{
		return m_strName;
	}

	const CMediaTime& GetAddTime() const
	{
		return m_addTime;
	}

	bool notExist() const
	{
		return -1 == m_nFileSize;
	}

	uint64_t fileSize() const override
	{
		return m_nFileSize > 0 ? (uint64_t)m_nFileSize : 0;
	}

	void SetFileSize(int64_t nFileSize)
	{
		m_nFileSize = nFileSize;

#if __winvc
		if (nFileSize > 0)
		{
			m_uDisplayFileSize = (uint64_t)nFileSize;
		}
#endif
	}

    void SetDuration(UINT uDuration)
    {
        m_uDuration = uDuration;

#if __winvc
        if (uDuration > 0)
        {
            m_uDisplayDuration = m_uDuration;
        }
#endif
    }

    UINT duration() const override
    {
        return m_uDuration;
    }

#if !__winvc
    void SetFileSizeDuration(const CMedia& media) //合并提速
    {
        m_nFileSize = media.m_nFileSize;
        m_uDuration = media.m_uDuration;
    }
    void SetFileSizeDuration(uint64_t uFileSize, UINT uDuration) //合并提速
    {
        m_nFileSize = uFileSize;
        m_uDuration = uDuration;
    }

#else
	wstring displayFileSizeString(bool bIgnoreByte) const
	{
		return genFileSizeString(m_uDisplayFileSize, bIgnoreByte);
	}
	wstring displayDurationString() const
	{
		return genDurationString(m_uDisplayDuration);
	}
	uint64_t displayByteRate() const
	{
		if (0 == m_uDisplayDuration)
		{
			return 0;
		}

		return m_uDisplayFileSize / m_uDisplayDuration;
	}

	virtual wstring GetExportFileName() override
	{
		return GetName();
	}

	CRCueFile cueFile() override;
	
    void UpdatePath(cwstr strPath);
	
	virtual const CMedia* findRelatedMedia() { return NULL; }
#endif

	bool find(const struct tagFindMediaPara& FindPara, tagFindMediaResult& FindResult) const;

	CMediaSet* mediaSet() const override
	{
		return m_pParent;
	}

private:
    inline void _UpdatePath(cwstr strPath)
    {
        m_eFileType = getMediaFileType(strPath);

        fsutil::SplitPath(strPath, &m_strParentDir, &m_strName);

        __transSeparator(m_strParentDir);
    }

#if __winvc
    bool GetRenameText(wstring& stRenameText) const override
	{
		stRenameText = GetTitle();
		return true;
	}

	bool OnListItemRename(cwstr strNewName) override;
#endif
};
