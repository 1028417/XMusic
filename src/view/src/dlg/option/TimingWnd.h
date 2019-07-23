#pragma once

class CTimingWnd : public CWnd
{
public:
	CTimingWnd(__view& view);

private:
	__view& m_view;

	//tagTimerOperateOpt& m_TimerOperateOpt;

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

private:
	CComboBox m_wndOperateCombo;

public:
	BOOL Create(CWnd* pParentWnd, int x, int y);

public:
	virtual BOOL SetOccDialogInfo(struct _AFX_OCC_DIALOG_INFO* pOccDialogInfo)
	{
		return TRUE;
	}

	afx_msg void OnCbnSelchangeComboOperate();

	afx_msg void OnDeltaposSpinHour(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMinute(NMHDR *pNMHDR, LRESULT *pResult);
};
