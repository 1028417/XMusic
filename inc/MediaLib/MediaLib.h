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
	CAttachDir(const wstring& strDir, E_AttachDirType eType)
		: CMediaDir(strDir)
		, m_eType(eType)
	{
	}

public:
	E_AttachDirType m_eType;

public:
    E_MediaDirType dirType() override
	{
		return E_MediaDirType::MDT_Attach;
	}

	wstring GetPath() const override
	{
        return __wcPathSeparator + XFile::fileName();
	}

	wstring GetTitle() const override
	{
		return XFile::fileName() + L" | " + XFile::parentDir();
	}

#if __winvc
	int getImage() override
	{
		return (int)E_GlobalImage::GI_DirLink;
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

	CRootMediaDir(const wstring& strPath, const PairList<wstring, E_AttachDirType>& plAttachDir)
	{
		setDir(strPath, plAttachDir);
	}

private:
	PairList<wstring, E_AttachDirType> m_plAttachDir;
	map<wstring, list<class CAttachDir*>> m_mapAttachDir;

private:
    virtual void _onFindFile(TD_PathList& lstSubDir, TD_XFileList& lstSubFile) override;

    virtual int _sort(const XFile& lhs, const XFile& rhs) const override;

    virtual void _onClear() override
	{
                m_mapAttachDir.clear();
	}
	
public:
	bool available();
	
	void setDir(const wstring& strDir, const PairList<wstring, E_AttachDirType>& plAttachDir);
	
	wstring toAbsPath(const wstring& strSubPath, bool bDir = false);

	wstring toOppPath(const wstring& strAbsPath) const;

	bool checkIndependentDir(const wstring& strAbsDir, bool bCheckAttachDir);

	CMediaRes* findSubPath(const wstring& strSubPath, bool bDir);

	CMediaDir* findSubDir(const wstring& strSubDir)
	{
		return (CMediaDir*)findSubPath(strSubDir, true);
	}

	CMediaRes* findSubFile(const wstring& strSubFile)
	{
		return findSubPath(strSubFile, false);
	}

	CMediaRes* findSubFile(const CMedia& media)
	{
		return findSubPath(media.GetPath(), false);
	}
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
	virtual CMedia* findRelatedMedia(const wstring& strPath, E_MediaSetType, class CSinger*& pSinger)
	{
		(void)strPath;
		(void)pSinger;
		return NULL;
	}
	
	virtual void renameMedia(IMedia&, const wstring& strNewName)
	{
		(void)strNewName;
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
