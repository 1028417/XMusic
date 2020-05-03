#pragma once

class CChooseDirDlg : public TDialog<IDD_DLG_ChooseDir, true>
{
public:
	CChooseDirDlg(cwstr strTitle, CMediaDir& RootDir, bool bShowRoot);

	void DoDataExchange(CDataExchange* pDX);

private:
	wstring m_strTitle;

	bool m_bShowRoot = false;

	CMediaDir& m_RootDir;

	CMediaDir* m_pSelDir = NULL;

	CDirTree m_wndTree;

private:
	BOOL OnInitDialog() override;

	void OnOK() override
	{
		m_pSelDir = (CMediaDir*)m_wndTree.GetSelectedObject();
		CDialog::OnOK();
	}

public:
	CMediaDir* show()
	{
		m_pSelDir = NULL;
		(void)CDialog::DoModal();
		return m_pSelDir;
	}
};
