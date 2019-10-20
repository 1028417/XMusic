
#pragma once

#include "CueFile.h"

class __MediaLibExt CMediaRes : public IMedia, public CPathObject
{
public:
    CMediaRes(const wstring& strDir)
        : CPathObject(strDir)
    {
    }

    CMediaRes(const tagFileInfo& fileInfo, E_MediaFileType eFileType)
        : IMedia(eFileType)
        , CPathObject(fileInfo)
    {
    }

	virtual ~CMediaRes() {}

private:
#if __winvc
	wstring m_strCreateTime;
	wstring m_strModifyTime;

	struct tagMediaTag
	{
		wstring strTitle;
		wstring strArtist;
		wstring strAlbum;
	} m_MediaTag;
#endif

	LPCCueFile m_pCueFile = NULL;

private:
#if __winvc
	void ReadMP3Tag(FILE *lpFile);
	bool ReadFlacTag();
#endif

	bool CheckRelatedMediaSetChanged(const tagMediaSetChanged& MediaSetChanged) override;

	bool GetRenameText(wstring& stRenameText) const override;

	void OnListItemRename(const wstring& strNewName) override;
	
	virtual int _getImage();

	CMediaRes* GetMediaRes() const override
	{
		return (CMediaRes*)this;
	}

public:
	CRCueFile getCueFile();

	class CMediaDir* parent() const;

	virtual wstring GetPath() const override;

	wstring GetAbsPath() const override;

	bool IsDir() const override
	{
        return fileinfo.bDir;
	}

	wstring GetName() const override
	{
		return CPath::GetName();
	}

	int GetFileSize() const override
	{
        return (int)fileinfo.uFileSize;
	}
	
	void SetDirRelatedSinger(UINT uSingerID, const wstring& strSingerName, bool& bChanged);

    void genMediaResListItem(bool bReportView, vector<wstring>& vecText, int& iImage, bool bGenRelatedSinger);

	void AsyncTask() override;
};

class __MediaLibExt CMediaDir : public CMediaRes
{
public:
	CMediaDir(const wstring& strDir = L"")
		: CMediaRes(strDir)
	{
	}

	CMediaDir(const tagFileInfo& fileInfo)
		: CMediaRes(fileInfo, E_MediaFileType::MFT_Null)
	{
	}

private:
	ArrList<CCueFile> m_alSubCueFile;

	SHashMap<wstring, UINT> m_mapSubCueFile;
	
	unordered_map<string, string> m_mapFileUrl;

private:
	bool _loadCue(const wstring& strFileName);

    bool _loadXURL(const wstring& strFile);

protected:	
	virtual CPath* _newSubDir(const tagFileInfo& fileInfo) override;
	virtual XFile* _newSubFile(const tagFileInfo& fileInfo) override;

public:
	const ArrList<CCueFile>& SubCueList()
	{
		return m_alSubCueFile;
	}

	CRCueFile getSubCueFile(CMediaRes& MediaRes);

	string getFileUrl(const wstring& strFileName);

	void subMediaRes(const function<void(CMediaRes&)>& cb)
	{
		CPath::get([&](XFile& subObj) {
			cb((CMediaRes&)subObj);
		});
	}

	void subMediaRes(UINT uIdx, const function<void(CMediaRes&)>& cb)
	{
		CPath::get(uIdx, [&](XFile& subObj) {
			cb((CMediaRes&)subObj);
		});
	}
	
	virtual void Clear() override
	{
		m_mapSubCueFile.clear();
		m_alSubCueFile.clear();

		CPathObject::Clear();
	}
};
