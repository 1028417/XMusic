#pragma once

class IMediaObserver
{
public:
	virtual void renameMedia(IMedia& media, const wstring& strNewName) = 0;

	virtual class CMedia *findRelatedMedia(IMedia& media, E_MediaSetType eMediaSetType) = 0;

	virtual UINT checkDuration(IMedia& media, int& nFileSize) = 0;

	virtual UINT getSingerImgPos(UINT uSingerID) = 0;
};

enum class E_AttachDirType
{
    ADT_Internal
    , ADT_TF
    , ADT_USB
};

class __MediaLibExt CAttachDir : public CMediaRes
{
public:
	CAttachDir(const wstring& strDir, E_AttachDirType eType)
		: CMediaRes(strDir)
		, m_eType(eType)
	{
	}

public:
	E_AttachDirType m_eType;

public:
	wstring GetPath() const override
	{
        return __wcFSSlant + m_FileInfo.strName;
	}

	int _getImage() override
	{
		return (int)E_GlobalImage::GI_DirLink;
	}

	bool GetRenameText(wstring&) const
	{
		return false;
    }
};

class __MediaLibExt CRootMediaRes : public CMediaRes
{
public:
	static E_MediaFileType GetMediaFileType(const wstring& strExtName);

	static const wstring& GetMediaFileType(E_MediaFileType eMediaFileType);

public:
	CRootMediaRes(IMediaObserver& MediaObserver);

public:
	IMediaObserver& m_MediaObserver;

private:
    PairList<wstring, E_AttachDirType> m_plAttachDir;

	map<wstring, list<class CAttachDir*>> m_mapAttachDir;

private:
    void _onFindFile(TD_PathList& lstSubDir, TD_PathList& lstSubFile) override;

	int _sort(const CPath& lhs, const CPath& rhs) const override;
	
public:
    void setDir(const wstring& strDir, const PairList<wstring, E_AttachDirType>& plAttachDir);

	bool available() const
	{
		return CPath::count() > 0;
	}

	wstring toAbsPath(const wstring& strSubPath, bool bDir=false);

	wstring toOppPath(const wstring& strAbsPath) const;

	bool checkIndependentDir(const wstring& strAbsDir, bool bCheckAttachDir);

	CMediaRes* FindSubPath(const wstring& strSubPath, bool bDir) override;
};

extern CRootMediaRes *g_pRootMediaRes;
