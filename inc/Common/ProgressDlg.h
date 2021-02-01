#pragma once

#include "TDialog.h"

using FN_Work = fn_void_t<class CProgressDlg&>;

class __CommonExt CProgressDlg : public TDialog<>, public CThreadGroup
{
public:
	CProgressDlg(const FN_Work& fnWork)
		: m_fnWork(fnWork)
	{
	}

	CProgressDlg(UINT uMaxProgress, const FN_Work& fnWork)
		: m_fnWork(fnWork)
		, m_uMaxProgress(uMaxProgress)
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

	DECLARE_MESSAGE_MAP()

private:
	FN_Work m_fnWork;

	UINT m_uMaxProgress = 0;
	UINT m_uProgress = 0;

	mutex m_mutex;
	wstring m_strStatusText;

	wstring m_strTitle;

	CProgressCtrl m_wndProgressCtrl;

	bool m_bFinished = false;

public:
	virtual INT_PTR DoModal(cwstr strTitle, CWnd *pWndParent=NULL);
	
	int msgBox(cwstr strText, cwstr strTitle, UINT uType = 0)
	{
		return __appSync([&]{
			return TDialog::msgBox(strText.c_str(), strTitle.c_str(), uType);
		});
	}

	int msgBox(cwstr strText, UINT uType = 0)
	{
		return msgBox(strText, m_strTitle, uType);
	}

	void SetStatusText(cwstr strStatusText);
	void SetStatusText(cwstr strStatusText, UINT uOffsetProgress);

	void SetProgress(UINT uProgress);
	void SetProgress(UINT uProgress, UINT uMaxProgress);

	void ForwardProgress(UINT uOffSet=1);

	void Close();

private:
	virtual BOOL OnInitDialog();
	
	LRESULT OnUpateStatus(WPARAM wParam, LPARAM lParam);

	virtual void OnCancel();

private:
	void _updateProgress();
};
