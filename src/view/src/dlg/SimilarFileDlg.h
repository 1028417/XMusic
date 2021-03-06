#pragma once

class CSimilarFileDlg : public TDialog<IDD_DLG_SimilarFile>
{
public:
	CSimilarFileDlg(__view& view, TD_SimilarFile& arrSimilarFile)
		: m_view(view)
		, m_arrSimilarFile(arrSimilarFile)
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	__view& m_view;

	CObjectList m_wndList;

	CSliderCtrl m_wndSlider;

	SArray<UINT> m_arrPercent;

	TD_SimilarFile& m_arrSimilarFile;
	
	SArray<pair<UINT, UINT>> m_arrSimilarFileInfo;

	vector<bool> m_vecRowFlag;

	UINT m_uPos = 0;

private:
	void _genPercent();

	void Refresh(UINT uPos);

	void OnClose(bool bCancel) override;

	void _removeMediaRes(const TD_MediaResList& paMediaRes);

public:
	BOOL OnInitDialog() override;

	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);

	void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);

	void OnNMDblclkList1(NMHDR *pNMHDR, LRESULT *pResult);

	void OnBnClickedPlay();

	void OnBnClickedExplore();

	void OnBnClickedAddin();

	void OnBnClickedRemove();
};
