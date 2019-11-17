#pragma once

class CDuplicateMediaDlg : public CDialogT<IDD_DLG_DuplicateMedia>
{
public:
	CDuplicateMediaDlg(__view& view, SArray<TD_MediaList>& arrDuplicateMedia)
		: m_view(view)
		, m_arrDuplicateMedia(arrDuplicateMedia)
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	__view& m_view;

	SArray<TD_MediaList>& m_arrDuplicateMedia;
	
	SArray<pair<UINT, UINT>> m_arrDuplicateMediaInfo;

	CObjectList m_wndList;

	vector<bool> m_vecRowFlag;

public:
	BOOL OnInitDialog() override;

	void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

	void OnBnClickedRemove();

	void OnBnClickedPlay();
};
