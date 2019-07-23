#pragma once

class CMediaSetDlg : public CDialogT<IDD_DLG_MEDIASET, true>
{
public:
	CMediaSetDlg(__view& view, CMediaSet& MediaSet, TD_MediaSetList& lstRetMediaSets, const wstring& strTitle);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

protected:
	__view& m_view;

	CMediaSet& m_MediaSet;

	CObjectCheckTree m_wndTree;

private:
	TD_MediaSetList& m_lstSelMediaSets;

	wstring m_strTitle;

protected:
	virtual BOOL OnInitDialog();

	virtual void OnOK();
};
