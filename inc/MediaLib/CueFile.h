
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

typedef const class CCueFile& CRCueFile;
typedef const class CCueFile* LPCCueFile;

class __MediaLibExt CCueFile
{
public:
	static const CCueFile NoCue;

	CCueFile() = default;

	CCueFile(wstring strFileName, const wstring& strDir)
		: m_strCueName(strFileName)
		, m_strDir(strDir)
	{
	}

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

public:
	wstring m_strCueName;
	wstring m_strDir;
	
	wstring m_strPerformer;
	
	wstring m_strTitle;
	UINT m_uLineTitle = 0;

	wstring m_strMediaFile;
	UINT m_uLineMediaFile = 0;

	ArrList<tagTrackInfo> m_alTrackInfo;

public:
	wstring filePath() const
	{
		return m_strDir + __wcPathSeparator + m_strCueName;
	}
	
    bool load(Instream& ins);

	CRTrackInfo getTrack(UINT uMs) const;
	
	bool updateMediaFileName(const wstring& strMediaFileName);
};
