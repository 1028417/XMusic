#pragma once

class __MediaLibExt CAlbumItem : public CMedia
{
public:
    CAlbumItem() = default;

    CAlbumItem(class CAlbum& Album, UINT uID, cwstr strPath
        , mediatime_t time, uint64_t uFileSize = 0, UINT uDuration = 0);

    CAlbumItem(class CAlbum& Album, CMediaRes& MediaRes);

public:
    E_MediaType type() const override
    {
        return E_MediaType::MT_AlbumItem;
    }

	class CAlbum* GetAlbum() const;
	
	wstring GetAlbumName() const;

	class CSinger* GetSinger() const;

	wstring GetSingerName() const;

	wstring GetBaseDir() const override;

#if __winvc
	const CMedia* findRelatedMedia() override
	{
		return IMedia::findRelatedPlayItem();
	}

private:
	void GenListItem(E_ListViewType, vector<wstring>& vecText, TD_ListImgIdx&) override;
#endif
};

enum class E_AlbumType
{
	AT_Normal = 0,
	AT_Dir,
	AT_WholeTrack
};

class __MediaLibExt CAlbum : public CMediaSet
{
public:
    virtual ~CAlbum() = default; // 解决qt-mac clang告警

    CAlbum() = default;

    CAlbum(class CSinger& Singer, UINT uID, cwstr strName, E_AlbumType eType, cwstr strAttachPath = L""
		, UINT uLanguage = 0, bool bDisableDemand = false, bool bDisableExport = false) :
        CMediaSet(strName, (CMediaSet*)&Singer, E_MediaSetType::MST_Album, uID
			, uLanguage, bDisableDemand, bDisableExport)
        , m_eAlbumType(eType)
		, m_strAttachPath(strAttachPath)
	{
	}

	CAlbum(const CAlbum& other);

	CAlbum(CAlbum&& other);

private:
    E_AlbumType m_eAlbumType = E_AlbumType::AT_Normal;
	wstring m_strAttachPath;

	void *m_pAttachPath = NULL;

	ArrList<CAlbumItem> m_alAlbumItems;

public:
	class CSinger& GetSinger() const;

    E_AlbumType albumType() const
	{
        return m_eAlbumType;
	}

    ArrList<CAlbumItem>& albumItems();
	
    CAlbumItem& add(const CAlbumItem& AlbumItem)
    {
        auto& t_AlbumItem = m_alAlbumItems.add(AlbumItem);
        t_AlbumItem.m_pParent = this;
        return t_AlbumItem;
    }

    template <class ITR>
    ITR erase(const ITR& itr)
    {
        return m_alAlbumItems.erase(itr);
    }

private:
    int indexOf(const IMedia& media) const override
    {
        return m_alAlbumItems.find([&](const CAlbumItem& PlayItem) {
            return &PlayItem == &media;
        });
    }

    void GetMedias(TD_IMediaList& lstMedias) override
    {
        lstMedias.add(m_alAlbumItems);
    }

    wstring GetExportName() override
    {
        return (m_pParent->m_strName + L"专辑") + __CNDot + m_strName;
    }

#if __winvc
    void GenListItem(E_ListViewType, vector<wstring>& vecText, TD_ListImgIdx&) override;
#endif
};

struct tagSingerAttachDir
{
    tagSingerAttachDir() = default;

    tagSingerAttachDir(cwstr strDir)
        : strDir(strDir)
    {
    }

    wstring strDir;

#if !__winvc
    CMediaSet *pSnapshotDir = NULL;
#endif
};

class __MediaLibExt CSinger : public CMediaSet
{
public:
    virtual ~CSinger() = default; // 解决qt-mac clang告警

    CSinger(CMediaSet& parent, UINT uID, cwstr strName, cwstr strDir, UINT uPos
            , UINT uLanguage = 0, bool bDisableDemand = false, bool bDisableExport = false);

	CSinger(const CSinger& other);

	CSinger(CSinger&& other);

public:
	UINT m_uPos = 0;

private:
	wstring m_strDir;

	list<CAlbum> m_lstAlbums;

    list<tagSingerAttachDir> m_lstAttachDir;

public:
    cwstr dir() const
    {
        return m_strDir;
    }

    const list<CAlbum>& albums() const
    {
        return m_lstAlbums;
    }
    list<CAlbum>& albums()
    {
        return m_lstAlbums;
    }

    const list<tagSingerAttachDir>& attachDir() const
    {
        return m_lstAttachDir;
    }
    list<tagSingerAttachDir>& attachDir()
    {
        return m_lstAttachDir;
    }

    CAlbum& addAlbum(const CAlbum& album)
    {
        m_lstAlbums.push_back(album);
        auto& t_album = m_lstAlbums.back();
        t_album.m_pParent = this;
        return t_album;
    }

    void GetSubSets(TD_MediaSetList& lstSubSets) override;

    bool FindMedia(const tagFindMediaPara& FindPara, tagFindMediaResult& FindResult);

#if !__winvc
    bool available() const override
    {
        for (cauto album : m_lstAlbums)
        {
            if (album.m_bAvailable)
            {
                return true;
            }
        }

        for (cauto attachDir : m_lstAttachDir)
        {
            if (attachDir.pSnapshotDir && attachDir.pSnapshotDir->m_bAvailable)
            {
                return true;
            }
        }

        return false;
    }
#endif

private:
#if __winvc
	int GetTreeImage() override
	{
		return __medialib.getSingerImgPos(m_uID);
	}

	wstring GetTreeText() const override
	{
		return m_strName + L' ';
	}

	void GetTreeChilds(TD_TreeObjectList& lstChilds) override
	{
		for (auto& album : m_lstAlbums)
		{
			if (album.albumType() == E_AlbumType::AT_Normal)
			{
				lstChilds.add(album);
			}
		}
	}

#else
    void GetAllMedias(TD_IMediaList& lstMedias) override
    {
        for (auto& album : m_lstAlbums)
        {
            lstMedias.add(album.albumItems());
        }

        for (cauto attachDir : m_lstAttachDir)
        {
            if (attachDir.pSnapshotDir)
            {
                attachDir.pSnapshotDir->GetAllMedias(lstMedias);
            }
        }
    }
#endif
};

class __MediaLibExt CSingerGroup : public CMediaSet
{
public:
    virtual ~CSingerGroup() = default; // 解决qt-mac clang告警

    CSingerGroup(CMediaSet& parent, UINT uID=0, cwstr strName = L""
            , UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport=false);

	CSingerGroup(const CSingerGroup& other);

	CSingerGroup(CSingerGroup&& other);

private:
	list<CSinger> m_lstSingers;

public:
    const list<CSinger>& singers() const
    {
        return m_lstSingers;
    }
    list<CSinger>& singers()
    {
        return m_lstSingers;
    }

    CSinger& add(const CSinger& singer)
    {
        m_lstSingers.push_back(singer);
        auto& t_singer = m_lstSingers.back();
        t_singer.m_pParent = this;
        return t_singer;
    }

#if !__winvc
    bool available() const override
    {
        for (cauto singer : m_lstSingers)
        {
            if (singer.available())
            {
                return true;
            }
        }

        return false;
    }
#endif

private:
	void GetSubSets(TD_MediaSetList& lstSubSets) override
	{
		lstSubSets.add(m_lstSingers);
	}

#if __winvc
	int GetTreeImage() override
	{
		return (int)E_GlobalImage::GI_SingerGroup;
	}

	wstring GetTreeText() const override
	{
		return m_strName + L' ';
	}
#endif
};
