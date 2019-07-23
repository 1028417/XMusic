
#include "stdafx.h"
#include "view.h"
#include "FloatPanel.h"

// CFloatPanel

BEGIN_MESSAGE_MAP(CFloatPanel, CWnd)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

bool CFloatPanel::Create(CWnd& wndParent, bool bLayerd, uint8_t uAlpha)
{
	LPCTSTR lpszClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS
		, 0, ::GetSysColorBrush(CTLCOLOR_DLG), NULL);
	__AssertReturn(lpszClassName, false);

	DWORD dwExStyle = WS_EX_TOOLWINDOW;
	if (bLayerd || MAX_ALPHA != uAlpha)
	{
		dwExStyle = WS_EX_TRANSPARENT | WS_EX_LAYERED;
	}

	RECT rcPos{ 0,0,0,0 };
	if (!CreateEx(dwExStyle, lpszClassName, NULL, WS_OVERLAPPED, rcPos, &wndParent, 0))
	{
		return false;
	}
	this->ModifyStyle(WS_OVERLAPPEDWINDOW, 0);

	if (MAX_ALPHA != uAlpha)
	{
		SetLayeredWindowAttributes(RGB(0, 0, 0), uAlpha, LWA_ALPHA);
	}

	return true;
}

void CFloatPanel::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (WA_INACTIVE != nState)
	{
		(void)this->GetTopLevelOwner()->SetForegroundWindow();
	}
}
