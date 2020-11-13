#pragma once

enum class E_SortMediaType
{
	SMT_Default = 0
	
	, SMT_ByPath

	, SMT_ByTitle

	, SMT_ByFileType

	, SMT_ByAddTime

	, SMT_ByPlaylist
	
	, SMT_BySinger
	
	, SMT_ByCustom
};

using TD_MediaMixtureVector = PtrArray<CMediaMixture>;

using CB_SortMediaMixture = __CB_Sort_T<const CMediaMixture&>;

class __ModelExt CMediaMixer
{
public:
	CMediaMixer(CB_SortMediaMixture cbSort=NULL)
		: m_cbSort(cbSort)
		, m_vecData(m_mapSortData.insert(E_SortMediaType::SMT_Default, TD_MediaMixtureVector()))
	{
	}

private:
	CB_SortMediaMixture m_cbSort;

	TD_MediaList m_lstMedias;
	
	SMultiHashMap<wstring, CMediaMixture> m_mapData;

	SMap<E_SortMediaType, TD_MediaMixtureVector> m_mapSortData;

	TD_MediaMixtureVector& m_vecData;

private:
	void _sort(TD_MediaMixtureVector& vecData, E_SortMediaType eSortType);

	bool _compare(E_SortMediaType eSortType, const CMediaMixture& lhs, const CMediaMixture& rhs);

	void _erase(SSet<CMedia*>& setMedias, cfn_void_t<CMediaMixture&> cb);

	void _remove(const CMediaMixture& MediaMixture);

public:
	const TD_MediaList& getMedias()
	{
		return m_lstMedias;
	}

	void clear();

	void remove(const TD_MediaList& lstMedias);

	void update(const TD_MediaList& lstMedias);

	CMediaMixer& add(CMedia& media);

	const TD_MediaMixtureVector& add(const TD_MediaList& lstMedias);
	
	const TD_MediaMixtureVector& get(E_SortMediaType eSortType = E_SortMediaType::SMT_Default);

	bool getMediaMixture(cfn_void_t<const CMediaMixture&> cb, UINT uIndex
		, E_SortMediaType eSortType = E_SortMediaType::SMT_Default);

	bool getMedia(cfn_void_t<CMedia&> cb, UINT uIndex
		, E_SortMediaType eSortType = E_SortMediaType::SMT_Default);
};
