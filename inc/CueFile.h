
#pragma once

//typedef const struct tagTrackInfo* LPCTrackInfo;
typedef const struct tagTrackInfo& CRTrackInfo;

struct __MediaLibExt tagTrackInfo
{
	static const tagTrackInfo NoTrack;

	bool operator ==(CRTrackInfo trackInfo) const
	{
		return &trackInfo == this;
	}

	/*bool operator ==(LPCTrackInfo pTrackInfo) const
	{
		return pTrackInfo == this;
	}*/

	operator bool() const
	{
		return &tagTrackInfo::NoTrack != this;
	}

	/*operator LPCTrackInfo() const
	{
		return this;
	}*/

	UINT uIndex = 0;
	wstring strPerformer;
	wstring strTitle;
	UINT uMsBegin = 0;
};

typedef const struct CCueFile& CRCueFile;
typedef const struct CCueFile* LPCCueFile;

struct __MediaLibExt CCueFile
{
public:
	static const CCueFile NoCue;

	bool operator ==(CRCueFile cueFile) const
	{
		return &cueFile == this;
	}

	bool operator ==(LPCCueFile pCueFile) const
	{
		return pCueFile == this;
	}

	operator bool() const
	{
		return &NoCue != this;
	}

	operator LPCCueFile() const
	{
		return this;
	}

	CCueFile()
	{
	}

	CCueFile(wstring strFileName)
	{
		m_strFileName = strFileName;
	}

public:
	SVector<string> m_vecLineData;

	wstring m_strFileName;

	wstring m_strPerformer;
	
	wstring m_strTitle;
	UINT m_uLineTitle = 0;

	wstring m_strMediaFileName;
	UINT m_uLineMediaFileName = 0;

	ArrList<tagTrackInfo> m_alTrackInfo;

public:
	bool load(const wstring& strFile);

	CRTrackInfo getTrack(UINT uMs) const;
	
	bool updateTitle(const wstring& strTitle);

	bool updateMediaFileName(const wstring& strMediaFileName);
};
