#pragma once

class __MediaLibExt CPlayItem : public CMedia
{
public:
    CPlayItem() = default;

    CPlayItem(int nID, const wstring& strPath, filetime_t time, class CPlaylist& Playlist);

private:
	wstring _GetDisplayName(bool bDynamic);

	wstring GetExportFileName() override
	{
        return _GetDisplayName(true) + __wcDot + GetExtName();
	}

	void GenListItem(bool bReportView, vector<wstring>& vecText, int& iImage) override;

public:
	class CPlaylist* GetPlaylist() const;

	wstring GetPlaylistName() const;

	inline bool isPlayingItem() const
	{
		return 0 == m_pParent->m_uID;
	}

	int getSingerImg() const;

	void AsyncTask() override;
};

class __MediaLibExt CPlaylist : public CMediaSet
{
public:
    virtual ~CPlaylist() = default; // 解决qt-mac clang告警

    CPlaylist() = default;

    CPlaylist(CMediaSet& parent, int nID, const wstring& strName
              , UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport=false);

	CPlaylist(const CPlaylist& other);

	CPlaylist(CPlaylist&& other);

private:
    ArrList<CPlayItem> m_alPlayItems;
	
public:
    size_t size() const
    {
        return m_alPlayItems.size();
    }

	const ArrList<CPlayItem>& playItems() const
	{
        return m_alPlayItems;
	}

    CPlayItem& add(const CPlayItem& PlayItem)
    {
        auto& t_PlayItem = m_alPlayItems.add(PlayItem);
        t_PlayItem.m_pParent = this;
        return t_PlayItem;
    }

    template <class ITR>
    ITR erase(const ITR& itr)
    {
        return m_alPlayItems.erase(itr);
    }

    void clear()
    {
        m_alPlayItems.clear();
    }

    bool playable() const override
	{
        return m_alPlayItems;
	}

	wstring GetDisplayName();

private:
	int indexOf(const CMedia& media) const override
	{
        return m_alPlayItems.find([&](const CPlayItem& PlayItem) {
			return &PlayItem == &media;
		});
	}

	void GetMedias(TD_MediaList& lstMedias) override
	{
        lstMedias.add(m_alPlayItems);
	}

    void GenListItem(bool, vector<wstring>& vecText, int& iImage) override
	{
		iImage = (int)E_GlobalImage::GI_Playlist;
        (void)vecText;//vecText.push_back(GetDisplayName().c_str());
	}

	wstring GetExportName() override
	{
		return L"歌单" + GetDisplayName();
	}
};
