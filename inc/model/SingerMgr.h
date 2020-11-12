#pragma once

class __ModelExt CSingerMgr : public CMediaSet
{
public:
    CSingerMgr(CMediaSet& RootMediaSet, class IModelObserver& ModelObserver);

private:
	class IModelObserver& m_ModelObserver;

    SList<CSingerGroup> m_lstGroups;

	list<CSinger> m_lstRootSingers;

public:
    const SList<CSingerGroup>& groups() const
	{
		return m_lstGroups;
	}

	const list<CSinger>& rootSingers() const
	{
		return m_lstRootSingers;
	}

    void clear()
    {
        m_lstGroups.clear();
        m_lstRootSingers.clear();
    }

	BOOL Init();

    void enumSinger(cfn_void_t<const CSinger&>) const;
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

	BOOL UpdateSingerPos(UINT uSingerID, int nPos, int nDstGroupID);

    BOOL AttachDir(CSinger& singer, cwstr strDir);
    BOOL DetachDir(CSinger& singer, cwstr strDir);

	CAlbum *AddAlbum(CSinger& Singer, wstring strName, const SArray<wstring> *plstAlbumItem = NULL
		, E_AlbumType eType = E_AlbumType::AT_Normal, cwstr strAttachPath = L"");

	BOOL RemoveAlbum(UINT uID);

	CAlbum *RepositAlbum(CAlbum& Album, int nNewPos);

	BOOL AddAlbumItems(const SArray<wstring>& lstOppPaths, CAlbum& Album, int nPos=-1);

	BOOL RemoveAlbumItems(const TD_AlbumItemList& lstAlbumItems);

    BOOL SetBackAlbumItems(const ConstPtrArray<CAlbumItem>& paAlbumItem);

	CSinger* RepositSinger(CSinger& Singer, CMediaSet& Target, bool bDropPositionFlag);

    int RepositAlbumItem(CAlbum& Album, const TD_IMediaList& lstMedias, UINT uTargetPos);

	bool FindMedia(const tagFindMediaPara& FindPara, tagFindMediaResult& FindResult);
};
