#pragma once

#define WM_SetProgress WM_USER+1
#define WM_SetStatusText WM_USER+2
#define WM_EndProgress WM_USER+3


using FN_Work = fn_void_t<class CProgressDlg&>;

class __CommonExt CProgressDlg : public CDialog, public CThreadGroup
{
public:
	CProgressDlg(const FN_Work& fnWork, UINT uMaxProgress=0)
		: m_fnWork(fnWork)
		, m_uMaxProgress(uMaxProgress)
	{
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV Ö§³Ö

	DECLARE_MESSAGE_MAP()

private:
	FN_Work m_fnWork;

	wstring m_strTitle;

	UINT m_uMaxProgress = 0;

	UINT m_uProgress = 0;

	CCASLock m_csLock;
	
	bool m_bFinished = false;

	CString m_cstrStatusText;

	CProgressCtrl m_wndProgressCtrl;

public:
	virtual INT_PTR DoModal(const wstring& strTitle, CWnd *pWndParent=NULL);
	
	int showMsgBox(const wstring& strText, const wstring& strTitle, UINT uType = 0)
	{
		return MessageBoxW(strText.c_str(), strTitle.c_str(), uType);
	}

	int showMsgBox(const wstring& strText, UINT uType = 0)
	{
		return showMsgBox(strText, m_strTitle, uType);
	}

	void SetStatusText(const CString& cstrStatusText, UINT uOffsetProgress=0);

	void SetProgress(UINT uProgress);
	void SetProgress(UINT uProgress, UINT uMaxProgress);

	void ForwardProgress(UINT uOffSet=1);

	void Close();

private:
	virtual BOOL OnInitDialog();

	LRESULT OnSetStatusText(WPARAM wParam, LPARAM lParam);

	LRESULT OnSetProgress(WPARAM wParam, LPARAM lParam);

	LRESULT OnEndProgress(WPARAM wParam, LPARAM lParam);

	virtual void OnCancel();

private:
	void _updateProgress();
	void _endProgress();
};
