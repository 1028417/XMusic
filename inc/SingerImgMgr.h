#pragma once

class __ModelExt CSingerImgMgr
{
public:
	CSingerImgMgr() {}

private:
	wstring m_strSingerImgDir;

	map<wstring, vector<wstring>> m_mapSingerImgFile;

public:
	void init();

	UINT addSingerImg(const wstring& strSingerName, const list<wstring>& lstFiles);

	void renameSinger(const wstring& strSingerName, const wstring& strNewName);

	void removeSingerImg(const wstring& strSingerName);

	void clearSingerImg();

	bool getSingerImg(const wstring& strSingerName, UINT uIndex, wstring& strSingerImg) const;
};
