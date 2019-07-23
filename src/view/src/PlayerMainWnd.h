
#pragma once

#include "QuickFindDlg.h"

#include "SysToolBar.h"

//CPlayerMainWnd

class CPlayerMainWnd : public CMainWnd
{
public:
	CPlayerMainWnd(__view& view)
		: m_view(view)
		, m_wndSysToolBar(view)
		, m_QuickFindDlg(view)
	{
	}

	DECLARE_MESSAGE_MAP()

private:
	__view& m_view;

	bool m_bTrackMouseFlag = false;

	CQuickFindDlg m_QuickFindDlg;

public:
	CSysToolBarPanel m_wndSysToolBar;

	bool m_bActivate = false;

public:
	bool Create();

	void show();

	void showMenu(bool bShowMenu);

private:
	afx_msg void OnWindowPosChanged(WINDOWPOS *pWndPos);
	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	void OnEnable(BOOL bEnable);

	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult) override;

	void onViewResize(CDockView& view) override;

	void _fixWorkArea(CRect& rcWorkArea, bool bFullScreen) override;

public:
	void OnFocus(BOOL bFocus);
};
