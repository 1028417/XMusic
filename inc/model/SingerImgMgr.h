#pragma once

class __ModelExt CSingerImgMgr
{
public:
	CSingerImgMgr() {}

private:
	wstring m_strDir;

	map<wstring, vector<wstring>> m_mapFile;

    list<wstring> m_lstOnlineImgFile;
    mutex m_mutex;
    XThread m_thrDownload;

public:
    void close()
    {
        m_thrDownload.cancel();
    }

	void init(const wstring& strDir, const string& strBaseUrl, const list<wstring>& lstOnlineImgFile);

	UINT addSingerImg(const wstring& strSingerName, const list<wstring>& lstFiles);

	void renameSinger(const wstring& strSingerName, const wstring& strNewName);

	void removeSingerImg(const wstring& strSingerName);

	void clearSingerImg();

	wstring getSingerImg(const wstring& strSingerName, UINT uIndex) const;

	void getSingerImg(SList<wstring>& lstSingerImg) const;
};
