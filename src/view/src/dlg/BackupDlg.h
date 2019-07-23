#pragma once

class CBackupDlg : public CDialogT<IDD_DLG_BACKUP>
{
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

public:
	CBackupDlg(__view& view)
		: m_view(view)
		, m_BackupMgr(view.getModel().getBackupMgr())
	{
	}

private:
	__view& m_view;

	CBackupMgr& m_BackupMgr;

	SHashMap<wstring, tagCompareBackupResult> m_mapCompareBackupResult;

	CObjectList m_wndList;

	CWinTimer m_timer;

private:
	BOOL OnInitDialog();

	void Refresh();

	void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

	void OnBnClickedBackup();
	void OnBnClickedCompare();
	void OnBnClickedDel();
	void OnBnClickedRestore();
	void OnBnClickedClearMedialib();

	void OnCancel();
};
