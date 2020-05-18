#pragma once

// CPlayRecordDlg �Ի���

class CPlayRecordDlg : public TDialog<IDD_DLG_PlayRecord>
{
public:
	CPlayRecordDlg(__view& view);   // ��׼���캯��

private:
	__view& m_view;

	time_t m_tFilterTime = 0;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	prvector<wstring, int> m_pvPlayRecord;

	CDateTimeCtrl m_wndDateTimeCtrl;

	CObjectList m_wndList;
	
private:
	BOOL Refresh();

public:
	virtual BOOL OnInitDialog() override;

	afx_msg void OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedPlay();
	afx_msg void OnBnClickedClear();
};
