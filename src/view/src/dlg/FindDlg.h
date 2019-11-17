#pragma once

class CFindDlg : public CDialogT<IDD_DLG_FIND>
{
public:
	CFindDlg(__view& view)
		: m_view(view)
	{
	}
	
	CFindDlg(__view& view, bool bQuickHittest, E_FindMediaMode eFindMediaMode, const wstring& strFindText, const wstring& strFindSingerName=L"")
		: m_view(view)
		, m_strFindText(strFindText)
		, m_strFindSingerName(strFindSingerName)
	{
		m_bQuickHittest = bQuickHittest;

		m_eFindMediaMode = eFindMediaMode;
	}

private:
	__view& m_view;

	bool m_bQuickHittest = false;

	E_FindMediaMode m_eFindMediaMode = E_FindMediaMode::FMM_MatchText;

	wstring m_strFindText;
	
	wstring m_strFindSingerName;

	CMediaMixer m_MediaMixer;

	CTouchWnd<CEdit> m_wndEdit;

	CObjectList m_wndList;

	CString m_cstrPrevFind;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

	DECLARE_MESSAGE_MAP()

private:
	const TD_MediaMixtureVector& FindMedia(E_FindMediaMode eFindMediaMode, const wstring& strFindText, const wstring& strFindSingerName=L"");

	void Refresh();

	virtual BOOL OnInitDialog() override;
	
	afx_msg void OnEnChangeEdit1();

	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);
	
	void OnBnClickedPlay();
	afx_msg void OnBnClickedBtnVerify();
	afx_msg void OnBnClickedBtnExport();
	afx_msg void OnBnClickedBtnExplore();
};
