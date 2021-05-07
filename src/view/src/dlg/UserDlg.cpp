
#include "stdafx.h"

#include "UserDlg.h"

CUserDlg::CUserDlg(__view& view)
	: m_view(view)
{
}

void CUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_User, m_wndUserEdit);
	DDX_Control(pDX, IDC_EDIT_Pwd, m_wndPwdEdit);
	DDX_Control(pDX, IDC_EDIT_VipDays, m_wndVipDaysEdit);
	DDX_Control(pDX, IDC_CHECK_MultiLogin, m_wndMultiLoginCheck);
	DDX_Control(pDX, IDC_CHECK_XPkg, m_wndXPkgCheck);
}

BEGIN_MESSAGE_MAP(CUserDlg, CDialog)
	ON_EN_CHANGE(IDC_EDIT_User, &CUserDlg::OnEnChangeEditUser)
	ON_BN_CLICKED(IDC_BTN_DelUser, &CUserDlg::OnBnClickedBtnDeluser)
	ON_BN_CLICKED(IDC_BTN_Signup, &CUserDlg::OnBnClickedBtnSignup)
END_MESSAGE_MAP()

void CUserDlg::OnEnChangeEditUser()
{
	string strPwd;
	uint64_t tVip = 0;
	UINT uAuth = 0;
	m_view.getModel().getUserMgr().qurryUser(_user(), strPwd, tVip, uAuth);

	m_wndPwdEdit.SetWindowText(strutil::fromAsc(strPwd).c_str());
	
	wstring strVipDays;
	if (tVip)
	{
		auto nTimeOut = (int64_t)tVip - time(0);
		auto nDays = nTimeOut / (3600 * 24);
		strVipDays = to_wstring(nDays);
	}
	m_wndVipDaysEdit.SetWindowText(strVipDays.c_str());

	m_wndMultiLoginCheck.SetCheck(uAuth & UA_MultiLogin);
	m_wndXPkgCheck.SetCheck(uAuth & UA_Xpk);
}

void CUserDlg::OnBnClickedBtnDeluser()
{
	if (m_view.getModel().getUserMgr().removeUser(_user()))
	{
		this->msgBox(L"删除用户成功");
	}
	else
	{
		this->msgBox(L"删除用户失败");
	}
}

void CUserDlg::OnBnClickedBtnSignup()
{
	CString cstrPwd;
	m_wndPwdEdit.GetWindowText(cstrPwd);
	cstrPwd.Trim();
	string strPwd = strutil::toAsc((LPCWSTR)cstrPwd);

	uint64_t tVip = 0;
	CString cstrVipDays;
	m_wndVipDaysEdit.GetWindowText(cstrVipDays);
	if (!cstrVipDays.IsEmpty())
	{
		auto nDays = atoll(strutil::toAsc((LPCWSTR)cstrVipDays).c_str());

		auto nTimeout = int64_t(3600 * 24) * nDays;
		tVip = (uint64_t)time(0) + nTimeout;
	}

	UINT uAuth = 0;
	if (m_wndMultiLoginCheck.GetCheck())
	{
		uAuth |= UA_MultiLogin;
	}
	if (m_wndXPkgCheck.GetCheck())
	{
		uAuth |= UA_Xpk;
	}

	if (m_view.getModel().getUserMgr().signupUser(_user(), strPwd, tVip, uAuth))
	{
		this->msgBox(L"提交用户成功");
	}
	else
	{
		this->msgBox(L"提交用户失败");
	}
}
