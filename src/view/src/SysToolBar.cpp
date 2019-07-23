
#include "stdafx.h"

#include "SysToolBar.h"

#define CX_TopToolBar 384
#define X_TopToolBar -55

#define __BottomOffset 30

CSysToolBar::CSysToolBar(__view& view)
	: m_view(view)
	, m_MenuGuard(view.m_ResModule, IDR_MAINDLG, __MenuWidth + 50)
{
}

CSysToolBar::~CSysToolBar()
{
	if (NULL != m_hRgn)
	{
		(void)::DeleteObject(m_hRgn);
	}
}

bool CSysToolBar::Create(CWnd& wndParent)
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC;
	if (!CreateEx(&wndParent, TBSTYLE_FLAT, dwStyle) || !LoadToolBar(IDR_TOOLBAR1))
	{
		return false;
	}
	
	POINT lpPoint[] = { {16,0 },{ 1000,0 },{ 1000, 100 },{ 0, 100 } };
	m_hRgn = ::CreatePolygonRgn(lpPoint, 4, ALTERNATE);
	wndParent.SetWindowRgn(m_hRgn, FALSE);

	move();

	if (m_view.getDataMgr().getOption().bRandomPlay)
	{
		GetToolBarCtrl().SetState(ID_RandomPlay, TBSTATE_PRESSED);
	}

	if (m_view.getDataMgr().getOption().bForce48000)
	{
		GetToolBarCtrl().SetState(ID_Force48000, TBSTATE_PRESSED);
	}

	return true;
}

void CSysToolBar::move()
{
	CRect rcMainWnd;
	m_view.m_MainWnd.GetWindowRect(rcMainWnd);
	rcMainWnd.right -= 2;

	if (m_view.getDataMgr().getOption().bHideMenuBar)
	{
		this->MoveWindow(X_TopToolBar + __BottomOffset, -1, 1000, 1000, FALSE);

		int nHeight = (int)m_view.m_globalSize.m_uHeadHeight + 10;
		SetHeight(nHeight);

		GetParent()->SetWindowPos(NULL, rcMainWnd.right - (CX_TopToolBar + __BottomOffset)
			, rcMainWnd.bottom - nHeight, CX_TopToolBar + __BottomOffset, nHeight
			, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
	}
	else
	{
		int nHeight = (int)m_view.m_globalSize.m_uMenuBarHeight+2;
		SetHeight(nHeight);
		this->MoveWindow(X_TopToolBar, -1, 1000, 1000, FALSE);
		
		CRect rcButton;
		this->GetItemRect(0, &rcButton);
		int cyButton = rcButton.Height();

		int y = (nHeight - cyButton) / 2;
		int cy = nHeight - y;
		if (y > 0)
		{
			cy -= y;
		}
		else
		{
			y--;
		}

		GetParent()->SetWindowPos(NULL, rcMainWnd.right - CX_TopToolBar, rcMainWnd.top + y + 2, CX_TopToolBar, cy
			, SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
	}
}

void CSysToolBar::_onNMClick(UINT uID)
{
	switch (uID)
	{
	case ID_Menu:
		if (m_view.getDataMgr().getOption().bHideMenuBar)
		{
			m_MenuGuard.clonePopup(&m_view.m_MainWnd, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMenuFontSize);
		}

		break;
	case ID_RandomPlay:
	{
		auto& bRandomPlay = m_view.getDataMgr().getOption().bRandomPlay;
		bRandomPlay = !bRandomPlay;

		GetToolBarCtrl().SetState(ID_RandomPlay, bRandomPlay ? TBSTATE_PRESSED : TBSTATE_ENABLED);
	}

	break;
	case ID_Force48000:
	{
		auto& bForce48000 = m_view.getDataMgr().getOption().bForce48000;
		bForce48000 = !bForce48000;

		GetToolBarCtrl().SetState(ID_Force48000, bForce48000 ? TBSTATE_PRESSED : TBSTATE_ENABLED);
	}
	
	break;
	case ID_FullScreen:
	{
		auto& bFullScreen = m_view.getDataMgr().getOption().bFullScreen;
		bFullScreen = !bFullScreen;
		m_view.m_MainWnd.fixWorkArea(bFullScreen);
	}

	break;
	case ID_MIN:
		m_view.m_MainWnd.SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);

		break;
	case ID_CLOSE:
		m_view.m_MainWnd.SendMessage(WM_SYSCOMMAND, SC_CLOSE);

		break;
	}
}

BOOL CSysToolBar::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (WM_NOTIFY == message)
	{
		NMHDR *pNMHDR = (NMHDR*)lParam;
		if (NULL != pNMHDR)
		{
			if (NM_CLICK == pNMHDR->code)
			{
				NMTOOLBAR *pNMHDR = (NMTOOLBAR*)lParam;
				_onNMClick(pNMHDR->iItem);
			}
		}
	}

	return __super::OnChildNotify(message, wParam, lParam, pResult);
}
