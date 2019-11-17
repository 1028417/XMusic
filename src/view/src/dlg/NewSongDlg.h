#pragma once

class CNewSongDlg : public CDialogT<IDD_DLG_NewSong>
{
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

public:
	CNewSongDlg(__view& view);   // 标准构造函数

private:
	__view& m_view;

	TD_MediaList m_lstSrcMedias;

	time_t m_tFilterTime = 0;
	int m_nSortType = 0;

	CMediaMixer m_MediaMixer;

	CDateTimeCtrl m_wndDateTimeCtrl;

	CComboBox m_wndCombo;

	CObjectList m_wndList;

private:
	void Refresh();

	const TD_MediaMixtureVector& _GetMediaMixture();

public:
	BOOL OnInitDialog() override;

	afx_msg void OnDtnDatetimechange(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnSelChangeCombo1();

	afx_msg void OnBnClickedCheck1();

	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	
	afx_msg void OnBnClickedBtnDemand();

	afx_msg void OnBnClickedBtnPlay();
};
