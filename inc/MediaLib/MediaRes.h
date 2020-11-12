
#pragma once

#include "CueFile.h"

struct tagMediaTag
{
	wstring strTitle;
	wstring strArtist;
	wstring strAlbum;
};

class __MediaLibExt CMediaRes : public IMedia, public CPathObject
{
public:
    virtual ~CMediaRes() = default;

    CMediaRes() = default;

	CMediaRes(E_MediaFileType eFileType, const tagFileInfo& fileInfo)
		: IMedia(eFileType)
		, CPathObject(fileInfo)
	{
	}

    CMediaRes(E_MediaFileType eFileType, bool bDir, cwstr strName, class CPath *pParent)
		: IMedia(eFileType)
        , CPathObject(bDir, strName, pParent)
	{
	}

#if __winvc
private:
	wstring m_strCreateTime;
	wstring m_strModifyTime;

	tagMediaTag m_MediaTag;

    LPCCueFile m_pCueFile = NULL;

private:
	bool GetRenameText(wstring& stRenameText) const override;

    void OnListItemRename(cwstr strNewName) override;
	
public:
    static void ReadMP3Tag(cwstr strFile, tagMediaTag& MediaTag);

    static bool ReadFlacTag(cwstr strFile, tagMediaTag& MediaTag);

    CRCueFile getCueFile();

    virtual int getImage();

    virtual void genMediaResListItem(E_ListViewType, vector<wstring>& vecText, int& iImage, bool bGenRelatedSinger);
#endif

    virtual E_MediaType type() const override
    {
        return E_MediaType::MT_MediaRes;
    }

	class CMediaDir* parent() const;

	virtual wstring GetPath() const override;

	wstring GetAbsPath() const override;

	wstring GetName() const override
	{
		return XFile::fileName();
	}

	long long fileSize() const override
	{
		return m_fi.uFileSize;
	}

	wstring fileTimeString(bool bNewLine) const
	{
		return CMediaTime::genFileTimeString(m_fi.tModifyTime, bNewLine);
	}

    void SetDirRelatedSinger(UINT uSingerID, cwstr strSingerName, bool& bChanged);

    void findRelatedMedia();
};

// TODO class __MediaLibExt CMediaFile : public IMedia, public CMediaRes


enum class E_MediaDirType
{
	MDT_Local
	, MDT_Attach
#if !__winvc
    , MDT_Snapshot
#endif
};

class __MediaLibExt CCueList
{
public:
	CCueList() = default;

private:
        list<CCueFile> m_lstCueFile;

        unordered_map<wstring, CCueFile*> m_mapCueFile;

private:
        void _tomap(CCueFile& cueFile);

public:
	const list<CCueFile>& cues() const
	{
                return m_lstCueFile;
	}

    bool load(cwstr strDir, cwstr strFileName);

    bool load(Instream& ins, cwstr strFileTitle);

    CRCueFile find(cwstr strTitle) const;

	void clear()
	{
                m_lstCueFile.clear();
                m_mapCueFile.clear();
	}
};

class __MediaLibExt CMediaDir : public CMediaRes
#if __winvc
	, public CTreeObject
#endif
{
public:
    virtual ~CMediaDir() = default;

    CMediaDir() = default;

	CMediaDir(const tagFileInfo& fileInfo)
		: CMediaRes(E_MediaFileType::MFT_Null, fileInfo)
	{
	}

    CMediaDir(cwstr strPath, class CPath *pParent = NULL)
        : CMediaRes(E_MediaFileType::MFT_Null, true, strPath, pParent)
	{
	}
	
#if __winvc
private:
	CCueList m_cuelist;

public:
	const CCueList& cuelist() const
	{
		return m_cuelist;
	}
#endif

	E_MediaType type() const override
	{
		return E_MediaType::MT_MediaDir;
	}

protected:
	virtual CPath* _newSubDir(const tagFileInfo& fileInfo) override;
	virtual XFile* _newSubFile(const tagFileInfo& fileInfo) override;

#if __winvc
    virtual void _onClear() override
    {
		m_cuelist.clear();
    }
#endif

public:
	virtual E_MediaDirType dirType()
	{
		return E_MediaDirType::MDT_Local;
	}

    virtual wstring GetTitle() const override
    {
        return XFile::fileName();
    }

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

	inline virtual CMediaRes* subPath(cwstr strSubPath, bool bDir)
	{
		return (CMediaRes*)CPath::findSubPath(strSubPath, bDir);
	}

	inline CMediaDir* subDir(cwstr strSubDir)
	{
		return (CMediaDir*)subPath(strSubDir, true);
	}

	inline CMediaRes* subFile(cwstr strSubFile)
	{
		return subPath(strSubFile, false);
	}

	inline CMediaRes* subFile(const IMedia& media)
	{
		return subPath(media.GetPath(), false);
	}

#if __winvc
private:
	wstring GetTreeText() const override
	{
	        return GetTitle();
	}

	void GetTreeChilds(TD_TreeObjectList& lstChilds) override
	{
		lstChilds.add(TD_MediaDirList(this->dirs()));
	}
#endif
};
