#pragma once

// CExportOptionDlg �Ի���

class CExportOptionDlg : public CDialogT<IDD_DLG_ExportOption>
{
public:
	CExportOptionDlg(tagExportOption& ExportOption)
		: m_ExportOption(ExportOption)
	{
	}

	DECLARE_MESSAGE_MAP()
	
private:
	tagExportOption& m_ExportOption;

	virtual BOOL OnInitDialog();
	
	void OnBnClickedActualMode();

	void OnBnClickedCompareFileSize();

	void OnBnClickedOK();
};
