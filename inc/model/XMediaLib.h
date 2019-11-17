
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

    unordered_map<string, string> m_mapFileUrl;
	
public:
    bool loadXurl(Instream& ins);

    bool checkFileUrl(const string& strFileTitle);
    bool checkFileUrl(const wstring& strFileTitle)
    {
       return checkFileUrl(strutil::toUtf8(strFileTitle));
    }

    string getFileUrl(const wstring& strFileTitle);

    bool loadSnapshot(Instream& ins);

private:
    void _insertFileUrl(const string& strFileName, const string& strUrl);

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

    CMediaDir* _genSnapshotDir(CMediaDir& parent, const JValue& jRoot);
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
