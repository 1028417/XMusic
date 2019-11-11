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
	void GenListItem(CListObject& Object, bool bReportView, vector<wstring>& vecText, int& iImage) override
	{
		((CMediaRes&)Object).genMediaResListItem(bReportView, vecText, iImage, m_bShowRelatedSinger);
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

	virtual ~CMediaResPanel(){}

private:
	CMenuGuard m_FileMenuGuard;

	static list<CMediaResPanel*> g_lstMediaResPanels;

	bool m_bShowRelatedSinger = true;

	CWnd m_wndStatic;

	CMediaDir *m_pRootPath = NULL;
	wstring m_strRootPath;

	CMediaDir *m_pCurrPath = NULL;
	wstring m_strCurrPath;
	
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
	BOOL OnInitDialog();

	virtual void UpdateTitle(const wstring& strTitle)
	{
		__super::SetTitle(strTitle);
	}

public:
	BOOL Create(CBasePage& wndPage);

	void ShowPath(const wstring& strPath=L"");

	void Refresh();

	BOOL HittestMediaRes(IMedia& media, CWnd& wnd);
	void HittestMediaRes(CMediaRes& MediaRes);

	void UpdateRelated(const tagMediaSetChanged& MediaSetChanged);

	void attachDir();

private:
	virtual void _OnInitDialog() {}

	virtual int GetTabImage() override;

	void _showDirMenu(CMediaDir *pSubDir);
	void _showFileMenu(TD_MediaResList& lstMediaRes);

	DROPEFFECT OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);
	BOOL OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext);

	//void OnDeleteDir(CMediaDir& dir);

	void _showPath(CMediaDir *pRootPath, CMediaDir *pCurrPath = NULL, CMediaRes *pHitestItem = NULL);
	void _showPath();

	void _asyncTask();

private:
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnNMDBblClkList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnMenuCommand(UINT uID, UINT uVkKey = 0);
};
