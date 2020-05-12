
#pragma once

enum class E_MediaType
{
    MT_MediaRes,
    MT_PlayItem,
    MT_AlbumItem
};

enum class E_MediaFileType
{
    MFT_Null = 0,

	MFT_FLAC, MFT_MP3, MFT_WAV,

	MFT_DSF, MFT_DFF,

	MFT_APE, MFT_WMA, MFT_M4A,
	MFT_AAC, MFT_AC3,
	
	MFT_MP4, MFT_MKV,

	MFT_AIFF,
};

enum class E_MediaQuality
{
    MQ_None = 0,
    MQ_LQ,
    MQ_HQ,
    MQ_SQ,
    MQ_CD,
    MQ_HiRes
};

class __MediaLibExt IMedia
{
public:
	static cwstr GetMediaFileType(E_MediaFileType eMediaFileType);
	static E_MediaFileType GetMediaFileType(cwstr strExtName);

public:
	IMedia(E_MediaFileType eFileType = E_MediaFileType::MFT_Null);

	IMedia(cwstr strPath);

    virtual ~IMedia() = default;

protected:
	E_MediaFileType m_eFileType = E_MediaFileType::MFT_Null;

private:
	tagRelatedMediaSet m_lpRelatedMediaSet[4];

	inline tagRelatedMediaSet& _getRelatedMediaSet(E_MediaSetType eMediaSetType)
	{
		return m_lpRelatedMediaSet[(UINT)eMediaSetType];
	}

	inline const tagRelatedMediaSet& _getRelatedMediaSet(E_MediaSetType eMediaSetType) const
	{
		return m_lpRelatedMediaSet[(UINT)eMediaSetType];
	}

public:
	virtual wstring GetPath() const = 0;
	virtual wstring GetAbsPath() const = 0;

	virtual wstring GetName() const = 0;

    virtual wstring GetTitle() const;

    virtual E_MediaType type() const = 0;

	virtual class CMediaSet* GetMediaSet() const
	{
		return NULL;
	}

	bool isXmsc() const
	{
#if __winvc
		return false;
#else
		return E_MediaFileType::MFT_Null == m_eFileType;
#endif
	}

	E_MediaFileType GetFileType() const
	{
		return m_eFileType;
	}

	cwstr GetExtName() const;

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

    E_MediaQuality quality() const;

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

	cwstr GetRelatedMediaSetName(E_MediaSetType eMediaSetType) const
	{
		return _getRelatedMediaSet(eMediaSetType).strMediaSetName;
	}

	void SetRelatedMediaSet(E_MediaSetType eMediaSetType, UINT uMediaSetID, cwstr strMediaSetName, int iMediaID = -1);

	void SetRelatedMediaSet(CMediaSet& MediaSet);

	void SetRelatedMediaSet(CMedia& media);

	void ClearRelatedMediaSet(E_MediaSetType eMediaSetType);
	
	CMedia *findRelatedMedia(E_MediaSetType eMediaSetType);

protected:
	void UpdateFileType(cwstr strPath);
};
