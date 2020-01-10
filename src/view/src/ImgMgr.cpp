
#include "stdafx.h"

#include "ImgMgr.h"

#define __Size(size) CSize(size, size)

#define __SingerImgStartPos (sizeof g_lpImgFile / sizeof *g_lpImgFile)

static const wstring g_lpImgFile[] = {
	L"dir.png"
	, L"dirLink.png"
	, L"file.png"
	, L"wholeTrack.png"

	, L"playlist.png"
	, L"playitem.png"

	, L"singergroup.png"
	, L"singerdefault.png"
	, L"album.png"
	, L"albumitem.png"
};

static const RECT g_rcMargin{ 0, 1, 0, 1 };

CImglst& CImgMgr::getImglst(E_GlobalImglst eImglstType)
{
	if (E_GlobalImglst::GIL_Small == eImglstType)
	{
		return m_imglstSmall;
	}
	else if (E_GlobalImglst::GIL_Tiny == eImglstType)
	{
		return m_imglstTiny;
	}
	else
	{
		return m_imglst;
	}
}

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

	__AssertReturn(m_imglst.Init(__Size(uBigIconSize)), false);
	__AssertReturn(m_imglstSmall.Init(__Size(uSmallIconSize)), false);
	__AssertReturn(m_imglstTiny.Init(__Size(uTinyIconSize)), false);
	
	for (cauto strImgFile : g_lpImgFile)
	{
		__AssertReturn(_setImg(m_strImgDir + strImgFile), false);
	}

	return true;
}

bool CImgMgr::_setImg(const wstring& strFile, bool bSinger, int nPosReplace)
{
	CImg img;
	__EnsureReturn(img.Load(strFile.c_str()), false);

	m_imglst.SetImg(img, true, &g_rcMargin, nPosReplace);
	m_imglstSmall.SetImg(img, true, &g_rcMargin, nPosReplace);
	m_imglstTiny.SetImg(img, bSinger, &g_rcMargin, nPosReplace);

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
	__EnsureReturn(_setImg(strFile, true), false);
	
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

	m_imglst.Remove(uImgPos);
	m_imglstSmall.Remove(uImgPos);
	m_imglstTiny.Remove(uImgPos);

	m_model.getSingerImgMgr().removeSingerImg(strSingerName);
}

void CImgMgr::clearSingerImg()
{
	for (UINT uIdx = 0; uIdx < m_vctSingerID.size(); uIdx++)
	{
		m_imglst.Remove(__SingerImgStartPos);
		m_imglstSmall.Remove(__SingerImgStartPos);
		m_imglstTiny.Remove(__SingerImgStartPos);
	}

	m_vctSingerID.clear();
}

//HBITMAP CImgMgr::getBitmap(UINT uImgPos, E_GlobalImglst eImglstType)
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
//
//HICON CImgMgr::GetIcon(UINT uImgPos, E_GlobalImglst eImglstType)
//{
//	return GetImglst(eImglstType).ExtractIcon(uImgPos);
//}
