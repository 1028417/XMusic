#pragma once

class CPlayRecord : public CMedia
{
public:
	CPlayRecord()
	{
	}

	CPlayRecord(const wstring& strPath, dbtime_t tTime)
		: CMedia(NULL, 0, strPath, tTime)
	{
	}

public:
	void GenListItem(bool bReportView, vector<wstring>& vecText, int& iImage) override
	{
		vecText.push_back(m_addTime.GetText(false));
		vecText.push_back(GetPath());
	}
};


// CPlayRecordDlg 对话框

class CPlayRecordDlg : public TDialog<IDD_DLG_PlayRecord>
{
public:
	CPlayRecordDlg(__view& view);   // 标准构造函数

private:
	__view& m_view;

	time_t m_tFilterTime = 0;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	list<CPlayRecord> m_vctPlayRecord;

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
