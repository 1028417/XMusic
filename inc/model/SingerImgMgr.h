#pragma once

class __ModelExt CSingerImgMgr
{
public:
    CSingerImgMgr(class CSingerMgr& SingerMgr, IModelObserver& ModelObserver)
        : m_SingerMgr(SingerMgr)
        , m_ModelObserver(ModelObserver)
    {
    }

private:
    class CSingerMgr& m_SingerMgr;

    IModelObserver& m_ModelObserver;

	wstring m_strDir;

    map<wstring, vector<tagSingerImg>> m_mapFile;

#if __OnlineMediaLib
    map<wstring, tagSingerImg> m_mapOnlineFile;
    list<wstring> m_lstDownloadFile;
    mutex m_mutex;
    XThread m_thrDownload;

private:
    void _download();

public:
    void quitDownload();
#endif

public:
	cwstr dir() const
	{
		return m_strDir;
	}

    const map<wstring, vector<tagSingerImg>>& fileMap() const
	{
		return m_mapFile;
	}

    bool init(cwstr strDir);

	UINT addSingerImg(cwstr strSingerName, const list<wstring>& lstFiles);

	void renameSinger(cwstr strSingerName, cwstr strNewName);

	void removeSingerImg(cwstr strSingerName);

	void clearSingerImg();

#if __OnlineMediaLib
    void downloadSingerHead(const list<wstring>& lstSingerName);
#endif

	wstring getSingerHead(cwstr strSingerName);

    wstring getSingerImg(cwstr strSingerName, UINT uIndex);
};
