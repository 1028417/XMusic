#pragma once

class __ModelExt CSingerMgr : public CMediaSet
{
public:
	CSingerMgr(CMediaSet& RootMediaSet, class IModelObserver& ModelObserver);

private:
	class IModelObserver& m_ModelObserver;

	SList<CSingerGroup> m_lstGroups;

	list<CSinger> m_lstRootSingers;

	PtrArray<CSinger> m_paSinger;

	list<pair<wstring, CSinger*>> m_lstSingerDir;

public:
	const PtrArray<CSinger>& singers() const
	{
		return m_paSinger;
	}

	const list<pair<wstring, CSinger*>>& singerDirList() const
	{
		return m_lstSingerDir;
	}

    void clear()
    {
        m_lstGroups.clear();
        m_lstRootSingers.clear();
		m_lstSingerDir.clear();

		m_paSinger.clear();
    }

	BOOL Init();

	CSinger* matchSingerDir(cwstr strPath, bool bDir) const;

    void enumSinger(cfn_void_t<CSinger&>);

	void GetSubSets(TD_MediaSetList& lstSubSets) override;

	CMediaSet* GetLastObject()
	{
		return NULL;
	}

	CSingerGroup *AddGroup();

	BOOL RemoveGroup(UINT uSingerGroupID);

	CSinger *AddSinger(CMediaRes& SrcPath, CSingerGroup *pGroup, bool bInitAlbum);

	BOOL RemoveSinger(UINT uSingerID);

    BOOL AttachDir(CSinger& singer, cwstr strDir);
    BOOL DetachDir(CSinger& singer, cwstr strDir);
	BOOL UpdateAttachName(CSinger& singer, cwstr strDir, cwstr strName);

	CAlbum *AddAlbum(CSinger& Singer, wstring strName, const SArray<wstring> *plstAlbumItem = NULL
		, E_AlbumType eType = E_AlbumType::AT_Normal, cwstr strAttachPath = L"");

	BOOL RemoveAlbum(UINT uID);

	CAlbum *RepositAlbum(CAlbum& Album, int nNewPos);

	BOOL AddAlbumItems(const SArray<wstring>& lstOppPaths, CAlbum& Album, int nPos=-1);

	BOOL RemoveAlbumItems(const TD_AlbumItemList& lstAlbumItems);

    BOOL SetBackAlbumItems(const ConstPtrArray<CAlbumItem>& paAlbumItem);

	CSinger* RepositSinger(CSinger& Singer, CMediaSet& Target, bool bDropPositionFlag);

    int RepositAlbumItem(CAlbum& Album, const TD_IMediaList& lstMedias, UINT uTargetPos);

    CMedia* GetMedia(UINT uID);

    bool FindMedia(const tagFindMediaPara& FindPara, tagFindMediaResult& FindResult);
};
