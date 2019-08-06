#pragma once

#include "AlbumPage.h"

class CSingerPage : public CBasePage
{
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX);
	
public:
	CSingerPage(__view& view, CAlbumPage& AlbumPage);

private:
	CAlbumPage& m_AlbumPage;

	CObjectTree m_wndTree;
	
	CMediaSet *m_pDropHilightSingerObject = NULL;

	enum E_DropPositionFlag
	{
		DPF_UP,
		DPF_DOWN
	};
	E_DropPositionFlag m_eDropPositionFlag = DPF_UP;

	set<UINT> m_setExpandedGroupIDs;

	bool m_bRefreshing = false;

	UINT m_uSingerID = 0;

public:
	BOOL RefreshTree(CMediaSet *pSingerObject=NULL);

	void Active(CSinger& Singer);

private:
	void _addSinger(CMediaRes *pSrcPath, CSingerGroup *pGroup);

	CMediaSet* _trySelectObject();

private:
	BOOL OnInitDialog();

	void OnActive(BOOL bActive);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	void OnMenuCommand(UINT uID, UINT uVkKey = 0);

	afx_msg void OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnTvnEndlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult);

	DROPEFFECT OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);
	BOOL OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);

	afx_msg void OnTvnItemexpandedTree(NMHDR *pNMHDR, LRESULT *pResult);

	BOOL PreTranslateMessage(MSG* pMsg) override;
};
