
#include "stdafx.h"

#include "ImgMgr.h"

#include <algorithm>

#define __Size(size) CSize(size, size)

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

	return m_uSingerImgOffset + int(itr - m_vctSingerID.begin());
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
	m_strImgDir = fsutil::workDir() + L"/img/";

	__AssertReturn(m_imglst.Init(__Size(uBigIconSize)), false);
	__AssertReturn(m_imglstSmall.Init(__Size(uSmallIconSize)), false);
	__AssertReturn(m_imglstTiny.Init(__Size(uTinyIconSize)), false);
	
	wstring lpImgFile[] = {
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
	for (auto& strImgFile : lpImgFile)
	{
		__AssertReturn(_setImg(m_strImgDir + strImgFile), false);
	}

	m_uSingerImgOffset = sizeof(lpImgFile)/sizeof(*lpImgFile);

	return true;
}

bool CImgMgr::_setImg(const wstring& strFile, bool bSinger, int iPosReplace)
{
	CImg img;
	__EnsureReturn(img.Load(strFile.c_str()), false);

	m_imglst.SetImg(img, true, &g_rcMargin, iPosReplace);
	m_imglstSmall.SetImg(img, true, &g_rcMargin, iPosReplace);
	m_imglstTiny.SetImg(img, bSinger, &g_rcMargin, iPosReplace);

	return true;
}

bool CImgMgr::initSingerImg(UINT uSingerID, const wstring& strSingerName, const wstring& strFile)
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
		wstring strSingerImg;
		__EnsureReturn(m_model.getSingerImgMgr().getSingerImg(strSingerName, 0, strSingerImg), false);
		
		__EnsureReturn(initSingerImg(uSingerID, strSingerName, strSingerImg), false);
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
	UINT uImgPos = m_uSingerImgOffset + int(itr - m_vctSingerID.begin());

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
		UINT uImgPos = m_uSingerImgOffset + uIdx;

		m_imglst.Remove(uImgPos);
		m_imglstSmall.Remove(uImgPos);
		m_imglstTiny.Remove(uImgPos);
	}

	m_vctSingerID.clear();

	m_model.getSingerImgMgr().clearSingerImg();
}

//HBITMAP CImgMgr::getBitmap(UINT uImgPos, E_GlobalImglst eImglstType)
//{
//	IMAGEINFO ImageInfo;
//	memset(&ImageInfo, 0, sizeof(ImageInfo));
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
