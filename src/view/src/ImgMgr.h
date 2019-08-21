
#pragma once

enum class E_GlobalImglst
{
	GIL_Big
	, GIL_Small
	, GIL_Tiny
};

class CImgMgr
{
public:
	CImgMgr(IModel& model)
		: m_model(model)
	{
	}

private:
	IModel& m_model;
	
	wstring m_strImgDir;

	CImglst m_imglst;
	CImglst m_imglstSmall;
	CImglst m_imglstTiny;

	vector<UINT> m_vctSingerID;

private:
	bool _setImg(const wstring& strFile, bool bSinger = false, int iPosReplace = -1);

	bool _initSingerImg(UINT uSingerID, const wstring& strSingerName, const wstring& strFile);
	
	int _getSingerImgPos(UINT uSingerID) const;

public:
	const wstring& getImgDir() const
	{
		return m_strImgDir;
	}

	CImglst& getImglst(E_GlobalImglst eImglstType);

	UINT getSingerImgPos(UINT uSingerID) const;

	bool init(UINT uBigIconSize, UINT uSmallIconSize, UINT uTinyIconSize);

	void initSingerImg();

	bool addSingerImg(UINT uSingerID, const wstring& strSingerName, const list<wstring>& lstFiles);

	void removeSingerImg(UINT uSingerID, const wstring& strSingerName);
	
	void clearSingerImg();
};
