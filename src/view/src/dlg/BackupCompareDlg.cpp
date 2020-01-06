
#include "stdafx.h"

#include "view.h"

#include "BackupCompareDlg.h"

BEGIN_MESSAGE_MAP(CCompareResultPage, CPage)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CCompareResultPage::OnNMDblclkList1)
END_MESSAGE_MAP()

void CCompareResultPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);
	
	__super::DoDataExchange(pDX);
}

BOOL CCompareResultPage::OnInitDialog()
{
	__super::OnInitDialog();

	CRect rcClient;
	this->m_dockView.GetParent()->GetClientRect(rcClient);
	
	CObjectList::tagListPara ListPara(m_ListColumnGuard.getListColumn(UINT(rcClient.Width() - 120)));
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	m_wndList.ModifyStyle(0, LVS_SINGLESEL);

	if (m_arrModifyedMedia)
	{
		fillModifyedMedia(m_arrModifyedMedia);
		m_arrModifyedMedia.clear();
	}
	else if (m_arrDeletedPlayItem || m_arrDeletedAlbumItem)
	{
		fillDeletedMedia(m_arrDeletedPlayItem, m_arrDeletedAlbumItem);
		m_arrDeletedPlayItem.clear();
		m_arrDeletedAlbumItem.clear();
	}
	else if (m_arrMovedMedia)
	{
		fillMovedMedia(m_arrMovedMedia);
		m_arrMovedMedia.clear();
	}
	else if (m_arrNewPlayItem || m_arrNewAlbumItem)
	{
		fillNewMedia(m_arrNewPlayItem, m_arrNewAlbumItem);
		m_arrNewPlayItem.clear();
		m_arrNewAlbumItem.clear();
	}

	return TRUE;
}

void CCompareResultPage::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndList)
	{
		m_wndList.MoveWindow(0, 0, cx, cy);
	}
}

void CBackupCompareDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_COMBO_DST, m_wndDstCombo);
	DDX_Control(pDX, IDC_COMBO_SRC, m_wndSrcCombo);

	CDialog::DoDataExchange(pDX);
}

void CCompareResultPage::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (m_fnGetPath)
	{
		int iItem = m_wndList.GetSelItem();
		if (iItem >= 0)
		{
			wstring strMediaPath = m_fnGetPath((UINT)iItem);
			if (!strMediaPath.empty())
			{
				m_view.m_PlayCtrl.addPlayingItem(strMediaPath);
			}
		}
	}
}

void CCompareResultPage::clear()
{
	m_wndList.DeleteAllItems();
}

