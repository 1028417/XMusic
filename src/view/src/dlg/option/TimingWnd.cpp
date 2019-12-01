
#include "stdafx.h"
#include "TimingWnd.h"

CTimingWnd::CTimingWnd(__view& view)
	: m_view(view)
{
}

void CTimingWnd::DoDataExchange(CDataExchange* pDX)
{
	CWnd::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_OPERATE, m_wndOperateCombo);
}

BEGIN_MESSAGE_MAP(CTimingWnd, CWnd)
	ON_CBN_SELCHANGE(IDC_COMBO_OPERATE, &CTimingWnd::OnCbnSelchangeComboOperate)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_HOUR, &CTimingWnd::OnDeltaposSpinHour)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MINUTE, &CTimingWnd::OnDeltaposSpinMinute)
END_MESSAGE_MAP()


BOOL CTimingWnd::Create(CWnd* pParentWnd, int x, int y)
{
	__AssertReturn(this->CreateDlg(MAKEINTRESOURCE(IDD_DLG_TIMING), pParentWnd), FALSE);
	(void)this->UpdateData(FALSE);
	(void)this->SetWindowPos(NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	auto& TimerOption = COptionMgr::inst().getTimerOption();
	this->SetDlgItemInt(IDC_EDIT_HOUR, TimerOption.nHour);
	this->SetDlgItemInt(IDC_EDIT_MINUTE, TimerOption.nMinute);

	(void)m_wndOperateCombo.InsertString(TO_Null, _T("未指定"));
	(void)m_wndOperateCombo.InsertString(TO_StopPlay, _T("停止播放"));
	(void)m_wndOperateCombo.InsertString(TO_Shutdown, _T("关机"));
	(void)m_wndOperateCombo.InsertString(TO_Hibernal, _T("休眠"));

	(void)m_wndOperateCombo.SetCurSel(TimerOption.eTimerOperate);

	this->SetDlgItemInt(IDC_EDIT_HOUR, TimerOption.nHour);
	this->SetDlgItemInt(IDC_EDIT_MINUTE, TimerOption.nMinute);


	(void)this->ShowWindow(SW_SHOW);

	return TRUE;
}

void CTimingWnd::OnCbnSelchangeComboOperate()
{
	COptionMgr::inst().getTimerOption().eTimerOperate = (E_TimerOperate)m_wndOperateCombo.GetCurSel();
}

void CTimingWnd::OnDeltaposSpinHour(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	int& nHour = COptionMgr::inst().getTimerOption().nHour;
	nHour -= pNMUpDown->iDelta;

	if (nHour >= 24)
	{
		nHour = 0;
	}
	else if (nHour < 0)
	{
		nHour = 23;
	}

	this->SetDlgItemInt(IDC_EDIT_HOUR, nHour);
}

void CTimingWnd::OnDeltaposSpinMinute(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

	int& nMinute = COptionMgr::inst().getTimerOption().nMinute;
	nMinute -= pNMUpDown->iDelta;

	if (nMinute >= 60)
	{
		nMinute = 0;
	}
	else if (nMinute < 0)
	{
		nMinute = 59;
	}

	this->SetDlgItemInt(IDC_EDIT_MINUTE, nMinute);
}
