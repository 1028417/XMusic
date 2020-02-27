// QuickPage.cpp : 实现文件
//

#include "stdafx.h"
#include "QuickFindDlg.h"
#include "dlg/FindDlg.h"

void CQuickFindDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit);
	DDX_Control(pDX, IDC_BUTTON1, m_Button);
}

BEGIN_MESSAGE_MAP(CQuickFindDlg, TDialog)
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON1, &CQuickFindDlg::OnBnClickedButton1)
	//ON_EN_SETFOCUS(IDC_EDIT1, &CQuickFindDlg::OnEnSetfocusEdit1)
END_MESSAGE_MAP()

// CQuickFindDlg 消息处理程序

BOOL CQuickFindDlg::OnInitDialog()
{
	__super::OnInitDialog();

	static CCompatableFont font;
	(void)font.setFont(m_Edit, m_view.m_globalSize.m_fMidFontSize);

	auto pDC = m_Edit.GetDC();
	pDC->SelectObject(font);
	CSize sz = pDC->GetTextExtent(L"我");
	m_uTextHeight = UINT(sz.cy*1.1);
	m_Edit.ReleaseDC(pDC);
	
	m_Edit.SetLimitText(MAX_PATH);
	m_Edit.SetMargins(13, 0);

	((CButton*)GetDlgItem(IDC_BUTTON1))->SetIcon(m_view.m_ResModule.loadIcon(IDI_Find));
	
	return TRUE;
}

HBRUSH CQuickFindDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_DLG)
	{
		return (HBRUSH)::GetStockObject(WHITE_BRUSH);
	}

	return __super::OnCtlColor(pDC, pWnd, nCtlColor);
}

void CQuickFindDlg::OnSize(UINT nType, int cx, int cy)
{
	if (m_Edit)
	{
		int nBtnWidth = cy + 12;

		m_Button.MoveWindow(cx - nBtnWidth + 1, -1, nBtnWidth, cy+1);

		m_Edit.MoveWindow(0, (cy - m_uTextHeight) / 2, cx - nBtnWidth-8, m_uTextHeight);
	}
}

void CQuickFindDlg::OnBnClickedButton1()
{
	CString cstrText;
	m_Edit.GetWindowText(cstrText);
	cstrText.Trim();

	(void)m_Edit.SetFocus();

	if (!cstrText.IsEmpty())
	{
		m_view.showFindDlg((wstring)cstrText, true);
	}
}

BOOL CQuickFindDlg::PreTranslateMessage(MSG* pMsg)
{
	static bool bFlag = false;
	switch (pMsg->message)
	{
	case WM_MOUSEMOVE:
		if (m_view.m_MainWnd.m_bActivate && pMsg->hwnd != m_Button.m_hWnd)
		{
			(void)m_Edit.SetFocus();
		}

		break;
	case WM_KEYDOWN:
		if (VK_RETURN == pMsg->wParam)
		{
			bFlag = true;
		}

		break;
	case WM_KEYUP:
		if (bFlag && VK_RETURN == pMsg->wParam)
		{
			bFlag = false;

			OnBnClickedButton1();
			return TRUE;
		}

		break;
	case WM_LBUTTONDBLCLK:
		if (pMsg->hwnd == m_Edit.m_hWnd)
		{
			m_Edit.SetSel(0, -1); // OnEnSetfocusEdit1();
			return TRUE;
		}

		break;
	}

	return __super::PreTranslateMessage(pMsg);
}
