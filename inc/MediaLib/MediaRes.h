
#pragma once

#include "CueFile.h"

class __MediaLibExt CMediaRes : public IMedia, public CPathObject
{
public:
	virtual ~CMediaRes() {}

	CMediaRes() {}

    CMediaRes(E_MediaFileType eFileType, const tagFileInfo& fileInfo)
        : IMedia(eFileType)
        , CPathObject(fileInfo)
    {
    }

	CMediaRes(E_MediaFileType eFileType, bool t_bDir, const wstring& t_strName, class CPath *t_pParent)
		: IMedia(eFileType)
		, CPathObject(t_bDir, t_strName, t_pParent)
	{
	}

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
	virtual ~CMediaDir() {}

	CMediaDir() {}

	CMediaDir(const tagFileInfo& fileInfo)
		: CMediaRes(E_MediaFileType::MFT_Null, fileInfo)
	{
	}

	CMediaDir(const wstring& t_strName, class CPath *t_pParent = NULL)
		: CMediaRes(E_MediaFileType::MFT_Null, true, t_strName, t_pParent)
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
#endif

protected:
	virtual CPath* _newSubDir(const tagFileInfo& fileInfo) override;
	virtual XFile* _newSubFile(const tagFileInfo& fileInfo) override;

#if __winvc
    virtual void _onClear() override
    {
        m_mapSubCueFile.clear();
        m_alSubCueFile.clear();
    }
#endif

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

    virtual CMediaDir* findSubDir(const wstring& strSubDir)
    {
        return (CMediaDir*)CPath::findSubDir(strSubDir);
    }
    virtual CMediaRes* findSubFile(const wstring& strSubFile)
    {
        return (CMediaRes*)CPath::findSubFile(strSubFile);
    }
};
