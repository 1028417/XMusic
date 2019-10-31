
#pragma once

#if !__winvc
#define __OnlineMediaLib 1
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
    bool onlineUpdate();

	string getUrl(CPlayItem& PlayItem);

private:
    bool _loadXurl(Instream& ins);

	bool _loadXurl(const string& strText);

private:
	CPath* _newSubDir(const tagFileInfo& fileInfo) override
	{
		if (strutil::matchIgnoreCase(fileInfo.strName, __xmedialib))
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
