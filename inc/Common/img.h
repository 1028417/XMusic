
#pragma once

#include <atlimage.h>

#include <gdiplus.h>

#define __crWhite RGB(255, 255, 255)

class __CommonExt CCompDC
{
public:
	CCompDC() = default;

	~CCompDC()
	{
		destroy();
	}

	UINT m_cx = 0;
	UINT m_cy = 0;

private:
	CDC m_dc;
	CBitmap m_Bitmap;

	HBITMAP m_hbmpPrev = NULL;

	//CGdiObject *m_prevBrush = NULL;
	//CGdiObject *m_prevPen = NULL;
	//CGdiObject *m_prevFont = NULL;

private:
	bool _create(HDC hDC=NULL);

public:
	CDC* operator ->()
	{
		return &m_dc;
	}

	CDC& getDC()
	{
		return m_dc;
	}

	void getBitmap(cfn_void_t<CBitmap&> cb);

	bool create(UINT cx, UINT cy, HDC hDC=NULL);

	bool create(HBITMAP hBitmap, HDC hDC = NULL);

	bool create(HICON hIcon, HDC hDC=NULL);

	void destroy();
};

using TD_IconVec = vector<HICON>;

enum class E_ImgFixMode
{
	IFM_None
	, IFM_Inner
	, IFM_Outer
	, IFM_Width
	, IFM_Height
};

class __CommonExt CImg : public CImage
{
public:
	CImg() = default;
	
public:
	BOOL Load(cwstr strFile);

	BOOL StretchBltEx(HDC hDC, const RECT& rc, E_ImgFixMode eFixMode);
};

enum class E_ImglstType
{
	ILT_Both = -1
	, ILT_Normal = 0
	, ILT_Small = LVSIL_SMALL
};

class __CommonExt CImglst : public CImageList
{
public:
	CImglst(COLORREF crBkg = CLR_NONE)
		: m_crBkg(crBkg)
	{
	}

private:
	COLORREF m_crBkg = CLR_NONE;

	UINT m_cx = 0;
	UINT m_cy = 0;

	CCompDC m_adpDC;

public:
	UINT imgWidth() const
	{
		return m_cx;
	}
	UINT imgHeight() const
	{
		return m_cy;
	}

	BOOL Init(UINT cx, UINT cy);

	BOOL Init(const CSize& size, const TD_IconVec& vecIcons = {});

	BOOL Init(CBitmap& bitmap);

	void SetIcon(HICON hIcon, int nPosReplace = -1);

	void SetBitmap(CBitmap& bitmap, int nPosReplace = -1);

	void SetImg(Gdiplus::Image& img, int nPosReplace = -1);
	void SetImg(list<Gdiplus::Image>& lstImg, int nPosReplace = -1);

	void SetImg(list<CImg>& lstImg, int nPosReplace = -1)
	{
		SetImg((list<Gdiplus::Image>&)lstImg, nPosReplace);
	}

	void SetImg(CImg& img, E_ImgFixMode eFixMode, int nPosReplace = -1);
	void SetImg(list<CImg>& lstImg, E_ImgFixMode eFixMode, int nPosReplace = -1);
};
