
#include "stdafx.h"

#include "PlayerMainWnd.h"

class CMainMenu : public CMenuEx
{
public:
	CMainMenu(__view& view)
		: CMenuEx(225, view.m_globalSize.m_uMenuItemHeight
			, view.m_globalSize.m_fMidFontSize, view.m_globalSize.m_fMidFontSize)
	{
	}

private:
	void onDrawItem(CDC& dc, LPDRAWITEMSTRUCT lpDrawItemStruct) override
	{
		__super::onDrawItem(dc, lpDrawItemStruct);

		auto& rcItem = lpDrawItemStruct->rcItem;
		if (rcItem.left<10)
		{
			rcItem.top += 1;
			dc.DrawText(L"  🎵", &lpDrawItemStruct->rcItem, DT_VCENTER);
		}//💻📂💽🎶
	}
};

BEGIN_MESSAGE_MAP(CPlayerMainWnd, CMainWnd)
	ON_WM_WINDOWPOSCHANGED()

	ON_WM_SYSCOMMAND()
	ON_WM_ENABLE()
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

bool CPlayerMainWnd::Create()
{
	m_view.m_ResModule.ActivateResource();

	tagMainWndInfo MainWndInfo;
	MainWndInfo.hIcon = m_view.m_ResModule.loadIcon(IDI_MAINWND);
	MainWndInfo.strText = L"XMusicHost";
	MainWndInfo.hMenu = m_view.m_ResModule.loadMenu(IDR_MAINDLG);
	
	__EnsureReturn(__super::Create(MainWndInfo), false);

	static CMainMenu s_Menu(m_view);
	s_Menu.Attach(MainWndInfo.hMenu);

	if (m_view.getOption().bHideMenuBar)
	{
		this->SetMenu(NULL);
	}

	__EnsureReturn(m_QuickFindDlg.Create(IDD_DLG_QuickFind, this), false);

	__EnsureReturn(m_wndSysToolBar.Create(*this), false);

	return true;
}

void CPlayerMainWnd::show()
{
	fixWorkArea(m_view.getOption().bFullScreen);

	ShowWindow(SW_SHOW);

	(void)m_wndSysToolBar.ShowWindow(SW_SHOWNOACTIVATE);
}

void CPlayerMainWnd::showMenu(bool bShowMenu)
{
	::SetMenu(m_hWnd, bShowMenu?m_WndInfo.hMenu:NULL);

	m_wndSysToolBar.GetToolBar().move();
}

void CPlayerMainWnd::OnWindowPosChanged(WINDOWPOS *pWndPos)
{
	if (0 == (pWndPos->flags & SWP_NOMOVE) || 0 == (pWndPos->flags & SWP_NOSIZE))
	{
		m_wndSysToolBar.GetToolBar().move();

		cauto rcPos = m_view.m_PlayCtrl.getPlaySpirit().rect();
		m_view.m_PlayCtrl.getPlaySpirit().move(rcPos.left - 1, rcPos.top - 1);
		m_view.m_PlayCtrl.getPlaySpirit().move(rcPos.left, rcPos.top);
	}

	__super::OnWindowPosChanged(pWndPos);
}

void CPlayerMainWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == SC_MOVE)
	{
		return;
	}

	if (SC_CLOSE == nID)
	{
		if (!IsWindowEnabled())
		{
			return;
		}

		m_view.quit();
		return;
	}
	
	__super::OnSysCommand(nID, lParam);
}

void CPlayerMainWnd::OnFocus(BOOL bFocus)
{
	static CWnd* s_pwndPrevFocusPage = NULL;

	if (!bFocus)
	{
		CWnd *pWnd = CWnd::GetFocus();
		__Ensure(pWnd);
		
		CWnd *pParent = pWnd->GetParent();
		__Ensure(pParent && pParent->GetRuntimeClass() == CPage::GetThisClass());
		
		s_pwndPrevFocusPage = pWnd;
	}
	else
	{
		if (s_pwndPrevFocusPage)
		{
			(void)::SetFocus(s_pwndPrevFocusPage->GetSafeHwnd());
		}
	}
}

void CPlayerMainWnd::OnEnable(BOOL bEnable)
{
	__super::OnEnable(bEnable);

	if (bEnable)
	{
		(void)m_wndSysToolBar.ModifyStyleEx(WS_EX_TRANSPARENT | WS_EX_LAYERED, 0);
	}
	else
	{	
		(void)m_wndSysToolBar.ModifyStyleEx(0, WS_EX_TRANSPARENT | WS_EX_LAYERED);
	}
}

void CPlayerMainWnd::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CMainWnd::OnActivate(nState, pWndOther, bMinimized);

	m_bActivate = (WA_INACTIVE != nState);

	OnFocus(m_bActivate);
}

BOOL CPlayerMainWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	switch (message)
	{
	case WM_DESTROY:
		m_wndSysToolBar.Destroy();

		(void)m_QuickFindDlg.DestroyWindow();

		break;
		//case WM_ENTERMENULOOP:
		//	bMenuPopup = TRUE;

		//	break;
		//case WM_EXITMENULOOP:
		//	bMenuPopup = FALSE;

		//	break;
	case WM_NCMOUSEMOVE:
		if (m_bActivate)
		{
			if (!m_bTrackMouseFlag)
			{
				TRACKMOUSEEVENT tme;
				memzero(tme);
				tme.cbSize = sizeof(tme);
				tme.hwndTrack = m_hWnd;
				tme.dwFlags = TME_LEAVE | TME_HOVER | TME_NONCLIENT;
				tme.dwHoverTime = HOVER_DEFAULT;
				m_bTrackMouseFlag = ::TrackMouseEvent(&tme);
			}
		}

		break;
	case WM_NCMOUSELEAVE:
		m_bTrackMouseFlag = false;

		break;
	case WM_NCMOUSEHOVER:
		m_bTrackMouseFlag = false;

		if (m_bActivate)
		{
			CPoint ptPos(lParam);

			CRect rcPos;
			int iCount = ::GetMenuItemCount(m_WndInfo.hMenu);
			for (int iIndex = 0; iIndex < iCount; iIndex++)
			{
				::GetMenuItemRect(m_hWnd, m_WndInfo.hMenu, iIndex, &rcPos);
				if (ptPos.x > rcPos.left && ptPos.x < rcPos.right && ptPos.y > rcPos.top && ptPos.y < rcPos.bottom)
				{
					mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
					mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);

					break;
				}
			}
		}
		
		break;
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CPlayerMainWnd::onViewResize(CDockView& view)
{
	if (E_DockViewType::DVT_DockLeft == view.getViewStyle().eViewType)
	{
		if (m_QuickFindDlg)
		{
			m_QuickFindDlg.SetWindowPos(NULL, 0, 0, view.getViewStyle().uDockSize
				, m_view.m_globalSize.m_uHeadHeight-5, SWP_NOZORDER);
		}
	}
}

#define __XOffset 7

void CPlayerMainWnd::_fixWorkArea(CRect& rcWorkArea, bool bFullScreen)
{
	rcWorkArea.left -= __XOffset;
	rcWorkArea.right += __XOffset - 1;

	if (m_view.getOption().bHideMenuBar)
	{
		rcWorkArea.top -= 4;
	}
	else
	{
		rcWorkArea.top -= 1;
	}

	if (bFullScreen)
	{
		rcWorkArea.bottom += 2;
	}
	else
	{
		rcWorkArea.bottom += 3;
	}

	__super::_fixWorkArea(rcWorkArea, bFullScreen);
}
