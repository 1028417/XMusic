
#pragma once

#include "DragDrop.h"

#include "App.h"

#include "Guard.h"

class CModuleApp;

class CMainWnd;

class __CommonExt CPage: public CPropertyPage
{
	friend class CDockView;

	DECLARE_MESSAGE_MAP()

	virtual void DoDataExchange(CDataExchange* pDX);
	
public:
	CPage(CResModule& resModule, UINT uIDDlgRes, const CString& cstrTitle=L"", bool bAutoActive=false);

	virtual ~CPage() {}

private:
	CResModule& m_resModule;

	CCompatableFont m_font;

	CString m_cstrTitle;

	bool m_bAutoActive = false;

	map<HWND, map<UINT, UINT>> m_mapMenuHotKeys;

	set<HWND> m_setDragableCtrls;

	bool m_bDragable = false;

public:
	void SetTitle(const CString& cstrTitle, int iImage = -1);

	const CString& GetTitle() const
	{
		return m_cstrTitle;
	}

	void RegMenuHotkey(CWnd& wndCtrl, UINT uVkKeyCode, UINT uCmd = 0);

	int msgBox(const wstring& strMsg, const wstring& strTitle, UINT nType)
	{
		return CMainApp::msgBox(strMsg, strTitle, nType, this);
	}
	void msgBox(const wstring& strMsg, const wstring& strTitle)
	{
		CMainApp::msgBox(strMsg, strTitle, this);
	}
	void msgBox(const wstring& strMsg)
	{
		CMainApp::msgBox(strMsg, this);
	}
	bool msgBox(const wstring& strMsg, bool bWarning)
	{
		return CMainApp::msgBox(strMsg, bWarning, this);
	}

	bool confirmBox(const wstring& strMsg, const wstring& strTitle)
	{
		return CMainApp::confirmBox(strMsg, strTitle, this);
	}
	bool confirmBox(const wstring& strMsg)
	{
		return CMainApp::confirmBox(strMsg, this);
	}

private:
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	BOOL OnSetActive() override;
	BOOL OnKillActive() override;

protected:
	virtual void PreSubclassWindow() override
	{
		__super::PreSubclassWindow();

		m_font.setFont(*this);
	}

	virtual void OnActive(BOOL bActive) {}

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	virtual void OnMenuCommand(UINT uID, UINT uVkKey = 0) {}

	virtual BOOL GetCtrlDragData(HWND hwndCtrl, const CPoint& point, LPVOID& pDragData)
	{
		return FALSE;
	}
	
	BOOL RegDragableCtrl(CWnd& wndCtrl)
	{
		__AssertReturn(wndCtrl.m_hWnd, FALSE);

		m_setDragableCtrls.insert(wndCtrl.m_hWnd);
		
		return TRUE;
	}
};
