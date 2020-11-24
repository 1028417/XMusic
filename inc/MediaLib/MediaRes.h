
#pragma once

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

    CMediaRes(cwstr strDir, CMediaRes *pParent = NULL)
        : IMedia(E_MediaFileType::MFT_Null)
        , CPathObject(strDir, pParent)
    {
    }

    CMediaRes(E_MediaFileType eFileType, CMediaRes& parent, cwstr strName, uint64_t uFileSize = 0)
		: IMedia(eFileType)
        , CPathObject(parent, strName, uFileSize)
	{
	}

#if __winvc
private:
	wstring m_strCreateTime;
	wstring m_strModifyTime;

	tagMediaTag m_MediaTag;

    LPCCueFile m_pCueFile = NULL;

protected:
	virtual bool GetRenameText(wstring& stRenameText) const override
	{
		stRenameText = m_fi.bDir ? fileName() : GetTitle();
		return true;
	}

    bool OnListItemRename(cwstr strNewName) override;

public:
	void asyncTask();

    static void ReadMP3Tag(cwstr strFile, tagMediaTag& MediaTag);

    static bool ReadFlacTag(cwstr strFile, tagMediaTag& MediaTag);

    virtual int getImage();

    virtual void genMediaResListItem(E_ListViewType, vector<wstring>& vecText, int& iImage, bool bSingerPanel);

    CRCueFile cueFile() override;
#endif

public:
    virtual E_MediaType type() const override
    {
        return E_MediaType::MT_MediaRes;
    }

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

#if !__winvc
    virtual bool isLocal() const
    {
        return true;
    }
#endif

    class CMediaDir* parent() const;

	wstring fileTimeString(bool bNewLine) const
	{
		return CMediaTime::genFileTimeString(m_fi.tModifyTime, bNewLine);
	}

    void SetDirRelatedSinger(UINT uSingerID, cwstr strSingerName, bool& bChanged);
};

// TODO class __MediaLibExt CMediaFile : public IMedia, public CMediaRes

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

    CMediaDir(cwstr strDir, CMediaDir *pParent = NULL)
        : CMediaRes(strDir, pParent)
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
	wstring GetTitle() const override
	{
		return XFile::fileName();
	}

    void get(cfn_void_t<CMediaRes&> cb)
	{
		CPath::get([&](XFile& subObj) {
			cb((CMediaRes&)subObj);
		});
	}
    void get(UINT uIdx, cfn_void_t<CMediaRes&> cb)
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

#if __winvc
public:
	virtual wstring GetDisplayTitle() const
	{
		return fileName();
	}

protected:
	virtual wstring GetTreeText() const override
	{
		return GetDisplayTitle();
	}

	void GetTreeChilds(TD_TreeObjectList& lstChilds) override
	{
		lstChilds.add(TD_MediaDirList(this->dirs()));
	}
#endif
};
