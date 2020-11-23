
#include "stdafx.h"

#include "ImgMgr.h"

#define __Size(size) CSize(size, size)

static const wstring g_lpImg[]{
	L"dir"
	, L"attachdir"
	, L"file"
	, L"attachfile"
	, L"wholeTrack"

	, L"playlist"
	, L"playitem"

	, L"singergroup"
	, L"singerdefault"
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
	
	for (cauto strImg : g_lpImg)
	{
		__EnsureReturn(_setImg(strImg), false);
	}

	return true;
}

bool CImgMgr::_setImg(cwstr strImg, bool bHalfToneMode)
{
	Gdiplus::Bitmap img(getImgPath(strImg).c_str());
	HICON hIcon = NULL;
	(void)img.GetHICON(&hIcon);
	if (NULL == hIcon)
	{
		return false;
	}

	m_bigImglst.SetIcon(hIcon);
	m_smallImglst.SetIcon(hIcon);

	/*CImg img;
	__AssertReturn(img.Load(getImgPath(strImg).c_str()), false);
	m_bigImglst.SetImg(img, E_ImgFixMode::IFM_None);
	m_smallImglst.SetImg(img, E_ImgFixMode::IFM_None);*/

	return true;
}

inline void CImgMgr::_setSingerImg(CImg& img)
{
	m_bigImglst.SetImg(img, E_ImgFixMode::IFM_Inner);
	m_smallImglst.SetImg(img, E_ImgFixMode::IFM_Inner);
}

bool CImgMgr::_setSingerImg(cwstr strFile)
{
	CImg img;
	__EnsureReturn(img.Load(strFile.c_str()), false);
	_setSingerImg(img);

	return true;
}

void CImgMgr::initSingerImg()
{
	CSignal sgn(false, true);
	list<UINT> lstSingerID;
	list<CImg> lstImg;
	mtutil::concurrence([&]() {
		for (auto pSinger : m_model.getSingerMgr().singers())
		{
			cauto strHeadImg = m_model.getSingerImgMgr().getSingerHead(pSinger->m_strName);
			if (strHeadImg.empty())
			{
				continue;
			}

			CImg img;
			if (!img.Load(strHeadImg.c_str()))
			{
				continue;
			}
			
			sgn.set([&]() {
				lstSingerID.push_back(pSinger->m_uID);
				lstImg.emplace_back(img);
			});
			img.Detach();
		}

		sgn.stop();
	}, [&]() {
		while (true)
		{
			list<UINT> t_lstSingerID;
			list<CImg> t_lstImg;
			if (sgn.wait([&]() {
				t_lstSingerID.swap(lstSingerID);
				t_lstImg.swap(lstImg);
			}))
			{
				_initSingerHead(t_lstSingerID, t_lstImg);
				continue;
			}
			
			break;
		}

		if (!lstSingerID.empty())
		{
			_initSingerHead(lstSingerID, lstImg);
		}
	});
}

bool CImgMgr::_initSingerHead(UINT uSingerID, cwstr strSingerName)
{
	cauto strHeadImg = m_model.getSingerImgMgr().getSingerHead(strSingerName);
	__EnsureReturn(!strHeadImg.empty(), false);

	__EnsureReturn(_setSingerImg(strHeadImg), false);

	m_vctSingerID.push_back(uSingerID);

	return true;
}

void CImgMgr::_initSingerHead(UINT uSingerID, CImg& img)
{
	_setSingerImg(img);

	m_vctSingerID.push_back(uSingerID);
}

void CImgMgr::_initSingerHead(const list<UINT>& lstSingerID, list<CImg>& lstImg)
{
	m_bigImglst.SetImg(lstImg, E_ImgFixMode::IFM_Inner);
	m_smallImglst.SetImg(lstImg, E_ImgFixMode::IFM_Inner);

	m_vctSingerID.insert(m_vctSingerID.end(), lstSingerID.begin(), lstSingerID.end());
}

bool CImgMgr::addSingerImg(UINT uSingerID, cwstr strSingerName, const list<wstring>& lstFiles)
{
	UINT uRet = m_model.getSingerImgMgr().addSingerImg(strSingerName, lstFiles);
	__EnsureReturn(uRet != 0, false);

	if (_getSingerImgPos(uSingerID) < 0)
	{		
		__EnsureReturn(_initSingerHead(uSingerID, strSingerName), false);
	}
	
	return true;
}

void CImgMgr::removeSingerImg(UINT uSingerID, cwstr strSingerName)
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

	m_model.getSingerImgMgr().removeSingerImg(strSingerName);
}

void CImgMgr::clearSingerImg()
{
	for (UINT uIdx = 0; uIdx < m_vctSingerID.size(); uIdx++)
	{
		m_bigImglst.Remove(__SingerImgStartPos);
		m_smallImglst.Remove(__SingerImgStartPos);
	}

	m_vctSingerID.clear();
}
