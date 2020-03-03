
#pragma once

enum class E_MediaFileType
{
	MFT_Null = 0,
#if !__winvc
	MFT_XMSC,
#endif
	MFT_FLAC, MFT_MP3, MFT_WAV,
#if __winvc
	MFT_DSF, MFT_DFF,
#endif
	MFT_APE, MFT_WMA, MFT_M4A,
	MFT_AAC, MFT_AC3,
	
	MFT_MP4, MFT_MKV
};

class __MediaLibExt IMedia
{
	static const wstring g_strEraseChars;
public:
    static void transTitle(wstring& strFileTitle)
    {
		strutil::eraseChars(strFileTitle, g_strEraseChars);

		for (auto pstr : { L"mqms2", L"-163" })
		{
			strutil::replace(strFileTitle, pstr);
		}

		strutil::trim(strFileTitle);
		strutil::lowerCase(strFileTitle);
    }

    static wstring transTitle_r(const wstring& strFileTitle)
    {
        auto strRet = strFileTitle;
        transTitle(strRet);
        return strRet;
    }

	static const wstring& GetMediaFileType(E_MediaFileType eMediaFileType);
	static E_MediaFileType GetMediaFileType(const wstring& strExtName);

public:
	IMedia(E_MediaFileType eFileType = E_MediaFileType::MFT_Null);

	IMedia(const wstring& strPath);

	virtual ~IMedia() {}

protected:
	E_MediaFileType m_eFileType = E_MediaFileType::MFT_Null;

private:
	tagRelatedMediaSet m_lpRelatedMediaSet[4];

	tagRelatedMediaSet& _getRelatedMediaSet(E_MediaSetType eMediaSetType)
	{
		return m_lpRelatedMediaSet[(UINT)eMediaSetType];
	}

	const tagRelatedMediaSet& _getRelatedMediaSet(E_MediaSetType eMediaSetType) const
	{
		return m_lpRelatedMediaSet[(UINT)eMediaSetType];
	}

public:
	virtual wstring GetPath() const = 0;
	virtual wstring GetAbsPath() const = 0;

	virtual bool IsDir() const
	{
		return false;
	}

	virtual wstring GetName() const = 0;

	E_MediaFileType GetFileType() const
	{
		return m_eFileType;
	}

	bool isXmsc() const
	{
#if __winvc
		return false;
#else
		return E_MediaFileType::MFT_XMSC == m_eFileType;
#endif
	}

    const wstring& GetFileTypeString() const;

	wstring GetTitle() const;

	virtual long long fileSize() const
	{
		return -1;
	}

	wstring fileSizeString(bool bIgnoreByte)
	{
		return genFileSizeString(fileSize(), bIgnoreByte);
	}

	static wstring genFileSizeString(long long nFileSize, bool bIgnoreByte);

	virtual UINT duration() const
	{
		return 0;
	}

	wstring durationString() const
	{
		return genDurationString(duration());
	}

	static wstring genDurationString(int nDuration);

	virtual CMediaSet *GetMediaSet()
	{
		return NULL;
	}

	E_MediaSetType GetMediaSetType();

	virtual wstring GetExportFileName()
	{
		return GetName();
	}

	bool UpdateRelatedMediaSet(const tagMediaSetChanged& MediaSetChanged);

	UINT GetRelatedMediaID(E_MediaSetType eMediaSetType) const
	{
		return _getRelatedMediaSet(eMediaSetType).uMediaID;
	}

	UINT GetRelatedMediaSetID(E_MediaSetType eMediaSetType) const
	{
		return _getRelatedMediaSet(eMediaSetType).uMediaSetID;
	}

	const wstring& GetRelatedMediaSetName(E_MediaSetType eMediaSetType) const
	{
		return _getRelatedMediaSet(eMediaSetType).strMediaSetName;
	}

	void SetRelatedMediaSet(E_MediaSetType eMediaSetType, UINT uMediaSetID, const wstring& strMediaSetName, int iMediaID = -1);

	void SetRelatedMediaSet(CMediaSet& MediaSet);

	void SetRelatedMediaSet(CMedia& media);

	void ClearRelatedMediaSet(E_MediaSetType eMediaSetType);
	
	void ShellExplore(bool bSel=true);

	CMedia *findRelatedMedia(E_MediaSetType eMediaSetType);

protected:
	void UpdateFileType(const wstring& strPath);
};
