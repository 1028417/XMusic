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

#if !__winvc
    list<wstring> m_lstOnlineImgFile;
    mutex m_mutex;
    XThread m_thrDownload;

private:
    void _download(const string& strBaseUrl);
#endif

public:
	void init(const wstring& strDir, const string& strBaseUrl, const list<wstring>& lstOnlineImgFile);

	UINT addSingerImg(const wstring& strSingerName, const list<wstring>& lstFiles);

	void renameSinger(const wstring& strSingerName, const wstring& strNewName);

	void removeSingerImg(const wstring& strSingerName);

	void clearSingerImg();

    wstring getSingerImg(const wstring& strSingerName, UINT uIndex);

	void getSingerImg(SList<wstring>& lstSingerImg) const;

	bool exportSingerImg(const wstring& strDstDir);
};
