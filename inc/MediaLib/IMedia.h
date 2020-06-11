
#pragma once

#define __mediaFileType(strPath) IMedia::GetMediaFileType(fsutil::GetFileExtName(strPath));

enum class E_MediaFileType
{
    MFT_Null = 0,

	MFT_MP3, MFT_FLAC, MFT_WAV,

	MFT_DSF, MFT_DFF,

	MFT_M4A, MFT_APE, MFT_AIFF,

	MFT_WMA, MFT_AC3, MFT_AAC,
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

enum class E_MediaType
{
    MT_MediaRes,
    MT_PlayItem,
    MT_AlbumItem
};

enum class E_RelatedMediaSet
{
        RMS_Playlist = (int)E_MediaSetType::MST_Playlist
        , RMS_Album = (int)E_MediaSetType::MST_Album
        , RMS_Singer = (int)E_MediaSetType::MST_Singer
};

class __MediaLibExt IMedia
{
public:
	static cwstr GetMediaFileType(E_MediaFileType eMediaFileType);
	static E_MediaFileType GetMediaFileType(cwstr strExtName);

public:
    IMedia() = default;

    IMedia(E_MediaFileType eFileType);

	virtual ~IMedia() = default;

protected:
	E_MediaFileType m_eFileType = E_MediaFileType::MFT_Null;

private:
	tagRelatedMediaSet m_lpRelatedMediaSet[UINT(E_RelatedMediaSet::RMS_Singer) + 1];

	inline tagRelatedMediaSet& _getRelatedMediaSet(E_RelatedMediaSet eRmsType)
	{
		return m_lpRelatedMediaSet[(UINT)eRmsType];
	}

	inline const tagRelatedMediaSet& _getRelatedMediaSet(E_RelatedMediaSet eRmsType) const
	{
		return m_lpRelatedMediaSet[(UINT)eRmsType];
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

	CMedia *findRelatedMedia(E_RelatedMediaSet eRmsType);

	UINT GetRelatedMediaID(E_RelatedMediaSet eRmsType) const
	{
		return _getRelatedMediaSet(eRmsType).uMediaID;
	}

	UINT GetRelatedMediaSetID(E_RelatedMediaSet eRmsType) const
	{
		return _getRelatedMediaSet(eRmsType).uMediaSetID;
	}

	cwstr GetRelatedMediaSetName(E_RelatedMediaSet eRmsType) const
	{
		return _getRelatedMediaSet(eRmsType).strMediaSetName;
	}

	void SetRelatedMediaSet(E_RelatedMediaSet eRmsType, UINT uMediaSetID, cwstr strMediaSetName, int iMediaID = -1);

	void SetRelatedMediaSet(E_RelatedMediaSet eRmsType, CMedia& media);

	void ClearRelatedMediaSet(E_RelatedMediaSet eRmsType)
	{
		_getRelatedMediaSet(eRmsType).clear();
	}

	bool UpdateRelatedMediaSet(E_RelatedMediaSet eRmsType, const tagMediaSetChanged& MediaSetChanged);
};
