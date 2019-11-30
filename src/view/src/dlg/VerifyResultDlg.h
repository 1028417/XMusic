#pragma once

class CVerifyResultDlg : public CDialogT<IDD_DLG_VerifyResult>
{
public:
	CVerifyResultDlg(__view& view, tagVerifyResult& VerifyResult)
		: m_view(view)
		, m_VerifyResult(VerifyResult)
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

	DECLARE_MESSAGE_MAP()

private:
	__view& m_view;

	tagVerifyResult& m_VerifyResult;

	CObjectList m_wndList;

private:
	void UpdateItem(int nItem, CMedia& media);
	
	void LinkMedia(int nItem, CMedia& media);

public:
	BOOL OnInitDialog() override;

	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnNMDBLClickList1(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnBnClickedAutoMatch();

	void OnBnClickedLink();

	void OnBnClickedVerify();

	void OnBnClickedRemove();
};
