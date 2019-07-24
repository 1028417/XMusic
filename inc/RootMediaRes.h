#pragma once

class IMediaObserver
{
public:
	virtual void renameMedia(IMedia& media, const wstring& strNewName) = 0;

	virtual class CMedia *findRelatedMedia(IMedia& media, E_MediaSetType eMediaSetType) = 0;

	virtual int checkDuration(IMedia& media) = 0;

	virtual UINT getSingerImgPos(UINT uSingerID) = 0;
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
	vector<wstring> m_vecAttachDir;

	map<wstring, class CAttachDir*> m_mapAttachDir;

private:
    void _onFindFile() override;

    inline class CAttachDir* _getAttachDir(const wstring& strDirName);

public:
	void init(const wstring& strDir, const vector<wstring>& vecAttachDir);

	wstring toAbsPath(const wstring& strSubPath, bool bDir=false);

	wstring toOppPath(const wstring& strAbsPath) const;

	bool checkIndependentDir(const wstring& strAbsDir, bool bCheckAttachDir);

	CMediaRes* FindSubPath(const wstring& strSubPath, bool bDir) override;
};

extern CRootMediaRes *g_pRootMediaRes;
