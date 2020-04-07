
#pragma once

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

	CImglst m_bigImglst;
	CImglst m_smallImglst;

	vector<UINT> m_vctSingerID;

private:
	bool _setImg(const wstring& strImg, bool bHalfToneMode=false);
	bool _setSingerImg(const wstring& strFile);

	bool _initSingerImg(UINT uSingerID, const wstring& strSingerName, const wstring& strFile);
	
	int _getSingerImgPos(UINT uSingerID) const;

public:
	wstring getImgPath(const wstring& strImgFile) const
	{
		return m_strImgDir + strImgFile + L".png";
	}

	CImglst& bigImglst()
	{
		return m_bigImglst;
	}
	CImglst& smallImglst()
	{
		return m_smallImglst;
	}
	
	UINT getSingerImgPos(UINT uSingerID) const;

	bool init(UINT uBigIconSize, UINT uSmallIconSize, UINT uTinyIconSize);

	void initSingerImg();

	bool addSingerImg(UINT uSingerID, const wstring& strSingerName, const list<wstring>& lstFiles);

	void removeSingerImg(UINT uSingerID, const wstring& strSingerName);
	
	void clearSingerImg();
};
