#pragma once

class __ModelExt CSingerImgMgr
{
public:
	CSingerImgMgr() {}

private:
	wstring m_strDir;

	map<wstring, vector<wstring>> m_mapFile;

public:
	void init(const wstring& strSingerImgDir, const list<wstring>& lstOnlineImgFile);

	UINT addSingerImg(const wstring& strSingerName, const list<wstring>& lstFiles);

	void renameSinger(const wstring& strSingerName, const wstring& strNewName);

	void removeSingerImg(const wstring& strSingerName);

	void clearSingerImg();

	bool getSingerImg(const wstring& strSingerName, UINT uIndex, wstring& strSingerImg) const;

	void getSingerImg(SList<wstring>& lstSingerImg) const;
};
