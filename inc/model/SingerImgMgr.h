#pragma once

class __ModelExt CSingerImgMgr
{
public:
	CSingerImgMgr() {}

private:
	wstring m_strSingerImgDir;

	map<wstring, vector<wstring>> m_mapSingerImgFile;

#if !__winvc
private:
    unordered_map<string, string> m_mapUrl;

public:
    void setShareUrl(unordered_map<string, string>&& mapUrl)
    {
        m_mapUrl.swap(mapUrl);
    }
#endif

public:
	void init(const wstring& strSingerImgDir);

	UINT addSingerImg(const wstring& strSingerName, const list<wstring>& lstFiles);

	void renameSinger(const wstring& strSingerName, const wstring& strNewName);

	void removeSingerImg(const wstring& strSingerName);

	void clearSingerImg();

	bool getSingerImg(const wstring& strSingerName, UINT uIndex, wstring& strSingerImg) const;

	void exportSingerImg(const wstring& strDir);
};
