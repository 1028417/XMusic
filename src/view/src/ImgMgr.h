
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

	UINT m_uSingerImgOffset = 0;

	vector<UINT> m_vctSingerID;

	CImglst m_imglst;
	CImglst m_imglstSmall;
	CImglst m_imglstTiny;

private:
	int _getSingerImgPos(UINT uSingerID) const;

	bool _setImg(const wstring& strFile, bool bSinger=false, int iPosReplace=-1);
	
public:
	const wstring& getImgDir() const
	{
		return m_strImgDir;
	}

	CImglst& getImglst(E_GlobalImglst eImglstType);

	UINT getSingerImgPos(UINT uSingerID) const;

	bool init(UINT uBigIconSize, UINT uSmallIconSize, UINT uTinyIconSize);

	bool initSingerImg(UINT uSingerID, const wstring& strSingerName, const wstring& strFile);

	bool addSingerImg(UINT uSingerID, const wstring& strSingerName, const list<wstring>& lstFiles);

	void removeSingerImg(UINT uSingerID, const wstring& strSingerName);
	
	void clearSingerImg();
};
