
#pragma once

#include "FloatPanel.h"

class CSysToolBar : public CToolBar
{
public:
	CSysToolBar(__view& view);

	~CSysToolBar();

private:
	__view& m_view;
	
	HMENU m_hMainMenu = NULL;

	CMenuGuard m_MenuGuard;

	HRGN m_hRgn = NULL;

public:
	bool Create(CWnd& wndParent);
	
	void move();

private:
	BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	void _onNMClick(UINT uID);
};

class CSysToolBarPanel : public CFloatPanel
{
public:
	CSysToolBarPanel(__view& view)
		: m_wndToolBar(view)
	{
	}

private:
	CSysToolBar m_wndToolBar;
	
public:
	CSysToolBar& GetToolBar()
	{
		return m_wndToolBar;
	}

	bool Create(CWnd& wndParent)
	{
		__EnsureReturn(__super::Create(wndParent), false);

		return m_wndToolBar.Create(*this);
	}

	void Destroy()
	{
		(void)m_wndToolBar.DestroyWindow();

		(void)DestroyWindow();
	}
};
