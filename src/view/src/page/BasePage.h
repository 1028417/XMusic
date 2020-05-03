#pragma once

#include "DropTarget.h"

#include "globalSize.h"

#define __MenuWidth 200

class CBasePage : public CPage, protected CDropTargetEx
{
public:
	CBasePage(class __view& view, UINT uIDDlgRes, cwstr strBaseTitle, UINT uIDMenu = 0, bool bAutoActive=false);
	
	virtual ~CBasePage() = default;

protected:
	class __view& m_view;

	wstring m_strBaseTitle;

	CMenuGuard m_MenuGuard;

	using CB_GetCtrlDragData = fn_bool_t<tagDragData&>;

private:
	bool m_bDragScrolling = false;

	map<CWnd*, CB_GetCtrlDragData> m_mapDragableCtrl;

private:
	BOOL GetCtrlDragData(HWND hwndCtrl, const CPoint& point, LPVOID& pDragData) override;

protected:
	BOOL RegDragDropCtrl(CWnd& wndCtrl, const CB_GetCtrlDragData& cb);

	BOOL RegDragableCtrl(CWnd& wndCtrl, const CB_GetCtrlDragData& cb);

	virtual int GetTabImage() { return -1; }

	void handleDragOver(CObjectList& wndList, CDragContext& DragContext
		, const function<bool(int& uDragOverPos)>& cb=NULL);

	bool DragScroll(CObjectList& wndList, LONG x, LONG y);

public:
	void Active(bool bForceFocus = true);
	
	void SetTitle(cwstr strTitle);
};
