
#pragma once

class __ModelExt XMediaLib : public CMediaLib, public CMediaSet
{
public:
	static bool m_bOnlineMediaLib;

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
    list<JValue> m_lstSnapshot;

    unordered_map<string, string> m_mapXurl;

    map<string, string> m_mapShareUrl;

public:
    bool loadShare(Instream& ins);

    bool loadXurl(Instream& ins);

    bool checkUrl(const string& strFileTitle);
    bool checkUrl(const wstring& strFileTitle)
    {
       return checkUrl(strutil::toUtf8(strFileTitle));
    }

    string getUrl(const wstring& strFileTitle);

    bool loadSnapshot(Instream& ins);

private:
    void _insertUrl(const string& strFileName, const string& strUrl);

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

    CMediaDir* _genSnapshotDir(CMediaDir& parent, const JValue& jRoot);
#endif

private:
    void GetSubSets(TD_MediaSetList& lstSubSets) override
	{
		lstSubSets.add(m_PlaylistMgr);
		lstSubSets.add(m_SingerMgr);
	}
};
