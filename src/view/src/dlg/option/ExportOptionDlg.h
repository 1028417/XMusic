#pragma once

// CExportOptionDlg ¶Ô»°¿ò

class CExportOptionDlg : public TDialog<IDD_DLG_ExportOption>
{
public:
	CExportOptionDlg(bool bForceActualMode, tagExportOption& ExportOption)
		: m_bForceActualMode(bForceActualMode)
		, m_ExportOption(ExportOption)
	{
	}

	DECLARE_MESSAGE_MAP()
	
private:
	bool m_bForceActualMode;

	tagExportOption& m_ExportOption;

	virtual BOOL OnInitDialog() override;
	
	void OnBnClickedExportXmsc();

	void OnBnClickedCompareFileSize();

	void OnBnClickedOK();
};
