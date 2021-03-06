#pragma once

class CMediaSetDlg : public TDialog<IDD_DLG_MEDIASET, true>
{
public:
	CMediaSetDlg(__view& view, CMediaSet& MediaSet, TD_MediaSetList& lstRetMediaSets, cwstr strTitle);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

protected:
	__view& m_view;

	CMediaSet& m_MediaSet;

	CObjectCheckTree m_wndTree;

private:
	TD_MediaSetList& m_lstSelMediaSets;

	wstring m_strTitle;

protected:
	virtual BOOL OnInitDialog() override;

	virtual void OnOK() override;
};
