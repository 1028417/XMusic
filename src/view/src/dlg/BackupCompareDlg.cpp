
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
		_fillModifyedMedia();
	}
	else if (m_arrDeletedPlayItem || m_arrDeletedAlbumItem)
	{
		_fillDeletedMedia();
	}
	else if (m_arrMovedMedia)
	{
		_fillMovedMedia();
	}
	else if (m_arrNewPlayItem || m_arrNewAlbumItem)
	{
		_fillNewMedia();
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
		int nItem = m_wndList.GetSelItem();
		if (nItem >= 0)
		{
			cauto strOppPath = m_fnGetPath((UINT)nItem);
			if (!strOppPath.empty())
			{
				cauto strAbsPath = m_view.getMediaLib().toAbsPath(strOppPath);
				if (fsutil::existFile(strAbsPath))
				{
					m_view.m_PlayCtrl.addPlayingItem(strOppPath);
				}
			}
		}
	}
}

void CCompareResultPage::clear()
{
	m_wndList.DeleteAllItems();
}

void CCompareResultPage::setModifyedMedia(const SArray<tagModifyedMedia>& arrModifyedMedia)
{
	m_arrModifyedMedia.assign(arrModifyedMedia);
	_fillModifyedMedia();
}

void CCompareResultPage::_fillModifyedMedia()
{
	_updateTitle(m_arrModifyedMedia.size());

	if (!m_wndList)
	{
		return;
	}

	m_wndList.DeleteAllItems();
	UINT uItem = 0;
	wstring strMediaSetName;
	for (auto& ModifyedMedia : m_arrModifyedMedia)
	{
		strMediaSetName = ModifyedMedia.strMediaSetName;
		if (!ModifyedMedia.strSingerName.empty())
		{
			strMediaSetName = ModifyedMedia.strSingerName + __CNDot + strMediaSetName;
		}

		m_wndList.InsertItemEx(uItem++, { strMediaSetName, __substr(ModifyedMedia.strPath, 1)
			, ModifyedMedia.strModifyedPath }, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		wstring strPath;
		m_arrModifyedMedia.get(uItem, [&](tagModifyedMedia& ModifyedMedia) {
			strPath = ModifyedMedia.strSingerDir +  ModifyedMedia.strNewPath;
		});

		return strPath;
	};
}

void CCompareResultPage::setDeletedMedia(const SArray<tagDiffMedia>& arrDeletedPlayItem, const SArray<tagDiffMedia>& arrDeletedAlbumItem)
{
	m_arrDeletedPlayItem.assign(arrDeletedPlayItem);
	m_arrDeletedAlbumItem.assign(arrDeletedAlbumItem);
	_fillDeletedMedia();
}

void CCompareResultPage::_fillDeletedMedia()
{
	_updateTitle(m_arrDeletedPlayItem.size() + m_arrDeletedAlbumItem.size());

	if (!m_wndList)
	{
		return;
	}

	m_wndList.DeleteAllItems();

	UINT uItem = 0;
	for (auto& DeletedPlayItem : m_arrDeletedPlayItem)
	{
		m_wndList.InsertItemEx(uItem++, { DeletedPlayItem.strMediaSetName
			, __substr(DeletedPlayItem.strPath, 1) }, L" ");
	}

	for (auto& DeletedAlbumItem : m_arrDeletedAlbumItem)
	{
		m_wndList.InsertItemEx(uItem++, {DeletedAlbumItem.strSingerName	+ __CNDot
			+ DeletedAlbumItem.strMediaSetName, __substr(DeletedAlbumItem.strPath, 1) }, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		wstring strPath;
		if (!m_arrDeletedPlayItem.get(uItem, [&](tagDiffMedia& DeletedPlayItem) {
			strPath = DeletedPlayItem.strPath;
		}))
		{
			m_arrDeletedAlbumItem.get(uItem, [&](tagDiffMedia& DeletedAlbumItem) {
				strPath = DeletedAlbumItem.strSingerDir + DeletedAlbumItem.strPath;
			});
		}
		
		return strPath;
	};
}

void CCompareResultPage::setMovedMedia(const SArray<tagMovedMedia>& arrMovedMedia)
{
	m_arrMovedMedia.assign(arrMovedMedia);
	_fillMovedMedia();
}

void CCompareResultPage::_fillMovedMedia()
{
	_updateTitle(m_arrMovedMedia.size());

	if (!m_wndList)
	{
		return;
	}

	m_wndList.DeleteAllItems();
	UINT uItem = 0;
	wstring strOldMediaSetName;
	wstring strNewMediaSetName;
	for (auto& MovedMedia : m_arrMovedMedia)
	{
		if (!MovedMedia.strSingerName.empty())
		{
			strOldMediaSetName = MovedMedia.strSingerName + __CNDot + MovedMedia.strMediaSetName;
			strNewMediaSetName = MovedMedia.strSingerName + __CNDot + MovedMedia.strNewMediaSetName;
		}
		else
		{
			strOldMediaSetName = MovedMedia.strMediaSetName;
			strNewMediaSetName = MovedMedia.strNewMediaSetName;
		}

		m_wndList.InsertItemEx(uItem++, {strOldMediaSetName
			, __substr(MovedMedia.strPath, 1), strNewMediaSetName}, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		wstring strPath;
		m_arrMovedMedia.get(uItem, [&](tagMovedMedia& MovedMedia) {
			strPath = MovedMedia.strSingerDir + MovedMedia.strPath;
		});
		
		return strPath;
	};
}

void CCompareResultPage::setNewMedia(const SArray<tagDiffMedia>& arrNewPlayItem, const SArray<tagDiffMedia>& arrNewAlbumItem)
{
	m_arrNewPlayItem.assign(arrNewPlayItem);
	m_arrNewAlbumItem.assign(arrNewAlbumItem);
	_fillNewMedia();
}

void CCompareResultPage::_fillNewMedia()
{
	_updateTitle(m_arrNewPlayItem.size() + m_arrNewAlbumItem.size());

	if (!m_wndList)
	{
		return;
	}

	m_wndList.DeleteAllItems();
	UINT uItem = 0;
	for (auto& NewPlayItem : m_arrNewPlayItem)
	{
		m_wndList.InsertItemEx(uItem++, { NewPlayItem.strMediaSetName, __substr(NewPlayItem.strPath, 1) }, L" ");
	}

	for (auto& NewAlbumItem : m_arrNewAlbumItem)
	{
		m_wndList.InsertItemEx(uItem++, {NewAlbumItem.strSingerName + __CNDot
			+ NewAlbumItem.strMediaSetName, __substr(NewAlbumItem.strPath, 1) }, L" ");
	}

	m_fnGetPath = [&](UINT uItem) {
		wstring strPath;
		if (!m_arrNewPlayItem.get(uItem, [&](tagDiffMedia& NewPlayItem) {
			strPath = NewPlayItem.strPath;
		}))
		{
			m_arrNewAlbumItem.get(uItem, [&](tagDiffMedia& NewAlbumItem) {
				strPath = NewAlbumItem.strSingerDir + NewAlbumItem.strPath;
			});
		}

		return strPath;
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

	async(50, [&]() {
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
	__waitCursor;

	CString cstrSrcTag;
	int nSrcIdx = m_wndSrcCombo.GetCurSel();
	__Ensure(nSrcIdx >= 0);
	m_wndSrcCombo.GetLBText(nSrcIdx, cstrSrcTag);
	cstrSrcTag.TrimLeft();
	wstring strSrcTag(cstrSrcTag);

	int nDstIdx = m_wndDstCombo.GetCurSel();
	__Ensure(nDstIdx >= 0);
	if (0 == nDstIdx)
	{
		tagCompareBackupResult result;
		__Assert(m_BackupMgr.compareBackup(strSrcTag, result));
		_fillResult(result);
	}
	else
	{
		CString cstrDstTag;
		m_wndDstCombo.GetLBText(nDstIdx, cstrDstTag);
		cstrDstTag.TrimLeft();
		wstring strDstTag(cstrDstTag);

		auto hWnd = m_hWnd;
		m_BackupMgr.compareBackup(strSrcTag, strDstTag, [=](const tagCompareBackupResult& result) {
			if (result.strSrcTag == strSrcTag && result.strDstTag == strDstTag && ::IsWindow(hWnd))
			{
				__appSync([&]() {
					_fillResult(result);
				});
			}
		});
	}
}

void CBackupCompareDlg::_fillResult(const tagCompareBackupResult& result)
{
	CRedrawLock RedrawLock(*this);
	
	bool bAutoActive = true;

	m_pageNew.setNewMedia(result.arrNewPlayItem, result.arrNewAlbumItem);
	if (bAutoActive)
	{
		if (result.arrNewPlayItem || result.arrNewAlbumItem)
		{
			m_dockView.SetActivePage(m_pageNew);
			bAutoActive = false;
		}
	}

	m_pageDeleled.setDeletedMedia(result.arrDeletedPlayItem, result.arrDeletedAlbumItem);
	if (bAutoActive)
	{
		if (result.arrDeletedPlayItem || result.arrDeletedAlbumItem)
		{
			m_dockView.SetActivePage(m_pageDeleled);
			bAutoActive = false;
		}
	}

	m_pageModifyed.setModifyedMedia(result.arrModifyedMedia);
	if (bAutoActive)
	{
		if (result.arrModifyedMedia)
		{
			m_dockView.SetActivePage(m_pageModifyed);
			bAutoActive = false;
		}
	}

	m_pageMoved.setMovedMedia(result.arrMovedMedia);
	if (bAutoActive)
	{
		if (result.arrMovedMedia)
		{
			m_dockView.SetActivePage(m_pageMoved);
			bAutoActive = false;
		}
	}
}
