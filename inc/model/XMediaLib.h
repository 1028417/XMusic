
#pragma once

#define __onlineMediaLib true
#if __winvc
#undef __onlineMediaLib
#define __onlineMediaLib false
#endif

class XMediaLib : public CMediaLib, public CMediaSet
{
public:
	XMediaLib(IMediaObserver& MediaObserver, CPlaylistMgr& PlaylistMgr, CSingerMgr& SingerMgr)
		: CMediaLib(MediaObserver)
		, CMediaSet(L"曲库")
		, m_PlaylistMgr(PlaylistMgr)
		, m_SingerMgr(SingerMgr)
	{
	}

private:
	CPlaylistMgr& m_PlaylistMgr;
	CSingerMgr& m_SingerMgr;

	map<string, string> m_mapFileUrl;
	
public:
    bool loadXurl(Instream& ins);

	string getUrl(CPlayItem& PlayItem);

private:
	CPath* _newSubDir(const tagFileInfo& fileInfo) override
	{
		if (strutil::matchIgnoreCase(fileInfo.strName, __medialibDir))
		{
			return NULL;
		}

		return CMediaDir::_newSubDir(fileInfo);
	}

    void GetSubSets(TD_MediaSetList& lstSubSets) override
	{
		lstSubSets.add(m_PlaylistMgr);
		lstSubSets.add(m_SingerMgr);
	}
};
