
#include "StdAfx.h"

#include "ChooseDirDlg.h"

CChooseDirDlg::CChooseDirDlg(const wstring& strTitle, const wstring& strRootDir, bool bShowRoot, wstring& strRetDir)
	: m_strTitle(strTitle)
	, m_mediaLibDir(strRootDir)
	, m_strRetDir(strRetDir)
{
	m_bShowRoot = bShowRoot;
}

void CChooseDirDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TREE, m_wndBrowseTree);

	__super::DoDataExchange(pDX);
}

BOOL CChooseDirDlg::OnInitDialog()
{
	(void)__super::OnInitDialog();

	this->SetWindowText(m_strTitle.c_str());

	TD_IconVec lstIcon{ winfsutil::getFolderIcon() };
	__AssertReturn(m_wndBrowseTree.InitImglst(CSize(30, 30), lstIcon), TRUE);
	
	m_wndBrowseTree.SetRootDir(&m_mediaLibDir, m_bShowRoot);

	return TRUE;
}

void CChooseDirDlg::OnOK()
{
	CDirObject *pDirObject = (CDirObject*)m_wndBrowseTree.GetSelectedObject();
	if (NULL != pDirObject)
	{
		m_strRetDir = pDirObject->GetPath();
	}

	CDialog::OnOK();
}
