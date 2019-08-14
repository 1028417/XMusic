
#pragma once

#include "CueFile.h"

class __MediaLibExt CMediaRes : public IMedia, public CPathObject
{
public:
	CMediaRes() {}

	CMediaRes(const tagFileInfo& FileInfo, CPath& ParentDir, E_MediaFileType eFileType = E_MediaFileType::MFT_Null);

	CMediaRes(const wstring& strDir);

private:
	struct tagMediaTag
	{
		wstring strTitle;
		wstring strArtist;
		wstring strAlbum;
	} m_MediaTag;
	
	ArrList<CCueFile> m_alSubCueFile;

	SHashMap<wstring, UINT> m_mapSubCueFile;

	LPCCueFile m_pCueFile = NULL;

	wstring m_strCreateTime;
	wstring m_strModifyTime;

private:
#if __android
    void _sortSubPath() override {}
#endif

	bool _loadCue(const wstring& strFileName);

	CPath* NewSubPath(const tagFileInfo& FileInfo, CPath& ParentDir) override;

	void ReadMP3Tag(FILE *lpFile);
	bool ReadFlacTag();

	bool CheckRelatedMediaSetChanged(const tagMediaSetChanged& MediaSetChanged) override;

	virtual bool GetRenameText(wstring& stRenameText) const override;

	void OnListItemRename(const wstring& strNewName) override;
	
	int _getImage();

	CMediaRes* GetMediaRes() const override
	{
		return (CMediaRes*)this;
	}

public:
    CMediaRes* parent() const
	{
		return (CMediaRes*)m_pParentDir;
	}

	const ArrList<CCueFile>& SubCueList()
	{
		return m_alSubCueFile;
	}

	CRCueFile getSubCueFile(CMediaRes& MediaRes);

	CRCueFile getCueFile();
	
	virtual wstring GetPath() const override;

	wstring GetAbsPath() const override;

	bool IsDir() const override
	{
		return CPath::IsDir();
	}

	wstring GetName() const override
	{
		return CPath::GetName();
	}

	int GetFileSize() const override
	{
		return (int)m_uFileSize;
	}
	
	void SetDirRelatedSinger(UINT uSingerID, const wstring& strSingerName, bool& bChanged);

	void GenListItem(bool bReportView, vector<wstring>& vecText, int& iImage, bool bGenRelatedSinger);

	virtual CMediaRes *FindSubPath(const wstring& strSubPath, bool bDir)
	{
        return (CMediaRes*)CPath::FindSubPath(strSubPath, bDir);
	}

	void Clear() override;

	void AsyncTask() override;
};
