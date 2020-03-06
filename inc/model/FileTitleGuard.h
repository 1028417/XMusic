#pragma once

class __ModelExt CFileTitleGuard
{
public:
	CFileTitleGuard(class CSingerMgr& SingerMgr);
	
private:
	class CSingerMgr& m_SingerMgr;

	PairList<wstring, wstring> m_plSingerInfo;

private:
	wstring _matchSinger(wstring& strFileTitle);

public:
	static wstring genCollateSingerName(const wstring& strSingerName);

	static void genCollateTitle(wstring& strFileTitle);
	static wstring genCollateTitle_r(const wstring& strFileTitle);

	static void genDisplayTitle(wstring& strPath);
	static wstring genDisplayTitle_r(const wstring& strFileTitle);

	wstring matchSinger(const wstring& strFileTitle);
	wstring matchSinger(IMedia& media);
};
