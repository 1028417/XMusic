#pragma once

class CChooseDirDlg : public CDialogT<IDD_DLG_ChooseDir, true>
{
public:
	CChooseDirDlg(const wstring& strTitle, CMediaDir& RootDir, bool bShowRoot, wstring& strRetDir);

	void DoDataExchange(CDataExchange* pDX);

private:
	wstring m_strTitle;

	bool m_bShowRoot = false;

	CMediaDir& m_RootDir;

	wstring& m_strRetDir;

	CDirTree m_wndTree;

private:
	BOOL OnInitDialog();

	void OnOK();
};
