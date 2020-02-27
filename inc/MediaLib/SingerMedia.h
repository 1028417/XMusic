#pragma once

class __MediaLibExt CAlbumItem : public CMedia
{
public:
    CAlbumItem() {}

	CAlbumItem(int nID, const wstring& strPath, mediatime_t time, class CAlbum& Album);

private:
	void GenListItem(bool bReportView, vector<wstring>& vecText, int& iImage) override;

public:
	class CAlbum* GetAlbum() const;
	
	wstring GetAlbumName() const;

	class CSinger* GetSinger() const;

	wstring GetSingerName() const;

	void AsyncTask() override;
};

class __MediaLibExt CAlbum : public CMediaSet
{
public:
	virtual ~CAlbum() {} // 解决qt-mac clang告警

	CAlbum(int nID = 0, const wstring& strName = L"", class CSinger *pSinger = NULL
		, UINT uLanguage = 0, bool bDisableDemand = false, bool bDisableExport = false)
		: CMediaSet(strName, (CMediaSet*)pSinger, nID, E_MediaSetType::MST_Album
			, uLanguage, bDisableDemand, bDisableExport)
	{
	}

	CAlbum(const CAlbum& other);

	CAlbum(CAlbum&& other);

private:
    ArrList<CAlbumItem> m_alAlbumItems;

public:
	class CSinger& GetSinger() const;

	const ArrList<CAlbumItem>& albumItems() const
	{
         return m_alAlbumItems;
	}

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

    bool available() override
	{
        return !m_alAlbumItems.empty();
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

	void GenListItem(bool bReportView, vector<wstring>& vecText, int& iImage) override;

	wstring GetExportName() override
	{
        return (m_pParent->m_strName + L"专辑") + __CNDot + m_strName;
	}
};


class __MediaLibExt CSinger : public CMediaSet
{
public:
	virtual ~CSinger() {} // 解决qt-mac clang告警

    CSinger(CMediaSet& parent, int nID = 0, const wstring& strName = L"", const wstring& strDir = L""
            , int nPos = 0, UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport=false);

	CSinger(const CSinger& other);

	CSinger(CSinger&& other);

	int m_nPos = 0;

private:
	wstring m_strDir;

	list<CAlbum> m_lstAlbums;

public:
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
	int GetTreeImage() override;
	
	bool FindMedia(const tagFindMediaPara& FindPara, tagFindMediaResult& FindResult) override;
};

class __MediaLibExt CSingerGroup : public CMediaSet
{
public:
    virtual ~CSingerGroup() {} // 解决qt-mac clang告警

    CSingerGroup(CMediaSet& parent, int nID=0, const wstring& strName = L""
            , UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport=false);

	CSingerGroup(const CSingerGroup& other);

	CSingerGroup(CSingerGroup&& other);

private:
	list<CSinger> m_lstSingers;

public:
    size_t size() const
    {
        return m_lstSingers.size();
    }

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

	int GetTreeImage() override
	{
		return (int)E_GlobalImage::GI_SingerGroup;
	}
};
