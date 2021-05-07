#pragma once

// COptionDlg 对话框

class CUserDlg : public TDialog<IDD_DLG_User>
{
public:
	CUserDlg(__view& view);

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	__view& m_view;

	CEdit m_wndUserEdit;
	CEdit m_wndPwdEdit;
	CEdit m_wndVipDaysEdit;
	CButton m_wndMultiLoginCheck;
	CButton m_wndXPkgCheck;

private:
	wstring _user() const
	{
		CString cstrUser;
		m_wndUserEdit.GetWindowText(cstrUser);
		cstrUser.Trim();
		return (LPCWSTR)cstrUser;
	}

public:
	afx_msg void OnEnChangeEditUser();
	afx_msg void OnBnClickedBtnDeluser();
	afx_msg void OnBnClickedBtnSignup();
};
