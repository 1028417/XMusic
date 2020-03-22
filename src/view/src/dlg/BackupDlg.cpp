
#include "stdafx.h"

#include "view.h"

#include "BackupDlg.h"

#include "BackupCompareDlg.h"

BEGIN_MESSAGE_MAP(CBackupDlg, CDialog)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CBackupDlg::OnLvnItemchangedList1)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CBackupDlg::OnNMDblclkList1)

	ON_BN_CLICKED(IDC_BTN_BACKUP, &CBackupDlg::OnBnClickedBackup)

	ON_BN_CLICKED(IDC_BTN_COMPARE, &CBackupDlg::OnBnClickedCompare)

	ON_BN_CLICKED(IDC_BTN_REMOVE, &CBackupDlg::OnBnClickedDel)

	ON_BN_CLICKED(IDC_BTN_RESTORE, &CBackupDlg::OnBnClickedRestore)

	ON_BN_CLICKED(IDC_BTN_ClearAllMedia, &CBackupDlg::OnBnClickedClearAllMedia)
END_MESSAGE_MAP()

void CBackupDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);

	CDialog::DoDataExchange(pDX);
}

// CBackupDlg 消息处理程序
BOOL CBackupDlg::OnInitDialog()
{
	__super::OnInitDialog();

	CRect rcClient;
	m_wndList.GetClientRect(rcClient);

	CListColumnGuard ColumnGuard(rcClient.Width() - m_view.m_globalSize.m_uScrollbarWidth);
	ColumnGuard.addFix(L"时间", 250)
		.addDynamic(L"新增项", 0.25, true)
		.addDynamic(L"删除项", 0.25, true)
		.addDynamic(L"修改项", 0.25, true)
		.addDynamic(L"移动项", 0.25, true);

	CObjectList::tagListPara ListPara(ColumnGuard);
	ListPara.uItemHeight = 60;
	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	(void)m_wndList.ModifyStyle(0, LVS_SINGLESEL);
	
	auto& arrBackupTag = m_BackupMgr.getBackupTags();
	if (arrBackupTag)
	{
		async([&]() {
			m_wndList.SelectItem(0);
			m_wndList.SetFocus();

			if (arrBackupTag.size() <= 1)
			{
				return;
			}

			wstring strPrevTag;
			SList<pair<wstring, wstring>> lstTask;
			for (auto& strTag : arrBackupTag)
			{
				if (!strPrevTag.empty())
				{
					lstTask.add({ strPrevTag, strTag });
				}

				strPrevTag = strTag;
			}

			SList<tagCompareBackupResult> lstResult;
			m_BackupMgr.compareBackup(lstTask, lstResult);
			for (auto& result : lstResult)
			{
				m_mapCompareBackupResult.set(result.strDstTag, result);
			}
		});
	}
	
	Refresh();

	return true;
}

#define __tostring(x) (0==(x)?L"-":to_wstring(x))

#define __compareResult(result) {__tostring(result.arrNewPlayItem.size() + result.arrNewAlbumItem.size()) \
	, __tostring(result.arrDeletedPlayItem.size() + result.arrDeletedAlbumItem.size()) \
	, __tostring(result.arrModifyedMedia.size()) \
	, __tostring(result.arrMovedMedia.size())}


