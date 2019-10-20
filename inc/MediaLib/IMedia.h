
#pragma once

enum class E_MediaFileType
{
	MFT_Null = 0
	
	, MFT_MP3
	, MFT_FLAC
	, MFT_APE
	, MFT_WAV
	, MFT_DSF
	, MFT_DFF
	, MFT_WMA
	, MFT_M4A
	, MFT_AAC
	, MFT_AC3
};

class __MediaLibExt IMedia
{
public:
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

    const wstring& GetFileTypeString() const;

	wstring GetTitle() const;

	virtual int GetFileSize() const = 0;
	wstring GetFileSizeString(bool bIgnoreByte = true) const;
	static wstring GetFileSizeString(int iFileSize, bool bIgnoreByte=true);
	
	virtual CMediaSet *GetMediaSet()
	{
		return NULL;
	}

	E_MediaSetType GetMediaSetType();

    virtual CMediaRes* GetMediaRes() const = 0;

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

	const wstring& GetRelatedMediaSetName(E_MediaSetType eMediaSetType)
	{
		return _getRelatedMediaSet(eMediaSetType).strMediaSetName;
	}

	void SetRelatedMediaSet(E_MediaSetType eMediaSetType, UINT uMediaSetID, const wstring& strMediaSetName, int iMediaID = -1);

	void SetRelatedMediaSet(CMediaSet& MediaSet);

	void SetRelatedMediaSet(CMedia& Media);

	void ClearRelatedMediaSet(E_MediaSetType eMediaSetType);

	virtual void AsyncTask() = 0;

	void ShellExplore(bool bSel=true);

protected:
	void UpdateFileType(const wstring& strPath);

    virtual bool CheckRelatedMediaSetChanged(const tagMediaSetChanged& MediaSetChanged)
    {
        (void)MediaSetChanged;
        return false;
    }
};
