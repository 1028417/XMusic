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
	ArrList<CAlbumItem> m_lstAlbumItems;

public:
	class CSinger& GetSinger() const;

	ArrList<CAlbumItem>& albumItems()
	{
		return m_lstAlbumItems;
	}
	const ArrList<CAlbumItem>& albumItems() const
	{
		 return m_lstAlbumItems;
	}

	void addAlbumItem(const CAlbumItem& AlbumItem);

	bool available() override
	{
		return !m_lstAlbumItems.empty();
	}

private:
	int indexOf(const CMedia& media) const override
	{
		return m_lstAlbumItems.find([&](const CAlbumItem& PlayItem) {
			return &PlayItem == &media;
		});
	}

	void GetMedias(TD_MediaList& lstMedias) override
	{
		lstMedias.add(m_lstAlbumItems);
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
	list<CAlbum>& albums()
	{
		return m_lstAlbums;
	}
	const list<CAlbum>& albums() const
	{
		return m_lstAlbums;
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
	list<CSinger>& singers()
	{
		return m_lstSingers;
	}
	const list<CSinger>& singers() const
	{
		return m_lstSingers;
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
