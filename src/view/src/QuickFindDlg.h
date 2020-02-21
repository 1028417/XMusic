#pragma once

class CQuickFindDlg : public TDialog<0, false, false>
{
public:
	CQuickFindDlg(__view& view)
		: m_view(view)
	{
	}

private:
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

private:
	__view& m_view;

	CTouchWnd<CEdit> m_Edit;

	CButton m_Button;
	
	UINT m_uTextHeight = 0;
	
public:
	BOOL OnInitDialog() override;

	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnBnClickedButton1();

	BOOL PreTranslateMessage(MSG* pMsg) override;
};