void CBackupDlg::Refresh()
{
	m_wndList.DeleteAllItems();

	static SMap<wstring, pair<UINT, wstring>> s_mapEmptyRow;
	s_mapEmptyRow.clear();

	wstring strPrevTag;
	UINT uRow = 0;

	auto arrBackupTag = m_BackupMgr.getBackupTags();
	for (auto& strTag : arrBackupTag)
	{
		if (0 == uRow)
		{
			m_wndList.InsertItemEx(0, { strTag, L"-", L"-", L"-", L"-" }, L"  ");
		}
		else if (!m_mapCompareBackupResult.get(strTag, [&](tagCompareBackupResult& result) {
			SArray<wstring> arrText(strTag);
			arrText.add(__compareResult(result));

			m_wndList.InsertItemEx(0, arrText, L"  ");
		}))
		{
			m_wndList.InsertItem(0, (L"  " + strTag).c_str());

			s_mapEmptyRow.insert(strTag, { arrBackupTag.size() - 1 - uRow, strPrevTag });
		}

		strPrevTag = strTag;
		uRow++;
	}

	if (!s_mapEmptyRow)
	{
		return;
	}
	
	m_timer.set(500, [&]() {
		SList<pair<wstring, wstring>> lstTask;
		s_mapEmptyRow([&](auto& strTag, auto& pr) {
			lstTask.add({pr.second, strTag});
		});

		SList<tagCompareBackupResult> lstResult;
		m_BackupMgr.getCompareResult(lstTask, lstResult);
		for (auto& result : lstResult)
		{
			s_mapEmptyRow.del(result.strDstTag, [&](auto& pr) {
				m_mapCompareBackupResult.set(result.strDstTag, result);

				SArray<wstring> arrText(__compareResult(result));

				m_wndList.SetItemTexts(pr.second.first, 1, arrText, L"  ");
			});
		}

		if (!s_mapEmptyRow)
		{
			return false;
		}

		return true;
	});
}

void CBackupDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(m_wndList.GetSelectedCount()==1);
}

void CBackupDlg::OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);

	wstring strTag = m_wndList.GetItemText(nItem, 0);
	strutil::ltrim(strTag);

	CBackupCompareDlg CompareDlg(m_view, strTag, false);
	CompareDlg.DoModal();
}

void CBackupDlg::OnBnClickedRestore()
{
	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);

	__Ensure(confirmBox(L"确认使用选中的备份来恢复曲目库？"));

	wstring strTag = m_wndList.GetItemText(nItem, 0);
	strutil::ltrim(strTag);

	this->OnCancel();
	
	CRedrawLock RedrawLock(m_view.m_MainWnd);
	m_view.getModel().restoreDB(strTag);
}

void CBackupDlg::OnBnClickedCompare()
{
	int nItem = m_wndList.GetSelItem();

	wstring strTag;
	if (nItem >= 0)
	{
		strTag = m_wndList.GetItemText(nItem, 0);
		strutil::ltrim(strTag);
	}

	CBackupCompareDlg CompareDlg(m_view, strTag, true);
	CompareDlg.DoModal();
}

void CBackupDlg::OnBnClickedDel()
{
	int nItem = m_wndList.GetSelItem();
	__Ensure(nItem >= 0);

	__Ensure(confirmBox(L"确认删除选中的备份？"));

	wstring strTag = m_wndList.GetItemText(nItem, 0);
	strutil::ltrim(strTag);

	m_BackupMgr.removeBackup(strTag);

	m_wndList.DeleteItem(nItem);

	if (nItem > 0)
	{
		wstring strNextTag = m_wndList.GetItemText(nItem - 1, 0);
		if (nItem < m_wndList.GetItemCount())
		{
			strutil::ltrim(strNextTag);

			wstring strPrevTag = m_wndList.GetItemText(nItem, 0);
			strutil::ltrim(strPrevTag);

			m_BackupMgr.compareBackup(strPrevTag, strNextTag);

			m_mapCompareBackupResult.del(strNextTag);

			Refresh();
		}
		else
		{
			m_wndList.SetItemTexts(nItem - 1, { strNextTag, L"-", L"-" , L"-", L"-" });
		}
	}
}

void CBackupDlg::OnBnClickedBackup()
{
	wstring strPrevTag;
	if (m_wndList.GetItemCount() > 0)
	{
		strPrevTag = m_wndList.GetItemText(0, 0);
		strutil::ltrim(strPrevTag);
	}

	wstring strNewTag = m_view.getModel().backupDB();
	__Ensure(!strNewTag.empty());
	
	Refresh();
	m_wndList.SelectFirstItem();

	if (!strPrevTag.empty())
	{
		m_BackupMgr.compareBackup(strPrevTag, strNewTag);
	}
}

void CBackupDlg::OnBnClickedClearAllMedia()
{
	__Ensure(confirmBox(L"确认清空所有歌单、歌手、专辑？"));

	__Ensure(m_view.getModel().clearData());
}

void CBackupDlg::OnCancel()
{
	m_timer.kill();
	CDialog::OnCancel();
}
