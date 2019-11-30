// OptionDlg.cpp : 实现文件
//

#include "stdafx.h"

#include "OptionDlg.h"

COptionDlg::COptionDlg(__view& view)
	: m_view(view)
	, m_TimingWnd(view)
	, m_AlarmClockWnd(view)
{
}

void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SKIN, m_wndSkbCombo);
}

BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_SKIN, &COptionDlg::OnCbnSelchangeComboSkin)
	ON_BN_CLICKED(IDC_HideMenuBar, &COptionDlg::OnBnClickedHideMenubar)
END_MESSAGE_MAP()

// COptionDlg 消息处理程序
void findSkb(list<wstring>& lstSkbName)
{
	list<WIN32_FIND_DATAW> lstFindData;

	(void)fsutil::findSubFile(__PlaySpiritSkinDir, [&](const wstring& strSubFile) {
		lstSkbName.push_back(fsutil::getFileTitle(strSubFile));
	}, E_FindFindFilter::FFP_ByExt, L"skb");
}

BOOL COptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	if (m_view.getOptionMgr().getOption().bHideMenuBar)
	{
		((CButton*)GetDlgItem(IDC_HideMenuBar))->SetCheck(TRUE);
	}

	(void)m_wndSkbCombo.InsertString(0, _T("默认"));
	m_wndSkbCombo.SetCurSel(0);

	list<wstring> lstSkbName;
	findSkb(lstSkbName);

	int nIndex = 1;
	for (auto& strSkbName : lstSkbName)
	{
		(void)m_wndSkbCombo.InsertString(nIndex++, strSkbName.c_str());
	}
	(void)m_wndSkbCombo.SelectString(0, m_view.getOptionMgr().getPlaySpiritOption().strSkinName.c_str());

	__EnsureReturn(m_TimingWnd.Create(this, 13, 130), FALSE);

	__EnsureReturn(m_AlarmClockWnd.Create(this, 13, 240), FALSE);

	return TRUE;
}

void COptionDlg::OnCbnSelchangeComboSkin()
{
	wstring strSkinName;
	wstring strSkinPath;
	if (m_wndSkbCombo.GetCurSel() > 0)
	{
		CString cstrText;
		m_wndSkbCombo.GetLBText(m_wndSkbCombo.GetCurSel(), cstrText);
		strSkinName = cstrText;

		strSkinPath = __PlaySpiritSkinDir + strSkinName + L".skb";
	}

	m_view.getOptionMgr().getPlaySpiritOption().strSkinName = strSkinName;

	m_view.m_PlayCtrl.getPlaySpirit().SetSkin(strSkinPath);
}

void COptionDlg::OnBnClickedHideMenubar()
{
	auto& bHideMenuBar = m_view.getOptionMgr().getOption().bHideMenuBar;
	bHideMenuBar = !bHideMenuBar;
}
