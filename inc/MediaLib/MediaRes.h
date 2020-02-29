
#pragma once

#if __winvc
#include "CueFile.h"

struct tagMediaTag
{
	wstring strTitle;
	wstring strArtist;
	wstring strAlbum;
};
#endif

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
	wstring m_strCreateTime;
	wstring m_strModifyTime;

	tagMediaTag m_MediaTag;

	LPCCueFile m_pCueFile = NULL;
#endif

private:
	bool GetRenameText(wstring& stRenameText) const override;

	void OnListItemRename(const wstring& strNewName) override;
	
	virtual int _getImage();

public:
#if __winvc
    static void ReadMP3Tag(const wstring& strFile, tagMediaTag& MediaTag);

	static bool ReadFlacTag(const wstring& strFile, tagMediaTag& MediaTag);

	CRCueFile getCueFile();
#endif

    class CMediaDir* parent() const;

	virtual wstring GetPath() const override;

	wstring GetAbsPath() const override;

	bool IsDir() const override
	{
	    return m_fi.bDir;
	}

	wstring GetName() const override
	{
		return CPath::name();
	}

	long long fileSize() const override
	{
	    return m_fi.uFileSize;
	}

	void SetDirRelatedSinger(UINT uSingerID, const wstring& strSingerName, bool& bChanged);

    void genMediaResListItem(bool bReportView, vector<wstring>& vecText, int& iImage, bool bGenRelatedSinger);

    void AsyncTask();
};

class __MediaLibExt CMediaDir : public CMediaRes, public CTreeObject
{
public:
	virtual ~CMediaDir() {}

	CMediaDir() {}

	CMediaDir(const tagFileInfo& fileInfo)
		: CMediaRes(E_MediaFileType::MFT_Null, fileInfo)
	{
	}

	CMediaDir(const wstring& strPath, class CPath *t_pParent = NULL)
		: CMediaRes(E_MediaFileType::MFT_Null, true, strPath, t_pParent)
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

private:
	wstring GetTreeText() const override
	{
	        return m_fi.strName;
	}

	void GetTreeChilds(TD_TreeObjectList& lstChilds) override
	{
		lstChilds.add(TD_MediaDirList(this->dirs()));
	}
};
