#pragma once

class CWholeTrackDlg : public TDialog<IDD_DLG_WholeTrack>
{
	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

public:
	CWholeTrackDlg(__view& view)
		: m_view(view)
	{
	}

private:
	__view& m_view;

	CObjectList m_wndList;

	XThread m_thread;

	bool m_bCancel = false;

	PairList<LPCCueFile, CMediaRes*> m_plCueFile;

private:
	BOOL OnInitDialog() override;

	void showCueInfo(cwstr strDir, CRCueFile cueFile, CMediaRes *pMediaRes);

	void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);

	void OnNMDbclickList1(NMHDR *pNMHDR, LRESULT *pResult);

	void OnBnClickedPlay();
	void OnBnClickedBtnExplore();
	void OnBnClickedViewDetail();

	void OnCancel() override;
};
