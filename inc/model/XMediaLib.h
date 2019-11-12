
#pragma once

class __ModelExt XMediaLib : public CMediaLib, public CMediaSet
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

#if !__winvc
private:
	JValue m_jSnapshot;

	map<string, string> m_mapFileUrl;
	
public:
    bool loadXurl(Instream& ins);

	string getUrl(CPlayItem& PlayItem);

    bool loadSnapshot(Instream& ins);

private:
    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;
#endif

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
