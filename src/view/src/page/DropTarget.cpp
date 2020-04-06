
#include "stdafx.h"

static const CPen g_pen(PS_DOT, 1, RGB(128, 192, 255));

bool CDragContext::AttachCtrl(CWnd& wndCtrl, const tagDragData& DragData)
{
	pwndCtrl = &wndCtrl;
	wndCtrl.GetClientRect(rcCtrl);

	uTargetPos = 0;
	pTargetObj = NULL;

	(void)m_dcDragIcon.destroy();

	pdcCtrl = wndCtrl.GetDC();
	if (NULL == pdcCtrl)
	{
		return false;
	}

	(void)pdcCtrl->SelectObject(g_pen);
	(void)pdcCtrl->SelectStockObject(NULL_BRUSH);

	if (DragData.hIcon)
	{
		if (!m_dcDragIcon.create(DragData.hIcon))
		{
			return false;
		}
	}

	return true;
}

void CDragContext::DettachCtrl()
{
	if (pwndCtrl)
	{
		if (pdcCtrl)
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
	if (pdcCtrl)
	{
		CDC& dc = m_dcDragIcon.getDC();
		if (dc)
		{
			BLENDFUNCTION bf;
			memzero(bf);
			bf.SourceConstantAlpha = 0xff/10;

			rcDragOverIcon.SetRect(nXPos, nYPos, nXPos + m_dcDragIcon.m_cx, nYPos + m_dcDragIcon.m_cy);
			rcDragOverIcon.OffsetRect(10-m_dcDragIcon.m_cx, 10-m_dcDragIcon.m_cy);
			(void)pdcCtrl->AlphaBlend(rcDragOverIcon.left, rcDragOverIcon.top, rcDragOverIcon.Width(), rcDragOverIcon.Height()
				, &dc, 0, 0, m_dcDragIcon.m_cx, m_dcDragIcon.m_cy, bf);
			// TODO CImageList::BeginDrag
		}
	}
}

void CDragContext::DrawDragOverHLine(long nYPos)
{
	if (1 > nYPos)
	{
		nYPos = 1;
	}

	DrawDragOverRect(nYPos, nYPos);
}

void CDragContext::DrawDragOverVLine(long nXPos, long nYPos1, long nYPos2)
{
	CRect rcPos(x, nYPos1, x, nYPos2);
	DrawDragOverRect(rcPos);
}

void CDragContext::DrawDragOverRect(long nYPos1, long nYPos2)
{
	CRect rcPos(0, nYPos1, rcCtrl.right, nYPos2);
	DrawDragOverRect(rcPos);
}

void CDragContext::DrawDragOverRect(const CRect& rc)
{
	if (pdcCtrl)
	{
		if (rc.left == rc.right || rc.top == rc.bottom)
		{
			pdcCtrl->MoveTo(rc.left, rc.top);
			pdcCtrl->LineTo(rc.right, rc.bottom);
		}
		else
		{
			(void)pdcCtrl->Rectangle(&rc);
		}

		rcDragOverFocus.SetRect(rc.left-1, rc.top-1, rc.right+1, rc.bottom+1);
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
		m_DragContext.AttachCtrl(*pWnd, DragData);
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
