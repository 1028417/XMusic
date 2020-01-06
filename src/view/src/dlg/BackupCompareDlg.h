#pragma once

class CCompareResultPage : public CPage
{
	DECLARE_MESSAGE_MAP()

public:
	CCompareResultPage(__view& view, CDockView& dockView, const CString& strTitle, const TD_ListColumn& lstColumns)
		: CPage(view.m_ResModule, IDD_PAGE_COMPARE_RESULT, strTitle, true)
		, m_view(view)
		, m_dockView(dockView)
		, m_strTitle(strTitle)
		, m_ListColumnGuard(lstColumns)
	{
	}

private:
	__view& m_view;

	CDockView& m_dockView;

	wstring m_strTitle;

	CListColumnGuard m_ListColumnGuard;

	CObjectList m_wndList;

	function<wstring(UINT uItem)> m_fnGetPath;

	SArray<tagModifyedMedia> m_arrModifyedMedia;
	
	SArray<tagDeletedPlayItem> m_arrDeletedPlayItem;
	SArray<tagDeletedAlbumItem> m_arrDeletedAlbumItem;
	
	SArray<tagMovedMedia> m_arrMovedMedia;
	
	SArray<tagNewPlayItem> m_arrNewPlayItem;
	SArray<tagNewAlbumItem> m_arrNewAlbumItem;

private:
	void DoDataExchange(CDataExchange* pDX);

	BOOL OnInitDialog() override;

	void OnSize(UINT nType, int cx, int cy);

	void _fillModifyedMedia();
	void _fillDeletedMedia();
	void _fillMovedMedia();
	void _fillNewMedia();

	void _updateTitle(size_t uCount);

	void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

public:
	void clear();

	void setModifyedMedia(const SArray<tagModifyedMedia>& arrModifyedMedia);
	void setDeletedMedia(const SArray<tagDeletedPlayItem>& arrDeletedPlayItem, const SArray<tagDeletedAlbumItem>& arrDeletedAlbumItem);
	void setMovedMedia(const SArray<tagMovedMedia>& arrMovedMedia);
	void setNewMedia(const SArray<tagNewPlayItem>& arrNewPlayItem, const SArray<tagNewAlbumItem>& arrNewAlbumItem);
};

class CBackupCompareDlg : public CDialogT<IDD_DLG_BackupCompare>
{
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	CBackupCompareDlg(__view& view, const wstring& strTag, bool bCompareCurrent)
		: m_view(view)
		, m_BackupMgr(view.getBackupMgr())
		, m_strTag(strTag)
		, m_bCompareCurrent(bCompareCurrent)
		, m_dockView(*this)
		, m_pageNew(view, m_dockView, L"新\n增", { { L"歌单/专辑", 300 },{ L"新增曲目", 800 } })
		, m_pageDeleled(view, m_dockView, L"删\n除", { { L"歌单/专辑", 300 },{ L"刪除曲目", 800 } })
		, m_pageModifyed(view, m_dockView, L"修\n改", { { L"歌单/专辑", 250 }, { L"曲目", 450 }, { L"新文件", 400 } })
		, m_pageMoved(view, m_dockView, L"移\n动", { { L"歌单/专辑", 250 },{ L"曲目", 600 },{ L"新位置", 250 } })
	{
	}
	
private:
	__view& m_view;
	CBackupMgr& m_BackupMgr;

	wstring m_strTag;
	bool m_bCompareCurrent = false;

	CDockView m_dockView;

	CCompareResultPage m_pageNew;
	CCompareResultPage m_pageDeleled;
	CCompareResultPage m_pageModifyed;
	CCompareResultPage m_pageMoved;

	CComboBox m_wndDstCombo;
	CComboBox m_wndSrcCombo;

	SArray<wstring> m_arrBackupTag;

private:
	BOOL OnInitDialog() override;

	void OnSelChangedDstCombo();

	void OnSelChangedSrcCombo();

private:
	void _initSrcCombo();

	void _compare();

	void _fillResult(const tagCompareBackupResult& result);
};
