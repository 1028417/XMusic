#pragma once

#define __medialibDir L".xmusic"

class IMediaObserver
{
public:
	virtual void renameMedia(IMedia& media, const wstring& strNewName) = 0;

	virtual class CMedia *findRelatedMedia(IMedia& media, E_MediaSetType eMediaSetType) = 0;

    virtual UINT checkDuration(IMedia& media, long long& nFileSize) = 0;

	virtual UINT getSingerImgPos(UINT uSingerID) = 0;
};

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
        return __wcDirSeparator + GetName();
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
		CMediaDir::_onClear();
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

	CMediaRes* findSubFile(const IMedia& media)
	{
		return _findSubPath(media.GetPath(), false);
	}
};

class __MediaLibExt CMediaLib : public CRootMediaDir
{
public:
	CMediaLib(IMediaObserver& MediaObserver);

public:
	IMediaObserver& m_MediaObserver;
};

extern CMediaLib *g_pMediaLib;
