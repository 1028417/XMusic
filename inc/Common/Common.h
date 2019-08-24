
#pragma once

#include <SDKDDKVer.h>

#include <afxcmn.h>			// MFC 对 Windows 公共控件的支持

#include "util.h"

#ifdef __CommonPrj
#define __CommonExt __dllexport
#else
#define __CommonExt __dllimport
#endif

#define DECLARE_SINGLETON(_CLASS) \
	public: \
		static _CLASS& inst() \
		{ \
			static _CLASS inst; \
			return inst; \
		} \
\
	private: \
		_CLASS();

#define __Assert(x) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		return; \
	}

#define __AssertReturn(x, y) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		return y; \
	}

#define __AssertContinue(x) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		continue; \
	}

#define __AssertBreak(x) \
	if (!(x)) \
	{ \
		ASSERT(FALSE); \
		break; \
	}

enum class E_TrackMouseEvent
{
	LME_MouseHover
	, LME_MouseLeave
};

using CB_TrackMouseEvent = function<void(E_TrackMouseEvent, const CPoint&)>;

#include "App.h"

#include "MainWnd.h"

#include "DockView.h"

#include "Page.h"

#include "Guard.h"

#include "TreeCtrl.h"

#include "ListCtrl.h"

#include "PathCtrl.h"

#include "DragDrop.h"

#include "ProgressDlg.h"

#include "DialogT.h"

class __CommonExt CFolderDlgEx : public CFolderDlg
{
public:
	CFolderDlgEx(){}

	wstring Show(HWND hWndOwner, LPCWSTR lpszInitialDir = NULL, LPCWSTR lpszTitle = NULL, LPCWSTR lpszTip = NULL
		, LPCWSTR lpszOKButton = NULL, LPCWSTR lpszCancelButton = NULL, UINT uWidth = 0, UINT uHeight = 0)
	{
		if (NULL == hWndOwner)
		{
			hWndOwner = CMainApp::GetMainApp()->GetMainWnd()->GetSafeHwnd();
		}

		if (0 == uWidth)
		{
			uWidth = CMainApp::getWorkArea(true).Width() * 38/100;
		}
		if (0 == uHeight)
		{
			uHeight = CMainApp::getWorkArea(true).Height() * 83/100;
		}

		wstring strRet;
		CMainApp::GetMainApp()->thread([&]() {
			strRet = CFolderDlg::Show(hWndOwner, lpszInitialDir, lpszTitle, lpszTip
				, lpszOKButton, lpszCancelButton, uWidth, uHeight);
		});
		return strRet;
	}

	wstring Show(const wstring& strTitle, const wstring& strTip = L"", HWND hWndOwner = NULL)
	{
		return Show(hWndOwner, NULL, strTitle.c_str(), strTip.c_str());
	}
};

class __CommonExt CFileDlgEx : public CFileDlg
{
public:
	CFileDlgEx(tagFileDlgOpt& opt)
	{
		if (NULL == opt.hWndOwner)
		{
			opt.hWndOwner = CMainApp::GetMainApp()->GetMainWnd()->GetSafeHwnd();
		}
		_setOpt(opt);
	}

	wstring ShowSave()
	{
		wstring strRet;
		CMainApp::GetMainApp()->thread([&]() {
			strRet = CFileDlg::ShowSave();
		});
		return strRet;
	}

	wstring ShowOpenSingle()
	{
		wstring strRet;
		CMainApp::GetMainApp()->thread([&]() {
			strRet = CFileDlg::ShowOpenSingle();
		});
		return strRet;
	}

	wstring ShowOpenMulti(list<wstring>& lstFiles)
	{
		wstring strRet;
		CMainApp::GetMainApp()->thread([&]() {
			strRet = CFileDlg::ShowOpenMulti(lstFiles);
		});
		return strRet;
	}
};
