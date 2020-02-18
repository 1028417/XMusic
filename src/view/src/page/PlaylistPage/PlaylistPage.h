#pragma once

#include "PlayItemPage.h"

class CPlaylistPage : public CBasePage
{
	DECLARE_MESSAGE_MAP()

public:
	CPlaylistPage(__view& view, CPlayItemPage& PlayItemPage);

private:
	CPlayItemPage& m_PlayItemPage;

	CCompatableFont m_fontBig;
	CCompatableFont m_fontSmall;

	int m_width = 0;

public:
	CObjectList m_wndList;

private:
	void DoDataExchange(CDataExchange* pDX);

	void fixColumnWidth(int width);

public:
	void RefreshList();

private:
	BOOL OnInitDialog() override;

	afx_msg void OnSize(UINT nType, int cx, int cy);

	void OnActive(BOOL bActive) override;

	void OnMenuCommand(UINT uID, UINT uVkKey = 0) override;
		
	DROPEFFECT OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);
	BOOL OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);

	DROPEFFECT OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);
	BOOL OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);

	afx_msg void OnLvnEndlabeleditList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDclickList(NMHDR *pNMHDR, LRESULT *pResult);
};
