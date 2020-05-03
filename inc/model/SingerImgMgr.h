#pragma once

class __ModelExt CSingerImgMgr
{
public:
    CSingerImgMgr(class CModel& model, IModelObserver& ModelObserver)
        : m_model(model)
        , m_ModelObserver(ModelObserver)
    {
    }

private:
    class CModel& m_model;

    IModelObserver& m_ModelObserver;

	wstring m_strDir;

	map<wstring, vector<wstring>> m_mapFile;

#if __OnlineMediaLib
    list<wstring> m_lstOnlineImgFile;
    mutex m_mutex;
    XThread m_thrDownload;

private:
    void _download(const string& strBaseUrl);

public:
    void quitDownload();
#endif

public:
    void init(cwstr strDir, const list<wstring>& lstOnlineSingerImg);

	UINT addSingerImg(cwstr strSingerName, const list<wstring>& lstFiles);

	void renameSinger(cwstr strSingerName, cwstr strNewName);

	void removeSingerImg(cwstr strSingerName);

	void clearSingerImg();

    wstring getSingerImg(cwstr strSingerName, UINT uIndex);

	void getSingerImg(list<wstring>& lstSingerImg) const;

	bool exportSingerImg(cwstr strDstDir);
};
