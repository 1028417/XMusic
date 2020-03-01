#pragma once

class CPlayItemPage : public CBasePage
{
	friend class CPlaylistPage;

	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX);

public:
	CPlayItemPage(__view& view);

public:
	CPlaylist *m_pPlaylist = NULL;

private:
	CObjectList m_wndList;
	
public:
	void ShowPlaylist(CPlaylist *pPlaylist, bool bSetActive = true);

	void UpdateRelated(const tagMediaSetChanged& MediaSetChanged);

	void UpdateTitle();

	void OnMenuCommand(UINT uID, UINT uVkKey = 0) override;

	void HittestMedia(CMedia *pMedia);

private:
	int GetTabImage() override
	{
		return (int)E_GlobalImage::GI_Playlist;
	}

	void UpdateHead();

	BOOL OnInitDialog() override;

	afx_msg void OnSize(UINT nType, int cx, int cy);

	DROPEFFECT OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);
	BOOL OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);

	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMSetFocusList(NMHDR *pNMHDR, LRESULT *pResult);
};
