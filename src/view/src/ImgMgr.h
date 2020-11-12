
#pragma once

class CImgMgr
{
public:
	CImgMgr(IModel& model) : m_model(model)
	{
	}

private:
	IModel& m_model;
	
	wstring m_strImgDir;

	CImglst m_bigImglst;
	CImglst m_smallImglst;

	vector<UINT> m_vctSingerID;

private:
	bool _setImg(cwstr strImg, bool bHalfToneMode=false);
	void _setSingerImg(CImg& img);
	bool _setSingerImg(cwstr strFile);

	void _initSingerHead(UINT uSingerID, CImg& img);
	bool _initSingerHead(UINT uSingerID, cwstr strSingerName);
	
	int _getSingerImgPos(UINT uSingerID) const;

public:
	wstring getImgPath(cwstr strImgFile) const
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

	bool addSingerImg(UINT uSingerID, cwstr strSingerName, const list<wstring>& lstFiles);

	void removeSingerImg(UINT uSingerID, cwstr strSingerName);
	
	void clearSingerImg();
};
