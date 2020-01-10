#pragma once

#define __medialibDir L".xmusic"

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
	wstring GetPath() const override
	{
        return __wchDirSeparator + GetName();
	}

	int _getImage() override
	{
		return (int)E_GlobalImage::GI_DirLink;
	}

    bool GetRenameText(wstring&) const override
	{
		return false;
    }
};

class __MediaLibExt CRootMediaDir : public CMediaDir
{
public:
	virtual ~CRootMediaDir() {}
	
	CRootMediaDir() {}

	CRootMediaDir(const wstring& strPath, const PairList<wstring, E_AttachDirType>& plAttachDir)
	{
		setDir(strPath, plAttachDir);
	}

private:
	PairList<wstring, E_AttachDirType> m_plAttachDir;
	map<wstring, list<class CAttachDir*>> m_mapAttachDir;

private:
	void _onFindFile(TD_PathList& lstSubDir, TD_XFileList& lstSubFile) override;

	int _sort(const XFile& lhs, const XFile& rhs) const override;

	void _onClear() override
	{
                m_mapAttachDir.clear();
	}

	CMediaRes* _findSubPath(const wstring& strSubPath, bool bDir);

	CPath* _newSubDir(const tagFileInfo& fileInfo) override
	{
		if (fileInfo.strName == __medialibDir)
		{
			return NULL;
		}

		return CMediaDir::_newSubDir(fileInfo);
	}

public:
	void setDir(const wstring& strDir, const PairList<wstring, E_AttachDirType>& plAttachDir);

	bool empty() const
	{
		return CPath::count() == 0;
	}

	wstring toAbsPath(const wstring& strSubPath, bool bDir = false);

	wstring toOppPath(const wstring& strAbsPath) const;

	bool checkIndependentDir(const wstring& strAbsDir, bool bCheckAttachDir);

	CMediaDir* findSubDir(const wstring& strSubDir) override
	{
		return (CMediaDir*)_findSubPath(strSubDir, true);
	}
	CMediaRes* findSubFile(const wstring& strSubFile) override
	{
		return _findSubPath(strSubFile, false);
	}

	CMediaRes* findSubFile(const CMedia& media);
	CMediaRes* findSubPath(const IMedia& media)
	{
		return _findSubPath(media.GetPath(), media.IsDir());
	}
};

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
	virtual CMedia* findRelatedMedia(const wstring& strPath, E_MediaSetType, CMediaSet*& pSinger)
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
