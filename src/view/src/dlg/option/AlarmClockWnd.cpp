
#include "stdafx.h"
#include "AlarmClockWnd.h"

void CAlarmClockWnd::DoDataExchange(CDataExchange* pDX)
{
	CWnd::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MEDIA, m_wndMediaResCombo);
}

BEGIN_MESSAGE_MAP(CAlarmClockWnd, CWnd)
	ON_CBN_SELCHANGE(IDC_COMBO_MEDIA, &CAlarmClockWnd::OnCbnSelchangeComboMedia)

	ON_BN_CLICKED(IDC_BTN_REMOVE, &CAlarmClockWnd::OnBnClickedRemove)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CAlarmClockWnd::OnBnClickedClear)

	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HOUR, &CAlarmClockWnd::OnDeltaposSpinHour)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINUTE, &CAlarmClockWnd::OnDeltaposSpinMinute)
END_MESSAGE_MAP()


BOOL CAlarmClockWnd::Create(CWnd* pParentWnd, int x, int y)
{
	__AssertReturn(this->CreateDlg(MAKEINTRESOURCE(IDD_DLG_ALARMCLOCK), pParentWnd), FALSE);
	(void)this->UpdateData(FALSE);
	(void)this->SetWindowPos(NULL, x, y, 0, 0, SWP_NOZORDER|SWP_NOSIZE);

	m_wndMediaResCombo.SetDroppedWidth(350);
	(void)m_wndMediaResCombo.InsertString(0, L"");

	UINT uIndex = 0;
	for (auto& strAlarmmedia : m_view.getOptionMgr().getAlarmOption().vctAlarmmedia)
	{
		(void)m_wndMediaResCombo.InsertString(++uIndex, strAlarmmedia.c_str());
	}

	m_wndMediaResCombo.SetCurSel(0);
	this->OnCbnSelchangeComboMedia();

	this->SetDlgItemInt(IDC_EDIT_HOUR, m_view.getOptionMgr().getAlarmOption().nHour);
	this->SetDlgItemInt(IDC_EDIT_MINUTE, m_view.getOptionMgr().getAlarmOption().nMinute);


	(void)this->ShowWindow(SW_SHOW);

	return TRUE;
}

void CAlarmClockWnd::OnCbnSelchangeComboMedia()
{
	(void)GetDlgItem(IDC_BTN_CLEAR)->EnableWindow(0<m_wndMediaResCombo.GetCount());
	(void)GetDlgItem(IDC_BTN_REMOVE)->EnableWindow(0<=m_wndMediaResCombo.GetCurSel());
}

void CAlarmClockWnd::OnBnClickedRemove()
{
	CString cstrAlarmmedia;
	(void)m_wndMediaResCombo.GetWindowText(cstrAlarmmedia);

	int iIndex = m_wndMediaResCombo.GetCurSel();
	__Ensure(iIndex > 0);
	
	__Ensure(m_view.getDataMgr().removeAlarmmedia((UINT)iIndex - 1));
	
	(void)m_wndMediaResCombo.DeleteString(iIndex);

	m_wndMediaResCombo.SetCurSel(0);
	this->OnCbnSelchangeComboMedia();
}

void CAlarmClockWnd::OnBnClickedClear()
{
	__Ensure(m_view.getDataMgr().clearAlarmmedia());

	int nIndex = m_wndMediaResCombo.GetCount() - 1;
	while (0 <= nIndex)
	{
		(void)m_wndMediaResCombo.DeleteString(nIndex);

		nIndex--;
	}

	this->OnCbnSelchangeComboMedia();
}

void CAlarmClockWnd::OnDeltaposSpinHour(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	int& nHour = m_view.getOptionMgr().getAlarmOption().nHour;
	nHour -= pNMUpDown->iDelta;

	if (24 <= nHour)
	{
		nHour = 0;
	}
	else if (0 > nHour)
	{
		nHour = 23;
	}

	this->SetDlgItemInt(IDC_EDIT_HOUR, nHour);
}

void CAlarmClockWnd::OnDeltaposSpinMinute(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	int& nMinute = m_view.getOptionMgr().getAlarmOption().nMinute;
	nMinute -= pNMUpDown->iDelta;

	if (60 <= nMinute)
	{
		nMinute = 0;
	}
	else if (0 > nMinute)
	{
		nMinute = 59;
	}

	this->SetDlgItemInt(IDC_EDIT_MINUTE, nMinute);
}
