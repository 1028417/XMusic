#pragma once

// CExportOptionDlg ¶Ô»°¿ò

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

	virtual BOOL OnInitDialog() override;
	
	void OnBnClickedActualMode();
	
	void OnBnClickedExportXmsc();

	void OnBnClickedCompareFileSize();

	void OnBnClickedOK();
};
