#pragma once

#include "Page.h"

#define BkgColor_Select RGB(204, 232, 255)
#define BkgColor_Hit RGB(229, 243, 255)

template <class T>
class CTouchWnd : public T
{
public:
	CTouchWnd() {}

protected:
	virtual ULONG GetGestureStatus(CPoint ptTouch) override { return 0; }
};

class __CommonExt CResGuard
{
public:
	CResGuard(CResModule& resModule)
	{
		m_hPreInstance = AfxGetResourceHandle();
		resModule.ActivateResource();
	}

	CResGuard(CResModule *pResModule)
	{
		if (NULL != pResModule)
		{
			m_hPreInstance = AfxGetResourceHandle();
			pResModule->ActivateResource();
		}
	}

	~CResGuard()
	{
		if (NULL != m_hPreInstance)
		{
			AfxSetResourceHandle(m_hPreInstance);
		}
	}

private:
	HINSTANCE m_hPreInstance = NULL;
};

class __CommonExt CRedrawLockGuard
{
public:
	CRedrawLockGuard(CWnd& wnd, bool bLockUpdate = false);

	CRedrawLockGuard(CRedrawLockGuard& other);
	
	~CRedrawLockGuard();

public:
	CRedrawLockGuard& operator =(CRedrawLockGuard& other)
	{
		CRedrawLockGuard::CRedrawLockGuard(other);
		return *this;
	}

	CRedrawLockGuard& operator =(CRedrawLockGuard&& other)
	{
		CRedrawLockGuard::CRedrawLockGuard(other);
		return *this;
	}

	void Unlock();

private:
	CWnd& m_wnd;

	bool m_bLockUpdate = false;

	bool m_bLocked = false;
};

class __CommonExt CMenuEx : private CMenu
{
public:
	CMenuEx(UINT uMenuWidth, bool bTopMenu = false) :
		m_uMenuWidth(uMenuWidth)
		, m_bTopMenu(bTopMenu)
	{
	}

	CMenuEx(UINT uMenuWidth, UINT uItemHeight, float fFontSize, bool bTopMenu=false) :
		m_uMenuWidth(uMenuWidth)
		, m_uItemHeight(uItemHeight)
		, m_fFontSize(fFontSize)
		, m_fTopFontSize(fFontSize)
		, m_bTopMenu(bTopMenu)
	{
	}

	CMenuEx(UINT uMenuWidth, UINT uItemHeight, float fFontSize, float fTopFontSize) :
		m_uMenuWidth(uMenuWidth)
		, m_uItemHeight(uItemHeight)
		, m_fFontSize(fFontSize)
		, m_fTopFontSize(fTopFontSize)
		, m_bTopMenu(true)
	{
	}

	CMenuEx(const CMenuEx& other, bool bParent=false) :
		m_uMenuWidth(other.m_uMenuWidth)
		, m_uItemHeight(other.m_uItemHeight)
		, m_fFontSize(other.m_fFontSize)
		, m_fTopFontSize(other.m_fTopFontSize)
		, m_mapMenuItemInfos(other.m_mapMenuItemInfos.begin(), other.m_mapMenuItemInfos.end())
	{
		if (bParent)
		{
			m_bTopMenu = false;

		}
		else
		{
			m_bTopMenu = other.m_bTopMenu;
		}
	}

private:
	UINT m_uMenuWidth = 0;
	UINT m_uItemHeight = 0;

	float m_fFontSize = 0;
	float m_fTopFontSize = 0;

	bool m_bTopMenu = false;

	struct tagMenuItemInfo
	{
		bool bDisable = false;

		bool bChecked = false;

		bool bDelete = false;

		CString strText;
	};
	map<UINT, tagMenuItemInfo> m_mapMenuItemInfos;

	list<CMenuEx> m_lstSubMenu;

private:
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	
protected:
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) override;

	virtual void onDrawItem(CDC& dc, LPDRAWITEMSTRUCT lpDrawItemStruct);

public:
	bool Attach(HMENU hMenu);
	void Detach();

	bool Popup(HMENU hMenu, CWnd *pWnd=NULL);
	bool Popup(HMENU hMenu, UINT uItemHeight, float fFontSize, CWnd *pWnd = NULL);

	bool clonePopup(HMENU hMenu, CWnd *pWnd = NULL);
	bool clonePopup(HMENU hMenu, UINT uItemHeight, float fFontSize, CWnd *pWnd = NULL);

	void EnableItem(UINT uIDItem, bool bEnable);
	void EnableItem(const std::initializer_list<UINT>& ilIDItems, bool bEnable);

	void CheckItem(UINT uIDItem, bool bCheck = true);

	void DeleteItem(UINT uIDItem);
	void DeleteItem(const std::initializer_list<UINT>& ilIDItems);

	void SetItemText(UINT uIDItem, const CString& cstrText);
};

class __CommonExt CMenuGuard
{
public:
	CMenuGuard(CResModule& resModule, UINT uIDMenu, UINT uMenuWidth)
		: m_uIDMenu(uIDMenu)
		, m_resModule(resModule)
		, m_menu(uMenuWidth)
	{
	}

private:
	CResModule& m_resModule;

	UINT m_uIDMenu = 0;
	
	CMenuEx m_menu;
	
public:
	void EnableItem(UINT uIDItem, bool bEnable);
	void EnableItem(const std::initializer_list<UINT>& ilIDItems, bool bEnable);

	void CheckItem(UINT uIDItem, bool bCheck = true);

	void DeleteItem(UINT uIDItem);
	void DeleteItem(const std::initializer_list<UINT>& ilIDItems);

	void SetItemText(UINT uIDItem, const CString& cstrText);
	
	bool Popup(CWnd *pWnd, UINT uItemHeight, float fFontSize = 0);
	bool clonePopup(CWnd *pWnd, UINT uItemHeight, float fFontSize = 0);
};

using CB_CompatableFont = fn_void_t<LOGFONT&>;

class __CommonExt CCompatableFont : public CFont
{
public:
	CCompatableFont() {}
	
private:
	float m_fFontSizeOffset = 0;
	LONG m_lfWeight = -1;
	bool m_bItalic = false;
	bool m_bUnderline = false;

private:
	inline bool _create(CFont& font, const CB_CompatableFont& cb = NULL);
	inline bool _create(CDC& dc, const CB_CompatableFont& cb = NULL);
	inline bool _create(CWnd& wnd, const CB_CompatableFont& cb = NULL);

	bool create(CFont& font, const CB_CompatableFont& cb);
	bool create(CDC& dc, const CB_CompatableFont& cb);
	bool create(CWnd& wnd, const CB_CompatableFont& cb);

public:
	bool create(CFont& font, float fFontSizeOffset = 0, LONG lfWeight = -1, bool bItalic = false, bool bUnderline = false);
	bool create(CDC& dc, float fFontSizeOffset = 0, LONG lfWeight = -1, bool bItalic = false, bool bUnderline = false);
	bool create(CWnd& wnd, float fFontSizeOffset = 0, LONG lfWeight = -1, bool bItalic = false, bool bUnderline = false);

	bool setFont(CWnd& wnd, float fFontSizeOffset = 0, LONG lfWeight = -1, bool bItalic = false, bool bUnderline = false);
};
