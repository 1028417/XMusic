#pragma once

class CTrackDetailDlg : public CDialogT<IDD_DLG_TrackDetail>
{
public:
	CTrackDetailDlg(__view& view, CRCueFile cueFile, CMediaRes *pMediaRes);

	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX);

private:
	__view& m_view;

	CRCueFile m_cueFile;
	CMediaRes *m_pMediaRes = NULL;

	CObjectList m_wndList;
	
private:
	BOOL OnInitDialog() override;

	void OnNMDbclickList1(NMHDR *pNMHDR, LRESULT *pResult);
	
	void OnBnClickedPlay();
};
