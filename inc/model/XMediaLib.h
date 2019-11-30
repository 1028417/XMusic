
#pragma once

class __ModelExt XMediaLib : public CMediaLib, public CMediaSet
{
public:
	static bool m_bOnlineMediaLib;

	XMediaLib(class IModelObserver& ModelObserver, CPlaylistMgr& PlaylistMgr, CSingerMgr& SingerMgr);

private:
	class IModelObserver& m_ModelObserver;

	CPlaylistMgr& m_PlaylistMgr;
	CSingerMgr& m_SingerMgr;

#if !__winvc
private:
    list<JValue> m_lstSnapshot;

    unordered_map<string, string> m_mapXurl;
    unordered_map<string, string> m_mapShareUrl;

public:
    void addShareUrl(const unordered_map<string, string>& mapUrl)
    {
        m_mapShareUrl.insert(mapUrl.begin(), mapUrl.end());
    }

    bool loadXurl(Instream& ins);

    bool checkUrl(const string& strFileTitle);
    bool checkUrl(const wstring& strFileTitle)
    {
       return checkUrl(strutil::toUtf8(strFileTitle));
    }

    string getXurl(const wstring& strFileTitle);
    string getShareUrl(const wstring& strFileTitle);

    bool loadSnapshot(Instream& ins);

private:
    void _insertUrl(const string& strFileName, const string& strUrl);

    void _onFindFile(TD_PathList& paSubDir, TD_XFileList& paSubFile) override;

    CMediaDir* _genSnapshotDir(CMediaDir& parent, const JValue& jRoot);
#endif

public:
    void GetSubSets(TD_MediaSetList& lstSubSets) override
	{
		lstSubSets.add(m_PlaylistMgr);
		lstSubSets.add(m_SingerMgr);
	}

	CMedia* findRelatedMedia(const wstring& strPath, E_MediaSetType eMediaSetType, CMediaSet*& pSinger) override;

	void renameMedia(IMedia& media, const wstring& strNewName) override;

	UINT checkDuration(IMedia& media, long long& nFileSize) override;

	UINT getSingerImgPos(UINT uSingerID) override;
};
