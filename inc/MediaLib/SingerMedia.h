#pragma once

class __MediaLibExt CAlbumItem : public CMedia
{
public:
    CAlbumItem() = default;

	CAlbumItem(class CAlbum& Album, int nID, cwstr strPath, mediatime_t time);

	CAlbumItem(class CAlbum& Album, cwstr strAttachPath);

public:
    E_MediaType type() const override
    {
        return E_MediaType::MT_AlbumItem;
    }

	class CAlbum* GetAlbum() const;
	
	wstring GetAlbumName() const;

	class CSinger* GetSinger() const;

	wstring GetSingerName() const;

#if __winvc
	void AsyncTask() override;

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

    CAlbum(int nID, cwstr strName, E_AlbumType eType, cwstr strAttachPath, class CSinger *pSinger
		, UINT uLanguage = 0, bool bDisableDemand = false, bool bDisableExport = false) :
		CMediaSet(strName, (CMediaSet*)pSinger, nID, E_MediaSetType::MST_Album
			, uLanguage, bDisableDemand, bDisableExport)
		, m_eType(eType)
		, m_strAttachPath(strAttachPath)
	{
	}

	CAlbum(const CAlbum& other);

	CAlbum(CAlbum&& other);

private:
	E_AlbumType m_eType = E_AlbumType::AT_Normal;
	wstring m_strAttachPath;

	void *m_pAttachPath = NULL;

	ArrList<CAlbumItem> m_alAlbumItems;

public:
	class CSinger& GetSinger() const;

	E_AlbumType type() const
	{
		return m_eType;
	}

        const ArrList<CAlbumItem>& albumItems();
	
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

    bool playable() const override
	{
        return m_alAlbumItems;
	}

private:
	int indexOf(const CMedia& media) const override
	{
        return m_alAlbumItems.find([&](const CAlbumItem& PlayItem) {
			return &PlayItem == &media;
		});
	}

	void GetMedias(TD_MediaList& lstMedias) override
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


class __MediaLibExt CSinger : public CMediaSet
{
public:
    virtual ~CSinger() = default; // 解决qt-mac clang告警

    CSinger(CMediaSet& parent, int nID, cwstr strName, cwstr strDir, int nPos
            , UINT uLanguage = 0, bool bDisableDemand = false, bool bDisableExport = false);

	CSinger(const CSinger& other);

	CSinger(CSinger&& other);

public:
	int m_nPos = 0;

private:
	wstring m_strDir;

	list<CAlbum> m_lstAlbums;

public:
	list<CAlbum>& albums()
	{
		return m_lstAlbums;
	}
	const list<CAlbum>& albums() const
	{
		return m_lstAlbums;
	}

    CAlbum& add(const CAlbum& album)
    {
        m_lstAlbums.push_back(album);
        auto& t_album = m_lstAlbums.back();
        t_album.m_pParent = this;
        return t_album;
    }

    template <class ITR>
    CAlbum& add(const CAlbum& album, const ITR& itr)
    {
        return *m_lstAlbums.insert(itr, album);
    }

    template <class ITR>
    ITR erase(const ITR& itr)
    {
        return m_lstAlbums.erase(itr);
    }

	void GetSubSets(TD_MediaSetList& lstSubSets) override
	{
		lstSubSets.add(m_lstAlbums);
	}

	wstring GetBaseDir() const override
	{
		return m_strDir;
	}

private:
	bool FindMedia(const tagFindMediaPara& FindPara, tagFindMediaResult& FindResult) override;

#if __winvc
	int GetTreeImage() override
	{
		return __medialib.getSingerImgPos(m_uID);
	}

	wstring GetTreeText() const override
	{
		return m_strName + L' ';
	}
#endif
};

class __MediaLibExt CSingerGroup : public CMediaSet
{
public:
    virtual ~CSingerGroup() = default; // 解决qt-mac clang告警

    CSingerGroup(CMediaSet& parent, int nID=0, cwstr strName = L""
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

    CSinger& add(const CSinger& singer)
    {
        m_lstSingers.push_back(singer);
        auto& t_singer = m_lstSingers.back();
        t_singer.m_pParent = this;
        return t_singer;
    }

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
