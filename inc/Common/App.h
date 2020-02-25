
#pragma once

#include "ModuleApp.h"

using CB_Sync = fn_void;

#define __appSync CMainApp::sync
#define __appAsync CMainApp::async

class IView
{
public:
	IView() {}

	virtual ~IView() {}

public:
	virtual CMainWnd* show() = 0;

	virtual bool handleCommand(UINT uID)
	{
		return false;
	}

	virtual bool handleHotkey(const tagHotkeyInfo& HotkeyInfo)
	{
		return false;
	}

	virtual void close() {}
};

class IController
{
public:
	IController(){}

public:
	virtual bool init()
	{
		return true;
	}

	virtual bool start()
	{
		return true;
	}

	virtual bool handleCommand(UINT uID)
	{
		return false;
	}

	virtual bool handleHotkey(const tagHotkeyInfo& HotkeyInfo)
	{
		return false;
	}

	virtual LRESULT handleModuleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		return 0;
	}

	virtual void stop()
	{
	}
};

enum class E_DoEventsResult
{
	DER_None
	, DER_OK
	, DER_Quit
};

class __CommonExt CMainApp : public CModuleApp, public IController
{
public:
	static CMainApp* GetMainApp()
	{
		return (CMainApp*)AfxGetApp();
	}

	CMainApp()
	{
		afxCurrentInstanceHandle = _AtlBaseModule.GetModuleInstance();
		afxCurrentResourceHandle = _AtlBaseModule.GetResourceInstance();
	}

	virtual IView& getView() = 0;

	virtual IController& getController()
	{
		return *this;
	}
	
private:
	typedef vector<CModuleApp*> ModuleVector;
	ModuleVector m_vctModules;

protected:
	virtual BOOL InitInstance() override;
	
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

private:
	void _run();
	void _run(class CMainWnd& MainWnd);

	static BOOL _RegGlobalHotkey(HWND hWnd, const tagHotkeyInfo &HotkeyInfo);
	
	bool _HandleHotkey(LPARAM lParam);
	bool _HandleHotkey(tagHotkeyInfo &HotkeyInfo);

	BOOL OnCommand(UINT uID);

	static void _sync(const CB_Sync& cb);

public:
	void Quit();

	static BOOL AddModule(CModuleApp& Module);

	static bool removeMsg(UINT uMsg);

	static void sync(const CB_Sync& cb);

	static void async(UINT uDelayTime, const CB_Sync& cb);
	static void async(const CB_Sync& cb)
	{
		async(0, cb);
	}

	void thread(cfn_void cb);

	E_DoEventsResult DoEvents(bool bOnce=false);

	static int showMsg(const wstring& strMsg, const wstring& strTitle, UINT nType, CWnd *pWnd);
	
	static void showMsg(const wstring& strMsg, const wstring& strTitle, CWnd *pWnd = NULL)
	{
		(void)showMsg(strMsg, strTitle, MB_OK, pWnd);
	}

	static void showMsg(const wstring& strMsg, CWnd *pWnd = NULL)
	{
		showMsg(strMsg, L"Ã· æ", pWnd);
	}

	static void showMsg(const wstring& strMsg, class CPage& wndPage);

	static bool showConfirmMsg(const wstring& strMsg, const wstring& strTitle, CWnd *pWnd = NULL)
	{
		return IDYES == showMsg(strMsg, strTitle, MB_YESNO, pWnd);
	}

	static bool showConfirmMsg(const wstring& strMsg, CWnd *pWnd = NULL)
	{
		return showConfirmMsg(strMsg, L"æØ∏Ê", pWnd);
	}

	static bool showConfirmMsg(const wstring& strMsg, class CPage& wndPage);
		
	static bool getKeyState(UINT uKey)
	{
		return ::GetKeyState(uKey) < 0;
	}

	static BOOL RegHotkey(const tagHotkeyInfo &HotkeyInfo);

	static BOOL RegisterInterface(UINT uIndex, LPVOID lpInterface);
	static LPVOID GetInterface(UINT uIndex);

	static LRESULT SendModuleMessage(UINT uMsg, WPARAM wParam=0, LPARAM lParam=0);

	static LRESULT SendModuleMessage(UINT uMsg, LPVOID pPara)
	{
		return CMainApp::SendModuleMessage(uMsg, (WPARAM)pPara);
	}

	template <typename _T1, typename _T2>
	static LRESULT SendModuleMessage(UINT uMsg, _T1 para1, _T2 para2)
	{
		return CMainApp::SendModuleMessage(uMsg, (WPARAM)para1, (LPARAM)para2);
	}

	static void BroadcastModuleMessage(UINT uMsg, WPARAM wParam=0, LPARAM lParam=0);

	static void BroadcastModuleMessage(UINT uMsg, LPVOID pPara)
	{
		BroadcastModuleMessage(uMsg, (WPARAM)pPara);
	}

	template <typename _T1, typename _T2>
	static void BroadcastModuleMessage(UINT uMsg, _T1 para1, _T2 para2)
	{
		BroadcastModuleMessage(uMsg, (WPARAM)para1, (LPARAM)para2);
	}
};
