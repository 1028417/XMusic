
#pragma once

//#include <commdlg.h>

class __UtilExt CFolderDlg
{
public:
	CFolderDlg() {}

private:
	wstring m_strInitialDir;

	wstring m_strTitle;	
	wstring m_strTip;
	
	wstring m_strOKButton;
	wstring m_strCancelButton;

	int m_nWidth = 0;
	int m_nHeight = 0;

	HWND m_hWnd = NULL;
	HWND m_hWndOkButton = NULL;
	HWND m_hWndCancelButton = NULL;

	RECT m_rcPreClient{0,0,0,0};

public:
	wstring Show(HWND hWndOwner, LPCWSTR lpszInitialDir=NULL, LPCWSTR lpszTitle=NULL, LPCWSTR lpszTip=NULL
		, LPCWSTR lpszOKButton=NULL, LPCWSTR lpszCancelButton=NULL, UINT uWidth=0, UINT uHeight=0);

private:
	static int __stdcall BrowseFolderCallBack(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpParam);

	void _handleCallBack(HWND hWnd, UINT uMsg, LPARAM lParam);
	void _initDlg();
	void _relayout();
};


struct tagFileDlgOpt
{
	wstring strTitle;
	
	wstring strInitialDir;
	
	wstring strFileName;
	
	wstring strFilter;
		
	bool bMustExist = false;

	HWND hWndOwner = NULL;
};

class __UtilExt CFileDlg
{
public:
	CFileDlg() {}

	CFileDlg(const tagFileDlgOpt& opt)
	{
		_setOpt(opt);
	}

private:
	OPENFILENAME m_ofn;

	TCHAR m_lpstrFilter[512];

	wstring m_strInitialDir;

	wstring m_strTitle;

	TCHAR m_lpstrFileName[40960];

protected:
	void _setOpt(const tagFileDlgOpt& opt);

private:
	bool _show(bool bSaveFile);

	wstring _getMultSel(list<wstring>& lstFiles);

public:
	wstring ShowSave();

	wstring ShowOpenSingle();
	wstring ShowOpenMulti(list<wstring>& lstFiles);

	UINT GetSelFilterIndex() const;
};
