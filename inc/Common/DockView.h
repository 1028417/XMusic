
#pragma once

#include <gdiplus.h>

#include "Page.h"

#include "Guard.h"

#define __DXView 5

enum class E_TabStyle
{
	TS_HideTab = 0,
	TS_Top,
	TS_Bottom,
	TS_Left,
	TS_Right
};

struct tagViewTabStyle
{
	E_TabStyle eTabStyle = E_TabStyle::TS_HideTab;

	float fTabFontSize = 0;

	CImageList *pImglst = NULL;

	UINT uTabHeight = 0;

	UINT uTabWidth = 0;
};

class CViewTab : public CTabCtrl
{
	DECLARE_MESSAGE_MAP()

public:
	CViewTab();

private:
	const Gdiplus::Pen m_pen;

	const Gdiplus::SolidBrush m_brushSel;
	const Gdiplus::SolidBrush m_brushUnsel;

	CCompatableFont m_font;

	E_TabStyle m_eTabStyle = E_TabStyle::TS_HideTab;

	CImageList m_Imglst;

	int m_nTrackMouseFlag = -1;

	CB_TrackMouseEvent m_cbMouseEvent;
	
	CRect m_rcTabItem;

public:
	UINT getItemHeight() const;

	BOOL init(const tagViewTabStyle& TabStyle);

	void SetTabStyle(E_TabStyle eTabStyle);

	BOOL SetFontSize(float fFontSizeOffset);

	void SetTrackMouse(const CB_TrackMouseEvent& cbMouseEvent);

private:
	void OnTrackMouseEvent(E_TrackMouseEvent eMouseEvent, const CPoint& point);
	
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	void OnPaint();

	void _drawItem(CDC& dc, int nItem, CRect& rcItem);
};

enum class E_DockViewType
{
	DVT_DockLeft,
	DVT_DockTop,
	DVT_DockBottom,
	DVT_DockRight,
	DVT_DockCenter
};

struct tagViewStyle
{
	tagViewStyle(E_DockViewType t_eViewType=E_DockViewType::DVT_DockCenter)
	{
		eViewType = t_eViewType;
	}

	E_DockViewType eViewType;

	UINT uDockSize = 0;
	UINT uMaxDockSize = 0;
	UINT uMinDockSize = 0;

	UINT uStartPos = 0;
	UINT uEndPos = 0;

	bool bBorder = false;

	tagViewTabStyle TabStyle;

	bool sizeable() const
	{
		return uDockSize != uMaxDockSize || uDockSize != uMinDockSize;
	}

	void setDockSize(UINT t_uDockSize)
	{
		if (0 != uMaxDockSize)
		{
			t_uDockSize = MIN(t_uDockSize, uMaxDockSize);
		}
		if (0 != uMinDockSize)
		{
			t_uDockSize = MAX(t_uDockSize, uMinDockSize);
		}
		else
		{
			return;
		}

		uDockSize = t_uDockSize;
	}
};

class CPage;

class __CommonExt CDockView : public CPropertySheet
{
	friend class CPage;

public:
	CDockView(CWnd& wndParent);
	
	CDockView(CWnd& wndParent, const tagViewStyle& ViewStyle);

	virtual ~CDockView() = default;

	DECLARE_MESSAGE_MAP()

private:
	CCompatableFont m_font;

	tagViewStyle m_ViewStyle;
	
	CViewTab m_wndTabCtrl;

	typedef vector<CPage*> PageVector;
	PageVector m_vctPages;

public:
	void SetTabStyle(E_TabStyle eTabStyle);

	void SetTabPadding(UINT cx, UINT cy)
	{
		m_wndTabCtrl.SetPadding({ (int)cx, (int)cy });
	}
	
	void setViewStyle(const tagViewStyle& ViewStyle);

	const tagViewStyle& getViewStyle() const
	{
		return m_ViewStyle;
	}

	void setDockSize(UINT uDockSize)
	{
		m_ViewStyle.setDockSize(uDockSize);
	}

	BOOL AddPage(CPage& Page);

	BOOL RemovePage(CPage& Page);

	BOOL SetActivePage(CPage& pPage);

	BOOL SetPageTitle(CPage& Page, const CString& cstrTitle, int iImage = -1);

	void Resize(CRect& rcViewArea, bool bManual=false);
	
private:
	BOOL Create();
	
	void OnDestroy();

	afx_msg void OnSize(UINT nType, int, int);

	void resizePage(CPropertyPage& Page);
};
