#pragma once

#include <afxole.h>

interface IDropTargetEx
{
	virtual DROPEFFECT OnDragOver(CWnd *pWnd, LPVOID pDragData, DWORD dwKeyState, CPoint point, BOOL bFirstEnter=FALSE) = 0;

	virtual BOOL OnDrop(CWnd *pWnd, LPVOID pDragData, DROPEFFECT dropEffect, CPoint point) = 0;

	virtual void OnDragLeave(CWnd *pWnd, LPVOID pDragData) = 0;
};

class __CommonExt CDragDropMgr : public COleDropTarget
{
public:
	CDragDropMgr()
	{
	}
	
	CDragDropMgr(const CDragDropMgr &)
	{
		if (0)
		return;
	}
	
public:
	static BOOL DoDrag(LPVOID pDragData=NULL);

	static BOOL RegDropTarget(IDropTargetEx& DropTarget, CWnd& Wnd);

public:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual void OnDragLeave(CWnd* pWnd);
};
