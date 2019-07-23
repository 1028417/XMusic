#pragma once

class CAlarmClockWnd : public CWnd
{
public:
	CAlarmClockWnd(__view& view)
		: m_view(view)
	{
	}

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

private:
	__view& m_view;

	CComboBox m_wndMediaResCombo;

public:
	BOOL Create(CWnd* pParentWnd, int x, int y);

	virtual BOOL SetOccDialogInfo(struct _AFX_OCC_DIALOG_INFO* pOccDialogInfo)
	{
		return TRUE;
	}

	afx_msg void OnCbnSelchangeComboMedia();

	afx_msg void OnBnClickedRemove();
	afx_msg void OnBnClickedClear();

	afx_msg void OnDeltaposSpinHour(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinMinute(NMHDR *pNMHDR, LRESULT *pResult);
};
