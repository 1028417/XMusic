#pragma once

class __MediaLibExt CPlayItem : public CMedia
{
public:
    CPlayItem() {}

    CPlayItem(int nID, const wstring& strPath, mediatime_t time, class CPlaylist& Playlist);

private:
	wstring _GetDisplayName(bool bDynamic);

	wstring GetExportFileName() override
	{
		return _GetDisplayName(true) + L"." + GetFileTypeString();
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

    CMedia* CheckRelatedMedia();
};

class __MediaLibExt CPlaylist : public CMediaSet
{
public:
    virtual ~CPlaylist() {} // 解决qt-mac clang告警

	CPlaylist() {}

    CPlaylist(CMediaSet& parent, int nID, const wstring& strName
              , UINT uLanguage=0, bool bDisableDemand=false, bool bDisableExport=false);

	CPlaylist(const CPlaylist& other);

	CPlaylist(CPlaylist&& other);

private:
	ArrList<CPlayItem> m_lstPlayItems;
	
public:
	ArrList<CPlayItem>& playItems()
	{
		return m_lstPlayItems;
	}
	const ArrList<CPlayItem>& playItems() const
	{
		return m_lstPlayItems;
	}

	bool available() override
	{
		return !m_lstPlayItems.empty();
	}

private:
	int indexOf(const CMedia& media) const override
	{
		return m_lstPlayItems.find([&](const CPlayItem& PlayItem) {
			return &PlayItem == &media;
		});
	}

	void GetMedias(TD_MediaList& lstMedias) override
	{
		lstMedias.add(m_lstPlayItems);
	}

	wstring _GetDisplayName();

    void GenListItem(bool, vector<wstring>& vecText, int& iImage) override
	{
		iImage = (int)E_GlobalImage::GI_Playlist;
		vecText.push_back(_GetDisplayName().c_str());
	}

	wstring GetExportName() override
	{
		return L"歌单" + _GetDisplayName();
	}
};
