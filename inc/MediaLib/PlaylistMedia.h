#pragma once

class __MediaLibExt CPlayItem : public CMedia
{
public:
    CPlayItem() = default;

    CPlayItem(int nID, cwstr strPath, mediatime_t time, class CPlaylist& Playlist);

private:
#if __winvc
    wstring _GetDisplayName(bool bDynamic);

    wstring GetExportFileName() override;

    void GenListItem(E_ListViewType, vector<wstring>& vecText, TD_ListImgIdx&) override;
#endif

public:
    E_MediaType type() const override
    {
        return E_MediaType::MT_PlayItem;
    }

	class CPlaylist* GetPlaylist() const;

	wstring GetPlaylistName() const;

#if __winvc
    int getSingerImg() const;

	const CMedia* findRelatedMedia() override;

#else
    wstring GetAbsPath() const override // 兼容本地文件
    {
        cauto strPath = GetPath();
        if (0 == m_pParent->m_uID)
        {
            if (fsutil::existFile(strPath))
            {
                return strPath;
            }
        }

        return __medialib.toAbsPath(strPath, false);
    }
#endif
};

class __MediaLibExt CPlaylist : public CMediaSet
{
public:
    virtual ~CPlaylist() = default; // 解决qt-mac clang告警

    CPlaylist() = default;

    CPlaylist(CMediaSet& parent, int nID, cwstr strName
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
    ArrList<CPlayItem>& playItems()
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

#if __winvc
    wstring GetDisplayName() const
    {
        wchar_t lpBuff[8] {0};
        swprintf(lpBuff, sizeof(lpBuff), L"%02u", (UINT)index()+1);

        return wstring(lpBuff) + __CNDot + m_strName;
    }

    wstring GetExportName() override;
    {
        return strutil::eraseChars_r(GetDisplayName(), wstring(1, L' ') + __CNDot);
        //return L"歌单" + GetDisplayName();
    }
#endif

private:
    int indexOf(const IMedia& media) const override
    {
        return m_alPlayItems.find([&](const CPlayItem& PlayItem) {
            return &PlayItem == &media;
        });
    }

    void GetMedias(TD_IMediaList& lstMedias) override
    {
        lstMedias.add(m_alPlayItems);
    }
};
