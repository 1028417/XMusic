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

	CMediaTime m_MediaTime;

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
	const wstring& GetTitle() const
	{
		return m_strTitle;
	}

	const CMediaTime& GetMediaTime() const
	{
		return m_MediaTime;
	}

	CPlayItem* GetPlayItem() const
	{
		return m_pPlayItem;
	}

	const wstring& GetPlaylistName() const
	{
		return m_strPlaylistName;
	}

	CAlbumItem* GetAlbumItem() const
	{
		return m_pAlbumItem;
	}

	CAlbum* GetAlbum() const;

	const wstring& GetAlbumName() const
	{
		return m_strAlbumName;
	}

	CSinger* GetSinger() const;

	const wstring& GetSingerName() const
	{
		return m_strSingerName;
	}

	CMedia* GetMedia() const;

	wstring GetDir() const;

	wstring GetPath() const;

	E_MediaFileType GetFileType() const;

	wstring GetFileTypeString() const;

	wstring GetSingerAlbumString() const;
};
