
#include "StdAfx.h"

#include "ChooseDirDlg.h"

CChooseDirDlg::CChooseDirDlg(const wstring& strTitle, CMediaDir& RootDir, bool bShowRoot, wstring& strRetDir)
	: m_strTitle(strTitle)
	, m_RootDir(RootDir)
	, m_strRetDir(strRetDir)
{
	m_bShowRoot = bShowRoot;
}

void CChooseDirDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TREE, m_wndTree);

	__super::DoDataExchange(pDX);
}

BOOL CChooseDirDlg::OnInitDialog()
{
	(void)__super::OnInitDialog();

	this->SetWindowText(m_strTitle.c_str());

	TD_IconVec lstIcon{ winfsutil::getFolderIcon() };
	__AssertReturn(m_wndTree.InitImglst(CSize(30, 30), lstIcon), TRUE);
	
	m_wndTree.SetRootDir(m_RootDir, m_bShowRoot);

	return TRUE;
}

void CChooseDirDlg::OnOK()
{
	CMediaDir *pMediaDir = (CMediaDir*)m_wndTree.GetSelectedObject();
	if (pMediaDir)
	{
		m_strRetDir = pMediaDir->absPath();
	}

	CDialog::OnOK();
}
