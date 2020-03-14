#pragma once

#include "TDialog.h"

using FN_Work = fn_void_t<class CProgressDlg&>;

class __CommonExt CProgressDlg : public TDialog<>, public CThreadGroup
{
public:
	CProgressDlg(const FN_Work& fnWork, UINT uMaxProgress=0)
		: m_fnWork(fnWork)
		, m_uMaxProgress(uMaxProgress)
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	FN_Work m_fnWork;

	wstring m_strTitle;

	UINT m_uMaxProgress = 0;

	UINT m_uProgress = 0;

	CCASLock m_csLock;
	mutex m_mtx;
	
	bool m_bFinished = false;

	CString m_cstrStatusText;

	CProgressCtrl m_wndProgressCtrl;

public:
	virtual INT_PTR DoModal(const wstring& strTitle, CWnd *pWndParent=NULL);
	
	int msgBox(const wstring& strText, const wstring& strTitle, UINT uType = 0)
	{
		int nRet = 0;
		__appSync([&]() {
			nRet = TDialog::msgBox(strText.c_str(), strTitle.c_str(), uType);
		});
		return nRet;
	}

	int msgBox(const wstring& strText, UINT uType = 0)
	{
		return msgBox(strText, m_strTitle, uType);
	}

	void SetStatusText(const CString& cstrStatusText);
	void SetStatusText(const CString& cstrStatusText, UINT uOffsetProgress);

	void SetProgress(UINT uProgress);
	void SetProgress(UINT uProgress, UINT uMaxProgress);

	void ForwardProgress(UINT uOffSet=1);

	void Close();

private:
	virtual BOOL OnInitDialog();

	LRESULT OnSetStatusText(WPARAM wParam, LPARAM lParam);

	LRESULT OnSetProgress(WPARAM wParam, LPARAM lParam);

	virtual void OnCancel();

private:
	void _updateProgress();
	void _endProgress();
};
