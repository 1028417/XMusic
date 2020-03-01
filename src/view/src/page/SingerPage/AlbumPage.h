#pragma once

#include "SingerMediaResPanel.h"

class CAlbumPage : public CBasePage
{
	friend class CSingerMediaResPanel;

private:
	enum E_DropPositionFlag
	{
		DPF_UP = 0,
		DPF_DOWN
	};

public:
	CAlbumPage(__view& view);
	
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX);

private:
	CSinger *m_pSinger = NULL;

	CSingerMediaResPanel m_wndMediaResPanel;

	CObjectList m_wndAlbumList;

	CObjectList m_wndAlbumItemList;

	CAlbum *m_pAlbum = NULL;

	CImg m_imgSinger;
	CImg m_imgSingerDefault;

	int m_cx = 0;

	CMenuGuard m_AlbumMenuGuard;

public:
	UINT GetSingerID() const
	{
		if (NULL == m_pSinger)
		{
			return 0;
		}

		return m_pSinger->m_uID;
	}

	void ShowSinger(CSinger *pSinger, CMedia *pAlbumItem=NULL, IMedia *pIMedia=NULL);

	void UpdateSingerName();

	void UpdateSingerImage();

	void UpdateTitle();

	void UpdateRelated(const tagMediaSetChanged& MediaSetChanged);

	void RefreshAlbum();

private:
	int GetTabImage() override;

	void _showAlbum(CAlbum *pAlbum);

	void _asyncTask();

	BOOL _checkMediasDropable(const TD_IMediaList& lstMedias);

	bool _playSingerImage(bool bReset);

private:
	BOOL OnInitDialog() override;

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnPaint();

	//void OnActive(BOOL bActive) override;

	void OnMenuCommand(UINT uID, UINT uVkKey = 0) override;

	void OnMenuCommand_Album(UINT uID);
	void OnMenuCommand_AlbumItem(UINT uID, UINT uVkKey=0);
	
	DROPEFFECT OnMediasDragOverExploreList(const TD_IMediaList& lstMedias, CDragContext& DragContext);
	BOOL OnMediasDropExploreList(const TD_IMediaList& lstMedias, UINT uTargetPos, DROPEFFECT nDropEffect);

	DROPEFFECT OnMediasDragOverBrowseList(const TD_IMediaList& lstMedias, CDragContext& DragContext);	
	BOOL OnMediasDropBrowseList(const TD_IMediaList& lstMedias, CAlbum *pTargetAlbum, DROPEFFECT nDropEffect);
	
	afx_msg void OnLvnItemchangedListBrowse(NMHDR *pNMHDR, LRESULT *pResult);
	void OnLvnBeginlabeleditListBrowse(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListBrowse(NMHDR *pNMHDR, LRESULT *pResult);
		
	DROPEFFECT OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);
	BOOL OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext);

	DROPEFFECT OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);	
	BOOL OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);

	afx_msg void OnNMRclickListBrowse(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListBrowse(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMRclickListExplore(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListExplore(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClickListExplore(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMSetFocusListBrowse(NMHDR *pNMHDR, LRESULT *pResult)
	{
		*pResult = 0;
		_select();
	}

	afx_msg void OnNMSetFocusListExplore(NMHDR *pNMHDR, LRESULT *pResult)
	{
		*pResult = 0;
		_select();
	}

	void _select()
	{
		if (m_pAlbum)
		{
			m_wndAlbumList.SelectObject(m_pAlbum);
		}
		else
		{
			m_wndAlbumList.SelectItem(0);
		}
	}
};
