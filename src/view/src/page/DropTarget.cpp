
#include "stdafx.h"

static CPen g_pen(PS_SOLID, 1, RGB(170, 170, 255));

bool CDragContext::AttachCtrl(CWnd& wndCtrl, HICON hDragIcon)
{
	pwndCtrl = &wndCtrl;
	wndCtrl.GetClientRect(rcCtrl);

	uTargetPos = 0;
	pTargetObj = NULL;

	(void)m_CompDC.destroy();

	pdcCtrl = wndCtrl.GetDC();
	if (NULL == pdcCtrl)
	{
		return false;
	}

	(void)pdcCtrl->SelectObject(g_pen);
	(void)pdcCtrl->SelectStockObject(NULL_BRUSH);

	if (NULL != hDragIcon)
	{
		if (!m_CompDC.create(hDragIcon))
		{
			return false;
		}
	}

	return true;
}

void CDragContext::DettachCtrl()
{
	if (NULL != pwndCtrl)
	{
		if (NULL != pdcCtrl)
		{
			pwndCtrl->ReleaseDC(pdcCtrl);
			pdcCtrl = NULL;
		}

		pwndCtrl = NULL;
	}

	uTargetPos = 0;
	pTargetObj = NULL;
}

void CDragContext::DrawDragOverIcon(long nXPos, long nYPos)
{
	if (NULL != pdcCtrl)
	{
		CDC& CompDC = m_CompDC.getDC();
		if (CompDC)
		{
			BLENDFUNCTION bf;
			memzero(bf);
			bf.SourceConstantAlpha = 0xff/4;

			rcDragOverIcon.SetRect(nXPos, nYPos, nXPos + m_CompDC.m_cx, nYPos + m_CompDC.m_cy);
			rcDragOverIcon.OffsetRect(10-m_CompDC.m_cx, 10-m_CompDC.m_cy);
			(void)pdcCtrl->AlphaBlend(rcDragOverIcon.left, rcDragOverIcon.top, rcDragOverIcon.Width(), rcDragOverIcon.Height()
				, &CompDC, 0, 0, m_CompDC.m_cx, m_CompDC.m_cy, bf);
		}
	}
}

void CDragContext::DrawDragOverHLine(long nYPos)
{
	if (1 > nYPos)
	{
		nYPos = 1;
	}

	DrawDragOverRect(nYPos - 1, nYPos + 1);
}

void CDragContext::DrawDragOverVLine(long nXPos, long nYPos1, long nYPos2)
{
	CRect rcPos(x-1, nYPos1, x+1, nYPos2);
	DrawDragOverRect(rcPos);
}

void CDragContext::DrawDragOverRect(long nYPos1, long nYPos2)
{
	CRect rcPos(0, nYPos1, rcCtrl.right, nYPos2);
	DrawDragOverRect(rcPos);
}

void CDragContext::DrawDragOverRect(const CRect& rcPos)
{
	if (NULL != pdcCtrl)
	{
		(void)pdcCtrl->Rectangle(&rcPos);
		(void)pdcCtrl->Rectangle(rcPos.left + 1, rcPos.top + 1, rcPos.right - 1, rcPos.bottom - 1);

		rcDragOverFocus = rcPos;
	}
}

void CDragContext::Erase()
{
	if (!rcDragOverIcon.IsRectNull())
	{
		(void)::RedrawWindow(pwndCtrl->GetSafeHwnd()
			, &rcDragOverIcon, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
		
		rcDragOverIcon.SetRect(0, 0, 0, 0);
	}

	if (!rcDragOverFocus.IsRectNull())
	{
		(void)::RedrawWindow(pwndCtrl->GetSafeHwnd()
			, &rcDragOverFocus, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);

		rcDragOverFocus.SetRect(0,0,0,0);
	}
}

BOOL CDropTargetEx::RegDropableCtrl(CWnd& wndCtrl)
{
	__AssertReturn(wndCtrl.GetSafeHwnd(), FALSE);

	return CDragDropMgr::RegDropTarget(*this, wndCtrl);
}

DROPEFFECT CDropTargetEx::OnDragOver(CWnd *pWnd, LPVOID pDragData, DWORD dwKeyState, CPoint point, BOOL bFirstEnter)
{
	__AssertReturn(pWnd, DROPEFFECT_NONE);
	__EnsureReturn(pDragData, DROPEFFECT_NONE);

	tagDragData& DragData = *(tagDragData*)pDragData;

	if (bFirstEnter)
	{
		m_DragContext.AttachCtrl(*pWnd, DragData.hIcon);
	}
	else
	{
		m_DragContext.Erase();
	}

	m_DragContext.bFirstEnter = bFirstEnter;

	m_DragContext.dwKeyState = dwKeyState;
	m_DragContext.x = point.x;
	m_DragContext.y = point.y;

	m_DragContext.nDropEffect = DROPEFFECT_NONE;

	if (DragData.pMediaSet)
	{
		m_DragContext.nDropEffect = OnMediaSetDragOver(pWnd, DragData.pMediaSet, m_DragContext);
	}
	else if (DragData.lstMedias)
	{
		m_DragContext.nDropEffect = OnMediasDragOver(pWnd, DragData.lstMedias, m_DragContext);
	}
	
	m_DragContext.DrawDragOverIcon(point.x, point.y);

	return m_DragContext.nDropEffect;
}

BOOL CDropTargetEx::OnDrop(CWnd *pWnd, LPVOID pDragData, DROPEFFECT dropEffect, CPoint point)
{
	__AssertReturn(pWnd, DROPEFFECT_NONE);
	__EnsureReturn(pDragData, DROPEFFECT_NONE);

	m_DragContext.Erase();

	m_DragContext.x = point.x;
	m_DragContext.y = point.y;

	tagDragData& DragData = *(tagDragData*)pDragData;

	BOOL bResult = FALSE;

	if (DragData.pMediaSet)
	{
		if (m_DragContext.pTargetObj != DragData.pMediaSet)
		{
			bResult = OnMediaSetDrop(pWnd, DragData.pMediaSet, m_DragContext);
		}
	}
	else if (DragData.lstMedias)
	{
		bResult = OnMediasDrop(pWnd, DragData.lstMedias, m_DragContext);
	}

	m_DragContext.DettachCtrl();
	
	return bResult;
}

void CDropTargetEx::OnDragLeave(CWnd *pWnd, LPVOID pDragData)
{
	m_DragContext.Erase();

	m_DragContext.DettachCtrl();
}
