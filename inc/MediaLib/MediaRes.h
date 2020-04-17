
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

private:
	bool GetRenameText(wstring& stRenameText) const override;

	void OnListItemRename(const wstring& strNewName) override;
	
public:
	static void ReadMP3Tag(const wstring& strFile, tagMediaTag& MediaTag);

	static bool ReadFlacTag(const wstring& strFile, tagMediaTag& MediaTag);

    CRCueFile getCueFile();

    virtual int getImage();

    void genMediaResListItem(E_ListViewType, vector<wstring>& vecText, int& iImage, bool bGenRelatedSinger);
#endif

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

	void SetDirRelatedSinger(UINT uSingerID, const wstring& strSingerName, bool& bChanged);

    void AsyncTask();
};

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

	bool load(const wstring& strDir, const wstring& strFileName);

    bool load(Instream& ins, const wstring& strFileTitle);

    CRCueFile find(const wstring& strTitle) const;

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

	CMediaDir(const wstring& strPath, class CPath *t_pParent = NULL)
		: CMediaRes(E_MediaFileType::MFT_Null, true, strPath, t_pParent)
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
