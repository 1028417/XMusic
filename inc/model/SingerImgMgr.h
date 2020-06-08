#pragma once

class __ModelExt CSingerImgMgr
{
public:
    CSingerImgMgr(IModelObserver& ModelObserver)
        : m_ModelObserver(ModelObserver)
    {
    }

private:
    IModelObserver& m_ModelObserver;

	wstring m_strDir;

	map<wstring, vector<wstring>> m_mapFile;

#if __OnlineMediaLib
    list<wstring> m_lstOnlineImgFile;
    mutex m_mutex;
    XThread m_thrDownload;

private:
    void _download(const CUpgradeUrl& UpgradeUrl);

public:
    void quitDownload();
#endif

public:
    bool init(cwstr strDir);

	UINT addSingerImg(cwstr strSingerName, const list<wstring>& lstFiles);

	void renameSinger(cwstr strSingerName, cwstr strNewName);

	void removeSingerImg(cwstr strSingerName);

	void clearSingerImg();

    wstring getSingerImg(cwstr strSingerName, UINT uIndex);

	void getSingerImg(prlist<wstring, UINT>& plSingerImg) const;

	bool exportSingerImg(cwstr strDstDir);
};
