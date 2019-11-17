#pragma once

#include "TimingWnd.h"

#include "AlarmClockWnd.h"

// COptionDlg 对话框

class COptionDlg : public CDialogT<IDD_DLG_Option>
{
public:
	COptionDlg(__view& view);

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	__view& m_view;

	CComboBox m_wndSkbCombo;
	
	CTimingWnd m_TimingWnd;
	
	CAlarmClockWnd m_AlarmClockWnd;

public:
	virtual BOOL OnInitDialog() override;

	afx_msg void OnCbnSelchangeComboSkin();
	afx_msg void OnBnClickedHideMenubar();
};
