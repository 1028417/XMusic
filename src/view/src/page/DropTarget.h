#pragma once

struct tagDragData
{
	CMediaSet *pMediaSet = NULL;

	TD_IMediaList lstMedias;

	int iImage = -1;
	HICON hIcon = NULL;
};

class CDragContext
{
public:
	CDragContext()
	{
	}

	~CDragContext()
	{
		DettachCtrl();
	}

	bool AttachCtrl(CWnd& wndCtrl, HICON hDragIcon);

	void DettachCtrl();

private:
	CWnd *pwndCtrl = NULL;
	CRect rcCtrl;

	CDC *pdcCtrl = NULL;
	CCompDC m_CompDC;

	CRect rcDragOverIcon;

	CRect rcDragOverFocus;

public:
	LONG x = 0;
	LONG y = 0;

	DWORD dwKeyState = 0;

	BOOL bFirstEnter = FALSE;

	DROPEFFECT nDropEffect = DROPEFFECT_NONE;
	UINT uTargetPos = 0;
	LPVOID pTargetObj = NULL;

public:
	void DrawDragOverIcon(long nXPos, long nYPos);

	void DrawDragOverHLine(long nYPos);

	void DrawDragOverVLine(long nXPos, long nYPos1, long nYPos2);

	void DrawDragOverRect(long nYPos1, long nYPos2);

	void DrawDragOverRect(const CRect& rcPos);

	void Erase();
};

class CDropTargetEx : protected IDropTargetEx
{
public:
	CDropTargetEx()
	{
	}

protected:
	BOOL RegDropableCtrl(CWnd& wndCtrl);
	
private:
	CDragContext m_DragContext;

private:
	DROPEFFECT OnDragOver(CWnd *pWnd, LPVOID pDragData, DWORD dwKeyState, CPoint point, BOOL bFirstEnter) override;

	BOOL OnDrop(CWnd *pWnd, LPVOID pDragData, DROPEFFECT dropEffect, CPoint point) override;

	void OnDragLeave(CWnd *pWnd, LPVOID pDragData) override;

protected:
	virtual DROPEFFECT OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
	{
		return DROPEFFECT_NONE;
	}

	virtual DROPEFFECT OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
	{
		return DROPEFFECT_NONE;
	}

	virtual BOOL OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
	{
		return FALSE;
	}

	virtual BOOL OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
	{
		return FALSE;
	}
};
