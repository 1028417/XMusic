#pragma once

enum class E_AttachDirType
{
    ADT_Internal
    , ADT_TF
    , ADT_USB
};

class __MediaLibExt CAttachDir : public CMediaDir
{
public:
	CAttachDir(cwstr strDir, E_AttachDirType eType)
		: CMediaDir(strDir)
		, m_eType(eType)
	{
	}

public:
        E_AttachDirType m_eType;

public:
    wstring GetPath() const override
    {
        return __wcPathSeparator + XFile::fileName();
    }

#if __winvc
    wstring GetDisplayTitle() const override
    {
        return XFile::fileName() + L" | " + XFile::parentDir();
    }

    int getImage() override
    {
        return (int)E_GlobalImage::GI_AttachDir;
    }

    bool GetRenameText(wstring&) const override
    {
        return false;
    }
#endif
};

class __MediaLibExt CRootMediaDir : public CMediaDir
{
public:
    virtual ~CRootMediaDir() = default;
	
    CRootMediaDir() = default;

	CRootMediaDir(cwstr strPath, const PairList<wstring, E_AttachDirType>& plAttachDir)
	{
		setDir(strPath, plAttachDir);
	}

private:
	PairList<wstring, E_AttachDirType> m_plAttachDir;
	map<wstring, list<class CAttachDir*>> m_mapAttachDir;

protected:
    virtual void _onFindFile(TD_PathList& lstSubDir, TD_XFileList& lstSubFile) override;

    virtual int _sort(const XFile& lhs, const XFile& rhs) const override;

    virtual void _onClear() override
	{
		CMediaDir::_onClear();

		m_mapAttachDir.clear();
	}
	
public:
	bool available();
	
	void setDir(cwstr strDir, const PairList<wstring, E_AttachDirType>& plAttachDir);
	
	wstring toAbsPath(cwstr strSubPath, bool bDir);

	wstring toOppPath(cwstr strAbsPath) const;

	bool checkIndependentDir(cwstr strAbsDir, bool bCheckAttachDir);

	CMediaRes* subPath(cwstr strSubPath, bool bDir) override;
};

#define __medialib CMediaLib::inst()

class __MediaLibExt CMediaLib : public CRootMediaDir
{
private:
	static CMediaLib *m_pInst;

public:
    static CMediaLib& inst()
    {
        if (m_pInst)
        {
            return *m_pInst;
        }

        static CMediaLib inst;
        return inst;
    }

protected:
	CMediaLib()
	{
        if (NULL == m_pInst)
		{
            m_pInst = this;
		}
	}
	
public:
	virtual const CMedia* findRelatedPlayItem(cwstr strPath)
        {
            (void)strPath;
            return NULL;
	}
	virtual const CMedia* findRelatedAlbumItem(cwstr strPath, const CSinger*& pRelatedSinger)
	{
            (void)strPath;
            (void)pRelatedSinger;
            return NULL;
	}

	virtual bool renameMedia(IMedia&, cwstr strNewName)
	{
		(void)strNewName;
		return false;
	}

	virtual UINT checkDuration(IMedia&, long long& nFileSize)
	{
		(void)nFileSize;
		return 0;
	}

	virtual UINT getSingerImgPos(UINT uSingerID)
	{
		(void)uSingerID;
		return 0;
	}
};

#if !__winvc
class __MediaLibExt CSnapshotMediaDir : public CMediaDir, public CMediaSet
{
public:
    CSnapshotMediaDir(cwstr strPath, CMediaDir *pParentDir, CMediaSet *pParentMediaSet)
        : CMediaDir(strPath, pParentDir)
        , CMediaSet(L"", pParentMediaSet, E_MediaSetType::MST_SnapshotMediaDir)
    {
    }

private:
    bool isLocal() const override
    {
        return false;
    }

    void GetMedias(TD_IMediaList&) override;

    void GetSubSets(TD_MediaSetList&) override;

#if !__winvc
    wstring name() const override
    {
        if (!m_strName.empty())
        {
            return m_strName;
        }

		return XFile::fileName();;
    }
#endif
};
#endif
