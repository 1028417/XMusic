
#include "stdafx.h"

#include "ExportOptionDlg.h"

BEGIN_MESSAGE_MAP(CExportOptionDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CExportOptionDlg::OnBnClickedOK)

	ON_BN_CLICKED(IDC_ActualMode, &CExportOptionDlg::OnBnClickedActualMode)

	ON_BN_CLICKED(IDC_ExportXmsc, &CExportOptionDlg::OnBnClickedExportXmsc)

	ON_BN_CLICKED(IDC_CompareFileSize, &CExportOptionDlg::OnBnClickedCompareFileSize)
END_MESSAGE_MAP()

BOOL CExportOptionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	auto pBtn = ((CButton*)GetDlgItem(IDC_ActualMode));
	pBtn->SetCheck(TRUE);

	if (m_ExportOption.bActualMode)
	{
		pBtn->EnableWindow(FALSE);
	}
		
	((CButton*)GetDlgItem(IDC_CompareFileSize))->SetCheck(TRUE);
	OnBnClickedCompareFileSize();

	//((CButton*)GetDlgItem(IDC_DeleteOther))->SetCheck(TRUE);

	return TRUE;
}

void CExportOptionDlg::OnBnClickedActualMode()
{
	BOOL bFlag = ((CButton*)GetDlgItem(IDC_ActualMode))->GetCheck();

	auto pbtnCheck = (CButton*)GetDlgItem(IDC_ExportXmsc);
	pbtnCheck->EnableWindow(bFlag);
	if (!bFlag)
	{
		pbtnCheck->SetCheck(FALSE);
	}
}

void CExportOptionDlg::OnBnClickedExportXmsc()
{
	BOOL bFlag = !((CButton*)GetDlgItem(IDC_ExportXmsc))->GetCheck();

	auto pbtnCheck = (CButton*)GetDlgItem(IDC_CompareFileSize);
	pbtnCheck->EnableWindow(bFlag);
	if (!bFlag)
	{
		pbtnCheck->SetCheck(FALSE);
	}

	pbtnCheck = (CButton*)GetDlgItem(IDC_CompareFileTime);
	pbtnCheck->EnableWindow(bFlag);
	if (!bFlag)
	{
		pbtnCheck->SetCheck(FALSE);
	}	
}

void CExportOptionDlg::OnBnClickedCompareFileSize()
{
	BOOL bFlag = ((CButton*)GetDlgItem(IDC_CompareFileSize))->GetCheck();
	
	auto pbtnCheck = (CButton*)GetDlgItem(IDC_CompareFileTime);
	pbtnCheck->EnableWindow(bFlag);
	if (!bFlag)
	{
		pbtnCheck->SetCheck(FALSE);
	}
}

void CExportOptionDlg::OnBnClickedOK()
{
	m_ExportOption.bActualMode = ((CButton*)GetDlgItem(IDC_ActualMode))->GetCheck();

	m_ExportOption.bExportXmsc = ((CButton*)GetDlgItem(IDC_ExportXmsc))->GetCheck();

	if (((CButton*)GetDlgItem(IDC_CompareFileSize))->GetCheck())
	{
		m_ExportOption.bCompareFileSize = true;
		m_ExportOption.bCompareFileTime = ((CButton*)GetDlgItem(IDC_CompareFileTime))->GetCheck();
	}

	m_ExportOption.bDeleteOther = ((CButton*)GetDlgItem(IDC_DeleteOther))->GetCheck();

	m_ExportOption.bExportDB = ((CButton*)GetDlgItem(IDC_ExportDB))->GetCheck();
	
	CDialog::OnOK();
}
