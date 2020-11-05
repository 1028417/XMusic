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

    map<wstring, wstring> m_mapSingerName;
    map<wstring, list<tagSingerImg>> m_mapFile;

#if __OnlineMediaLib
    set<tagSingerImg*> m_setOnlineFile;
    list<tagSingerImg*> m_lstDownloadFile;
    mutex m_mutex;
    XThread m_thrDownload;

private:
    bool _raiseFile(const tagSingerImg *pSingerImg);

    void _downloadFile(tagSingerImg *pSingerImg);

    void _download();
#endif

private:
    wstring _fileSingerName(cwstr strFile) const;

public:
    const map<wstring, list<tagSingerImg>>& fileMap() const
	{
		return m_mapFile;
	}

    wstring file(const tagSingerImg& singerImg) const
    {
        return m_strDir + singerImg.strFile;
    }

    bool init(cwstr strDir);

	UINT addSingerImg(cwstr strSingerName, const list<wstring>& lstFiles);

	void renameSinger(cwstr strSingerName, cwstr strNewName);

	void removeSingerImg(cwstr strSingerName);

	void clearSingerImg();

#if __OnlineMediaLib
    UINT getSingerImgCount(cwstr strSingerName);

    void downloadSingerHead(const list<wstring>& lstSingerName);

    void quitDownload();

	const tagSingerImg* getSingerHead(cwstr strSingerName);
#else

	wstring getSingerHead(cwstr strSingerName);
#endif

#if __winvc
	HBITMAP getSingerImg(cwstr strSingerName, UINT uIndex);
#else
	const tagSingerImg* getSingerImg(cwstr strSingerName, UINT uIndex, bool bIgnorePiiic);
#endif
};
