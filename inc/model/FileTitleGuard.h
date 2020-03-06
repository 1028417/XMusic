#pragma once

class __ModelExt CFileTitleGuard
{
public:
	CFileTitleGuard(class CSingerMgr& SingerMgr);
	
private:
	class CSingerMgr& m_SingerMgr;

	PairList<wstring, wstring> m_plSingerInfo;

private:
	wstring _matchSinger(const wstring& strFileTitle);

public:
	static wstring transSingerName(const wstring& strSingerName);

	wstring matchSinger(const wstring& strFileTitle);
	wstring matchSinger(IMedia& media);

	static void genCollateTitle(wstring& strFileTitle);
	static wstring genCollateTitle_r(const wstring& strFileTitle);

	static void genDisplayTitle(wstring& strPath);
	static wstring genDisplayTitle_r(const wstring& strFileTitle);
};
