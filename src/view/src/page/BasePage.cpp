
#include "stdafx.h"

#define __PlayerClipboardFormat ((UINT)-1)

CBasePage::CBasePage(__view& view, UINT uIDDlgRes, cwstr strBaseTitle, UINT uIDMenu, bool bAutoActive)
	: CPage(view.m_ResModule, uIDDlgRes, strBaseTitle.c_str(), bAutoActive)
	, m_view(view)
	, m_strBaseTitle(strBaseTitle)
	, m_MenuGuard(view.m_ResModule, uIDMenu, __MenuWidth)
{
}

void CBasePage::Active(bool bForceFocus)
{
	m_view.m_MainWnd.ActivePage(*this, bForceFocus);
}

void CBasePage::SetTitle(cwstr strTitle)
{
	int iImage = GetTabImage();
	__super::SetTitle((m_strBaseTitle + strTitle).c_str(), iImage);
}

BOOL CBasePage::RegDragDropCtrl(CWnd& wndCtrl, const CB_GetCtrlDragData& cb)
{
	if (!RegDragableCtrl(wndCtrl, cb))
	{
		return FALSE;
	}

	if (!__super::RegDropableCtrl(wndCtrl))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CBasePage::RegDragableCtrl(CWnd& wndCtrl, const CB_GetCtrlDragData& cb)
{
	if (!__super::RegDragableCtrl(wndCtrl))
	{
		return FALSE;
	}

	m_mapDragableCtrl[&wndCtrl] = cb;

	return TRUE;
}

static tagDragData g_DragData;

BOOL CBasePage::GetCtrlDragData(HWND hwndCtrl, const CPoint& point, LPVOID& pDragData)
{
	CWnd *pwndCtrl = CWnd::FromHandle(hwndCtrl);
	__AssertReturn(pwndCtrl, FALSE);

	auto itr = m_mapDragableCtrl.find(pwndCtrl);
	if (itr == m_mapDragableCtrl.end())
	{
		return FALSE;
	}

	if (NULL != g_DragData.hIcon)
	{
		DestroyIcon(g_DragData.hIcon);
	}
	g_DragData = tagDragData();

	__EnsureReturn(itr->second(g_DragData), FALSE);

	if (g_DragData.iImage >= 0)
	{
		g_DragData.hIcon = m_view.m_ImgMgr.bigImglst().ExtractIcon(g_DragData.iImage);
	}

	pDragData = &g_DragData;

	return TRUE;
}

void CBasePage::handleDragOver(CObjectList& wndList, CDragContext& DragContext
	, const function<bool(int& uDragOverPos)>& cb)
{
	__Ensure(wndList.GetView() == E_ListViewType::LVT_Report);

	if (m_bDragScrolling)
	{
		return;
	}

	int nDragOverPos = 0;
	auto y = DragContext.y;

	UINT uHeaderHeight = wndList.GetHeaderHeight();
	if (0 == wndList.GetItemCount())
	{
		nDragOverPos = 0;
		y = uHeaderHeight;
	}
	else if (0 != uHeaderHeight && y <= (int)uHeaderHeight)
	{
		nDragOverPos = wndList.HitTest(CPoint(5, uHeaderHeight + 5));
		if (nDragOverPos < 0)
		{
			nDragOverPos = 0;
		}

		y = uHeaderHeight;
	}
	else
	{
		nDragOverPos = wndList.HitTest(CPoint(5, y));
		if (nDragOverPos < 0)
		{
			nDragOverPos = wndList.GetItemCount() - 1;
		}

		CRect rcItem;
		(void)wndList.GetItemRect(nDragOverPos, &rcItem, LVIR_BOUNDS);
		if (y < rcItem.CenterPoint().y)
		{
			y = rcItem.top;
		}
		else
		{
			y = rcItem.bottom;

			nDragOverPos++;
		}
	}

	if (cb)
	{
		if (!cb(nDragOverPos))
		{
			return;
		}
	}

	DragContext.uTargetPos = (UINT)nDragOverPos;

	if (DragScroll(wndList, DragContext.x, DragContext.y))
	{
		return;
	}
	
	DragContext.DrawDragOverHLine(y);
}

bool CBasePage::DragScroll(CObjectList& wndList, LONG x, LONG y)
{
	if (m_bDragScrolling)
	{
		return true;
	}

	auto eViewType = wndList.GetView();
	if (E_ListViewType::LVT_List == eViewType)
	{
		if (x < 30)
		{
			if (0 == wndList.GetScrollPos(SB_HORZ))
			{
				return false;
			}

			m_bDragScrolling = true;
			(void)wndList.SendMessage(WM_HSCROLL, MAKELONG(SB_LINELEFT, 0));
			m_bDragScrolling = false;
			mtutil::usleep(50);
			return true;
		}
		else
		{
			CRect rcView;
			wndList.GetClientRect(rcView);
			if (x > rcView.Width() - 30)
			{
				if (wndList.GetScrollLimit(SB_HORZ) == wndList.GetScrollPos(SB_HORZ))
				{
					return false;
				}

				m_bDragScrolling = true;
				(void)wndList.SendMessage(WM_HSCROLL, MAKELONG(SB_LINERIGHT, 0));
				m_bDragScrolling = false;
				mtutil::usleep(50);
				return true;
			}
		}

		return false;
	}

	int nItem = wndList.HitTest(CPoint(5, y));
	if (y < (int)wndList.GetHeaderHeight() + 30)
	{
		if (0 == wndList.GetScrollPos(SB_VERT))
		{
			return false;
		}

		m_bDragScrolling = true;
		(void)wndList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP, 0));
		m_bDragScrolling = false;
		mtutil::usleep(50);
		return true;
	}
	else
	{
		CRect rcView;
		wndList.GetClientRect(rcView);
		if (y > rcView.Height() - 30)
		{
			if (wndList.GetScrollLimit(SB_VERT) == wndList.GetScrollPos(SB_VERT))
			{
				return false;
			}

			m_bDragScrolling = true;
			(void)wndList.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN, 0));
			m_bDragScrolling = false;
			mtutil::usleep(50);
			return true;
		}
	}

	return false;
}