void CCompareResultPage::fillModifyedMedia(const SArray<tagModifyedMedia>& arrModifyedMedia)
{
	_updateTitle(arrModifyedMedia.size());

	if (!m_wndList)
	{
		m_arrModifyedMedia.assign(arrModifyedMedia);
		return;
	}

	m_wndList.DeleteAllItems();
	UINT uItem = 0;
	wstring strMediaSetName;
	for (auto& ModifyedMedia : arrModifyedMedia)
	{
		strMediaSetName = ModifyedMedia.strMediaSetName;
		if (!ModifyedMedia.strSingerName.empty())
		{
			strMediaSetName = ModifyedMedia.strSingerName + __CNDot + strMediaSetName;
		}

		m_wndList.InsertItemEx(uItem++, { strMediaSetName, __substr(ModifyedMedia.strOldPath, 1)
			, __substr(ModifyedMedia.strNewPath, 1) }, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		wstring strOldPath = m_wndList.GetItemText(uItem, 1).TrimLeft();
		wstring strNewFile = m_wndList.GetItemText(uItem, 2).TrimLeft();

		return fsutil::GetParentDir(strOldPath) + __wcDirSeparator + strNewFile;
	};
}

void CCompareResultPage::fillDeletedMedia(const SArray<tagDeletedPlayItem>& arrDeletedPlayItem, const SArray<tagDeletedAlbumItem>& arrDeletedAlbumItem)
{
	_updateTitle(arrDeletedPlayItem.size() + arrDeletedAlbumItem.size());

	if (!m_wndList)
	{
		m_arrDeletedPlayItem.assign(arrDeletedPlayItem);
		m_arrDeletedAlbumItem.assign(arrDeletedAlbumItem);
		return;
	}

	m_wndList.DeleteAllItems();

	UINT uItem = 0;
	for (auto& DeletedPlayItem : arrDeletedPlayItem)
	{
		m_wndList.InsertItemEx(uItem++, { DeletedPlayItem.strPlaylistName
			, __substr(DeletedPlayItem.strPath, 1) }, L" ");
	}

	for (auto& DeletedAlbumItem : arrDeletedAlbumItem)
	{
		m_wndList.InsertItemEx(uItem++, {DeletedAlbumItem.strSingerName	+ __CNDot
			+ DeletedAlbumItem.strAlbumName, __substr(DeletedAlbumItem.strPath, 1) }, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		return (wstring)m_wndList.GetItemText(uItem, 1).TrimLeft();
	};
}

void CCompareResultPage::fillMovedMedia(const SArray<tagMovedMedia>& arrMovedMedia)
{
	_updateTitle(arrMovedMedia.size());

	if (!m_wndList)
	{
		m_arrMovedMedia.assign(arrMovedMedia);
		return;
	}

	m_wndList.DeleteAllItems();
	UINT uItem = 0;
	wstring strOldMediaSetName;
	wstring strNewMediaSetName;
	for (auto& MovedMedia : arrMovedMedia)
	{
		if (!MovedMedia.strSingerName.empty())
		{
			strOldMediaSetName = MovedMedia.strSingerName + __CNDot + MovedMedia.strOldMediaSetName;
			strNewMediaSetName = MovedMedia.strSingerName + __CNDot + MovedMedia.strNewMediaSetName;
		}
		else
		{
			strOldMediaSetName = MovedMedia.strOldMediaSetName;
			strNewMediaSetName = MovedMedia.strNewMediaSetName;
		}

		m_wndList.InsertItemEx(uItem++, {strOldMediaSetName
			, __substr(MovedMedia.strPath, 1), strNewMediaSetName}, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		return (wstring)m_wndList.GetItemText(uItem, 1).TrimLeft();
	};
}

void CCompareResultPage::fillNewMedia(const SArray<tagNewPlayItem>& arrNewPlayItem, const SArray<tagNewAlbumItem>& arrNewAlbumItem)
{
	_updateTitle(arrNewPlayItem.size() + arrNewAlbumItem.size());

	if (!m_wndList)
	{
		m_arrNewPlayItem.assign(arrNewPlayItem);
		m_arrNewAlbumItem.assign(arrNewAlbumItem);
		return;
	}

	m_wndList.DeleteAllItems();
	UINT uItem = 0;
	for (auto& NewPlayItem : arrNewPlayItem)
	{
		m_wndList.InsertItemEx(uItem++, { NewPlayItem.strPlaylistName, __substr(NewPlayItem.strPath, 1) }, L" ");
	}

	for (auto& NewAlbumItem : arrNewAlbumItem)
	{
		m_wndList.InsertItemEx(uItem++, {NewAlbumItem.strSingerName + __CNDot
			+ NewAlbumItem.strAlbumName, __substr(NewAlbumItem.strPath, 1) }, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		return (wstring)m_wndList.GetItemText(uItem, 1).TrimLeft();
	};
}

void CCompareResultPage::_updateTitle(size_t uCount)
{
	wstring strTitle = m_strTitle;
	if (0 != uCount)
	{
		strTitle.append(L"\n");

		if (uCount >= 10000)
		{
			strTitle.append(to_wstring(uCount / 10000) + L"w+");
		}
		else if (uCount >= 1000)
		{
			strTitle.append(to_wstring(uCount / 1000) + L"k+");
		}
		else if (uCount < 10)
		{
			strTitle.append(L" " + to_wstring(uCount));
		}
		else
		{
			strTitle.append(to_wstring(uCount));
		}
	}

	m_dockView.SetPageTitle(*this, strTitle.c_str());
}


BEGIN_MESSAGE_MAP(CBackupCompareDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_DST, OnSelChangedDstCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_SRC, OnSelChangedSrcCombo)
END_MESSAGE_MAP()

// CBackupCompareDlg 消息处理程序
BOOL CBackupCompareDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CRect rcCombo;
	m_wndDstCombo.GetWindowRect(rcCombo);
	this->ScreenToClient(rcCombo);

	tagViewStyle ViewStyle;
	ViewStyle.TabStyle.eTabStyle = E_TabStyle::TS_Left;
	ViewStyle.TabStyle.uTabHeight = rcCombo.left - 3;
	ViewStyle.TabStyle.uTabWidth = UINT(rcCombo.left * 1.9);
	m_dockView.setViewStyle(ViewStyle);

	m_dockView.AddPage(m_pageNew);
	m_dockView.AddPage(m_pageDeleled);
	m_dockView.AddPage(m_pageModifyed); 
	m_dockView.AddPage(m_pageMoved);

	CRect rcCancel;
	this->GetDlgItem(IDCANCEL)->GetWindowRect(rcCancel);
	this->ScreenToClient(rcCancel);

	CRect rcClient;
	this->GetClientRect(rcClient);
	rcClient.top = 2;
	rcClient.right -= 4;
	rcClient.bottom = rcCancel.top - (rcClient.bottom - rcCancel.bottom);

	m_dockView.MoveWindow(rcClient);

	m_arrBackupTag = m_BackupMgr.getBackupTags();

	m_arrBackupTag(1, [&](wstring& strBackupTag, size_t) {
		m_wndDstCombo.InsertString(0, (L" " + strBackupTag).c_str());
	});

	m_wndDstCombo.InsertString(0, L" 当前");
	
	size_t uIdxSrc = 0;
	size_t uIdxDst = 0;
	
	if (m_bCompareCurrent)
	{
		uIdxSrc = m_arrBackupTag.size() - m_arrBackupTag.indexOf(m_strTag) - 1;
	}
	else
	{
		uIdxDst = m_arrBackupTag.size() - MAX(1, m_arrBackupTag.indexOf(m_strTag));
	}

	m_wndDstCombo.SetCurSel(uIdxDst);
	
	_initSrcCombo();

	m_wndSrcCombo.SetCurSel(uIdxSrc);

	__async(50, [&]() {
		_compare();
	});
	return TRUE;
}

void CBackupCompareDlg::OnSelChangedDstCombo()
{
	_initSrcCombo();

	_compare();
}

void CBackupCompareDlg::_initSrcCombo()
{
	m_wndSrcCombo.ResetContent();

	int iIndex = m_wndDstCombo.GetCurSel();
	__Ensure(iIndex >= 0);

	m_arrBackupTag(0, -1-iIndex, [&](wstring& strBackupTag, size_t) {
		m_wndSrcCombo.InsertString(0, (L" " + strBackupTag).c_str());
	});

	m_wndSrcCombo.SetCurSel(0);
}

void CBackupCompareDlg::OnSelChangedSrcCombo()
{
	_compare();
}

void CBackupCompareDlg::_compare()
{
	CString strSrcTag;
	int iSelIndex = m_wndSrcCombo.GetCurSel();
	__Ensure(iSelIndex >= 0);
	m_wndSrcCombo.GetLBText(iSelIndex, strSrcTag);
	strSrcTag.TrimLeft();

	CWaitCursor WaitCursor;

	iSelIndex = m_wndDstCombo.GetCurSel();
	__Ensure(iSelIndex >= 0);
	if (iSelIndex > 0)
	{
		CString strDstTag;
		m_wndDstCombo.GetLBText(iSelIndex, strDstTag);
		strDstTag.TrimLeft();

		m_BackupMgr.compareBackup((wstring)strSrcTag, (wstring)strDstTag, [&](auto& result) {
			CMainApp::sync([&]() {
				_fillResult(result);
			});
		});
	}
	else
	{
		tagCompareBackupResult result;
		__Assert(m_BackupMgr.compareBackup((wstring)strSrcTag, result));
		_fillResult(result);
	}
}

void CBackupCompareDlg::_fillResult(const tagCompareBackupResult& result)
{
	CRedrawLockGuard RedrawLockGuard(*this);
	
	bool bAutoActive = true;

	m_pageNew.fillNewMedia(result.arrNewPlayItem, result.arrNewAlbumItem);
	if (bAutoActive)
	{
		if (result.arrNewPlayItem || result.arrNewAlbumItem)
		{
			m_dockView.SetActivePage(m_pageNew);
			bAutoActive = false;
		}
	}

	m_pageDeleled.fillDeletedMedia(result.arrDeletedPlayItem, result.arrDeletedAlbumItem);
	if (bAutoActive)
	{
		if (result.arrDeletedPlayItem || result.arrDeletedAlbumItem)
		{
			m_dockView.SetActivePage(m_pageDeleled);
			bAutoActive = false;
		}
	}

	m_pageModifyed.fillModifyedMedia(result.arrModifyedMedia);
	if (bAutoActive)
	{
		if (result.arrModifyedMedia)
		{
			m_dockView.SetActivePage(m_pageModifyed);
			bAutoActive = false;
		}
	}

	m_pageMoved.fillMovedMedia(result.arrMovedMedia);
	if (bAutoActive)
	{
		if (result.arrMovedMedia)
		{
			m_dockView.SetActivePage(m_pageMoved);
			bAutoActive = false;
		}
	}
}
