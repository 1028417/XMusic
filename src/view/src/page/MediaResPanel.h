#pragma once

#include "BasePage.h"

class CMediaResList : public CPathList
{
public:
	CMediaResList(bool bShowRelatedSinger)
		: m_bShowRelatedSinger(bShowRelatedSinger)
	{
	}

private:
	bool m_bShowRelatedSinger = true;

private:
	void GenListItem(CListObject& Object, E_ListViewType eViewType, vector<wstring>& vecText, TD_ListImgIdx& iImage) override
	{
		((CMediaRes&)Object).genMediaResListItem(eViewType, vecText, iImage, m_bShowRelatedSinger);
	}
};

class CMediaResPanel : public CBasePage
{
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX);

public:
	static void RefreshMediaResPanel();
	
public:
	CMediaResPanel(__view& view, bool bShowRelatedSinger=true);

	virtual ~CMediaResPanel() = default;

private:
	CMenuGuard m_FileMenuGuard;

	static list<CMediaResPanel*> g_lstMediaResPanels;

	bool m_bShowRelatedSinger = true;

	CWnd m_wndStatic;

	CMediaDir *m_pRootDir = NULL;
	wstring m_strRootDir;

	CMediaDir *m_pCurrDir = NULL;
	wstring m_strCurrDir;
	
	UINT m_Column_ID3 = 0;
	
	int m_cx = 0;
	int m_cy = 0;

	int m_nXOffset = 0;
	int m_nYOffset = 0;

	UINT m_uTileWidth = 0;
	UINT m_uIconWidth = 0;

protected:
	CMediaResList m_wndList;

protected:
	BOOL OnInitDialog() override;

	virtual void UpdateTitle(const wstring& strTitle)
	{
		__super::SetTitle(strTitle);
	}

public:
	BOOL Create(CBasePage& wndPage);

	void ShowDir(const wstring& strPath=L"");

	void Refresh();

	BOOL HittestMedia(IMedia& media, CWnd& wnd);
	
	virtual void HittestMediaRes(CMediaRes& MediaRes);

	void UpdateRelated(const tagMediaSetChanged& MediaSetChanged);

	void attachDir();

	//void OnActive(BOOL bActive) override;

private:
	virtual int GetTabImage() override;

	void _showDirMenu(CMediaDir *pSubDir);
	void _showFileMenu(TD_MediaResList& lstMediaRes);

	DROPEFFECT OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);
	BOOL OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);

	//void OnDeleteDir(CMediaDir& dir);

	void _showDir(CMediaDir *pRootDir, CMediaDir *pCurrDir = NULL, CMediaRes *pHitestItem = NULL);
	void _showDir();

	void _asyncTask();

private:
	virtual void _OnInitDialog() {}

	afx_msg void OnSize(UINT nType, int cx, int cy);
	
	void OnMenuCommand(UINT uID, UINT uVkKey = 0) override;

	afx_msg void OnNMDBblClkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
};
