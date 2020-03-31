
#include "stdafx.h"

#include "ImgMgr.h"

#define __Size(size) CSize(size, size)

static const wstring g_lpImg[]{
	L"dir"
	, L"attachdir"
	, L"file"
	, L"wholeTrack"

	, L"playlist"
	, L"playitem"

	, L"singergroup"
	, __singerDefImg
	, L"album"
	, L"media"
};

#define __SingerImgStartPos (sizeof g_lpImg / sizeof *g_lpImg)

static const RECT g_rcMargin{ 2, 2, 2, 2 };

//HBITMAP CImgMgr::getBitmap(E_GlobalImglst eImglstType, UINT uImgPos)
//{
//	IMAGEINFO ImageInfo;
//	memzero(ImageInfo);
//
//	if (!GetImglst(eImglstType).GetImageInfo(uImgPos, &ImageInfo))
//	{
//		return NULL;
//	}
//
//	return ImageInfo.hbmImage;
//}

int CImgMgr::_getSingerImgPos(UINT uSingerID) const
{
	auto itr = std::find(m_vctSingerID.begin(), m_vctSingerID.end(), uSingerID);
	if (itr == m_vctSingerID.end())
	{
		return -1;
	}

	return __SingerImgStartPos + int(itr - m_vctSingerID.begin());
}

UINT CImgMgr::getSingerImgPos(UINT uSingerID) const
{
	int iImgPos = _getSingerImgPos(uSingerID);
	if (iImgPos < 0)
	{
		return (UINT)E_GlobalImage::GI_SingerDefault;
	}

	return (UINT)iImgPos;
}

bool CImgMgr::init(UINT uBigIconSize, UINT uSmallIconSize, UINT uTinyIconSize)
{
	m_strImgDir = fsutil::getModuleSubPath(L"img\\");

	CPngImage png;

	__AssertReturn(m_bigImglst.Init(__Size(uBigIconSize)), false);
	__AssertReturn(m_smallImglst.Init(__Size(uSmallIconSize)), false);
	__AssertReturn(m_tabImglst.Init(__Size(uTinyIconSize)), false);
	
	for (cauto strImg : g_lpImg)
	{
		__EnsureReturn(_setImg(strImg), false);
	}

	return true;
}

bool CImgMgr::_setImg(const wstring& strImg, bool bHalfToneMode)
{	
	CImg img;
	__AssertReturn(img.Load(getImgPath(strImg).c_str()), false);

	m_bigImglst.SetImg(img, false, &g_rcMargin);
	m_smallImglst.SetImg(img, false, &g_rcMargin);
	m_tabImglst.SetImg(img, false, &g_rcMargin);

	return true;
}

bool CImgMgr::_setSingerImg(const wstring& strFile)
{
	CImg img;
	__EnsureReturn(img.Load(strFile.c_str()), false);

	m_bigImglst.SetImg(img, true, E_ImgFixMode::IFM_Inner);
	m_smallImglst.SetImg(img, true, E_ImgFixMode::IFM_Inner);
	m_tabImglst.SetImg(img, true, E_ImgFixMode::IFM_Inner);

	return true;
}

void CImgMgr::initSingerImg()
{
	TD_MediaSetList arrSingers;
	m_model.getSingerMgr().GetAllSinger(arrSingers);
	arrSingers([&](CMediaSet& Singer) {
		cauto strSingerImg = m_model.getSingerImgMgr().getSingerImg(Singer.m_strName, 0);
		if (!strSingerImg.empty())
		{
			(void)_initSingerImg(Singer.m_uID, Singer.m_strName, strSingerImg);
		}
	});
}

bool CImgMgr::_initSingerImg(UINT uSingerID, const wstring& strSingerName, const wstring& strFile)
{
	__EnsureReturn(_setSingerImg(strFile), false);
	
	m_vctSingerID.push_back(uSingerID);

	return true;
}

bool CImgMgr::addSingerImg(UINT uSingerID, const wstring& strSingerName, const list<wstring>& lstFiles)
{
	UINT uRet = m_model.getSingerImgMgr().addSingerImg(strSingerName, lstFiles);
	__EnsureReturn(uRet != 0, false);

	if (_getSingerImgPos(uSingerID) < 0)
	{
		cauto strSingerImg = m_model.getSingerImgMgr().getSingerImg(strSingerName, 0);
		__EnsureReturn(!strSingerImg.empty(), false);
		
		__EnsureReturn(_initSingerImg(uSingerID, strSingerName, strSingerImg), false);
	}
	
	return true;
}

void CImgMgr::removeSingerImg(UINT uSingerID, const wstring& strSingerName)
{
	auto itr = std::find(m_vctSingerID.begin(), m_vctSingerID.end(), uSingerID);
	if (itr == m_vctSingerID.end())
	{
		return;
	}
	UINT uImgPos = __SingerImgStartPos + int(itr - m_vctSingerID.begin());

	m_vctSingerID.erase(itr);

	m_bigImglst.Remove(uImgPos);
	m_smallImglst.Remove(uImgPos);
	m_tabImglst.Remove(uImgPos);

	m_model.getSingerImgMgr().removeSingerImg(strSingerName);
}

void CImgMgr::clearSingerImg()
{
	for (UINT uIdx = 0; uIdx < m_vctSingerID.size(); uIdx++)
	{
		m_bigImglst.Remove(__SingerImgStartPos);
		m_smallImglst.Remove(__SingerImgStartPos);
		m_tabImglst.Remove(__SingerImgStartPos);
	}

	m_vctSingerID.clear();
}
