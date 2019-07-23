
#include "stdafx.h"

#include "ExportOptionDlg.h"

BEGIN_MESSAGE_MAP(CExportOptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CExportOptionDlg::OnBnClickedOK)

	ON_BN_CLICKED(IDC_CompareFileSize, &CExportOptionDlg::OnBnClickedCompareFileSize)
END_MESSAGE_MAP()

BOOL CExportOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_ExportOption.bActualMode)
	{
		auto pBtn = ((CButton*)GetDlgItem(IDC_ActualMode));
		pBtn->SetCheck(TRUE);
		pBtn->EnableWindow(FALSE);
	}

	((CButton*)GetDlgItem(IDC_ActualMode))->SetCheck(TRUE);
	
	((CButton*)GetDlgItem(IDC_CompareFileSize))->SetCheck(TRUE);
	OnBnClickedCompareFileSize();

	//((CButton*)GetDlgItem(IDC_DeleteOther))->SetCheck(TRUE);

	return TRUE;
}

void CExportOptionDlg::OnBnClickedCompareFileSize()
{
	BOOL bFlag = ((CButton*)GetDlgItem(IDC_CompareFileSize))->GetCheck();
	
	auto pwndCheck = GetDlgItem(IDC_CompareFileTime);
	pwndCheck->EnableWindow(bFlag);
	if (!bFlag)
	{
		((CButton*)pwndCheck)->SetCheck(FALSE);
	}
}

void CExportOptionDlg::OnBnClickedOK()
{
	m_ExportOption.bActualMode = ((CButton*)GetDlgItem(IDC_ActualMode))->GetCheck();

	if (((CButton*)GetDlgItem(IDC_CompareFileSize))->GetCheck())
	{
		m_ExportOption.bCompareFileSize = true;
		m_ExportOption.bCompareFileTime = ((CButton*)GetDlgItem(IDC_CompareFileTime))->GetCheck();
	}

	m_ExportOption.bDeleteOther = ((CButton*)GetDlgItem(IDC_DeleteOther))->GetCheck();

	CDialog::OnOK();
}
