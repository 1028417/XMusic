#pragma once

class __MediaLibExt CMediaMixture
{
public:
	CMediaMixture()
	{
	}

	CMediaMixture(CMedia& media)
		: m_strTitle(media.GetTitle())
	{
		Add(media);
	}

    bool operator==(const CMediaMixture& other) const
    {
        return &other == this;
    }

private:
	wstring m_strTitle;

	CMediaTime m_addTime;

	CPlayItem *m_pPlayItem = NULL;
	wstring m_strPlaylistName;

	CAlbumItem *m_pAlbumItem = NULL;
	wstring m_strAlbumName;
	wstring m_strSingerName;

public:
	bool Add(CMedia& media);

	void UpdateTitle();

	bool Remove(CMedia *pMedia);

public:
	cwstr GetTitle() const
	{
		return m_strTitle;
	}

	const CMediaTime& GetAddTime() const
	{
		return m_addTime;
	}

	CPlayItem* GetPlayItem() const
	{
		return m_pPlayItem;
	}

	cwstr GetPlaylistName() const
	{
		return m_strPlaylistName;
	}

	CAlbumItem* GetAlbumItem() const
	{
		return m_pAlbumItem;
	}

	CAlbum* GetAlbum() const;

	cwstr GetAlbumName() const
	{
		return m_strAlbumName;
	}

	CSinger* GetSinger() const;

	cwstr GetSingerName() const
	{
		return m_strSingerName;
	}

	CMedia* GetMedia() const;

	wstring GetDir() const;

	wstring GetPath() const;

	E_MediaFileType GetFileType() const;

	wstring GetExtName() const;

	wstring GetSingerAlbumString() const;
};
