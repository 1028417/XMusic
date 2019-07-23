#pragma once

#include "PlayingList.h"

class CPlayingPage : public CBasePage
{
public:
	CPlayingPage(__view& view);

	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX);

public:
	CPlayingList m_wndList;

public:
	void RefreshList(int nPlayingItem=-1);

private:
	BOOL OnInitDialog();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	void OnMenuCommand(UINT uID, UINT uVkKey = 0);
	
	DROPEFFECT OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);
	BOOL OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);
	
	DROPEFFECT OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);
	BOOL OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);

	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);

	void fixColumnWidth(int width);
};
