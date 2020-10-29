
#include "stdafx.h"

#include "MediaSetDlg.h"

CMediaSetDlg::CMediaSetDlg(__view& view, CMediaSet& MediaSet, TD_MediaSetList& lstRetMediaSets, cwstr strTitle)
	: m_view(view)
	, m_MediaSet(MediaSet)
	, m_lstSelMediaSets(lstRetMediaSets)
	, m_strTitle(strTitle)
{
}

void CMediaSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE, m_wndTree);
}

// CMediaSetDlg 消息处理程序

BOOL CMediaSetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	this->SetWindowText(m_strTitle.c_str());

	__EnsureReturn(m_wndTree.InitCtrl(), FALSE);
	
	(void)m_wndTree.SetRootObject(m_MediaSet, false);
	m_wndTree.SetCheckState(m_MediaSet, true);

	return TRUE;
}

void CMediaSetDlg::OnOK()
{
	TD_TreeObjectList lstCheckedObjects;
	m_wndTree.GetAllObjects(lstCheckedObjects, CS_Checked);
	if (!lstCheckedObjects)
	{
		msgBox(L"请" + m_strTitle);
		return;
	}

	TD_MediaSetList lstMediaSets(lstCheckedObjects);
	m_lstSelMediaSets = lstMediaSets.filter([&](CMediaSet& MediaSet){
		return E_MediaSetType::MST_Playlist == MediaSet.m_eType
			|| E_MediaSetType::MST_Album == MediaSet.m_eType;
	});
	
	CDialog::OnOK();
}
