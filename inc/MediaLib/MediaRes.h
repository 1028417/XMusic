
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

#if __winvc
private:
    LPCCueFile m_pCueFile = NULL;

private:
	wstring m_strCreateTime;
	wstring m_strModifyTime;

	struct tagMediaTag
	{
		wstring strTitle;
		wstring strArtist;
		wstring strAlbum;
	} m_MediaTag;

    void ReadMP3Tag(FILE *pf);
    bool ReadFlacTag();
#endif

private:
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
		return CPath::name();
	}

	int GetFileSize() const
	{
        return fileinfo.uFileSize;
	}

	wstring GetFileSizeString(bool bIgnoreByte=true) const
	{
		__EnsureReturn(!fileinfo.bDir, L"");

		return IMedia::GetFileSizeString(fileinfo.uFileSize, bIgnoreByte);
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

#if __winvc
private:
	ArrList<CCueFile> m_alSubCueFile;
	SHashMap<wstring, UINT> m_mapSubCueFile;

    bool _loadCue(const wstring& strFileName);

public:
    CRCueFile getSubCueFile(CMediaRes& MediaRes);

    const ArrList<CCueFile>& SubCueList()
    {
        return m_alSubCueFile;
    }

#else
private:
    unordered_map<string, string> m_mapFileUrl;

    bool _loadXURL(const wstring& strFile);

 public:
    string getFileUrl(const wstring& strFileName);
#endif

	virtual void _onClear() override
	{
#if __winvc
		m_mapSubCueFile.clear();
		m_alSubCueFile.clear();
#else
		m_mapFileUrl.clear();
#endif
	}

protected:	
	virtual CPath* _newSubDir(const tagFileInfo& fileInfo) override;
	virtual XFile* _newSubFile(const tagFileInfo& fileInfo) override;

public:
	void subMediaRes(cfn_void_t<CMediaRes&> cb)
	{
		CPath::get([&](XFile& subObj) {
			cb((CMediaRes&)subObj);
		});
	}

	void subMediaRes(UINT uIdx, cfn_void_t<CMediaRes&> cb)
	{
		CPath::get(uIdx, [&](XFile& subObj) {
			cb((CMediaRes&)subObj);
		});
	}
};
