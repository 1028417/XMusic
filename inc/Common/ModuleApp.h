
#pragma once

class __CommonExt CResModule
{
public:
	CResModule(HINSTANCE hInstance=NULL)
	{
		_hInstance = hInstance;
	}

	CResModule(const string& strDllName)
	{
		_hInstance = GetModuleHandleA((strDllName + ".dll").c_str());
	}

private:
	HINSTANCE _hInstance = NULL;

	virtual HINSTANCE GetHInstance()
	{
		return _hInstance;
	}

public:
	void ActivateResource();

	HICON loadIcon(UINT uID);

	HBITMAP loadBitmap(UINT uID);

	HMENU loadMenu(UINT uID);

	LPCDLGTEMPLATE loadDialog(UINT uID);
};

// Hotkey
enum class E_HotkeyFlag
{
	HKF_Null = 0

	, HKF_Alt = MOD_ALT
	, HKF_Control = MOD_CONTROL
	, HKF_Shift = MOD_SHIFT

	, HKF_AltControl = MOD_ALT | MOD_CONTROL
	, HKF_AltShift = MOD_ALT | MOD_SHIFT
	, HKF_ControlShift = MOD_CONTROL | MOD_SHIFT
	, HKF_AltControlShift = MOD_ALT | MOD_CONTROL | MOD_SHIFT
};

struct tagHotkeyInfo
{
	tagHotkeyInfo(UINT t_uKey, E_HotkeyFlag eHotkeyFlag, int t_uIDMenuItem, bool t_bGlobal = false)
		: uKey(t_uKey)
		, eFlag(eHotkeyFlag)
		, lParam(MAKELPARAM(eHotkeyFlag, uKey))
		, uIDMenuItem((UINT)t_uIDMenuItem)
		, bGlobal(t_bGlobal)
	{
		bHandling = false;
	}

	tagHotkeyInfo(UINT t_uKey, E_HotkeyFlag eHotkeyFlag, bool t_bGlobal = false)
		: uKey(t_uKey)
		, eFlag(eHotkeyFlag)
		, lParam(MAKELPARAM(eHotkeyFlag, uKey))
		, uIDMenuItem(0)
		, bGlobal(t_bGlobal)
	{
		bHandling = false;
	}

	UINT uKey;
	E_HotkeyFlag eFlag;
	LPARAM lParam;
	
	UINT uIDMenuItem;
	
	bool bGlobal;

	bool bHandling;

	bool operator ==(const tagHotkeyInfo &HotkeyInfo)
	{
		return lParam == HotkeyInfo.lParam;
	}
};

class CMainWnd;

// CModuleApp
class __CommonExt CModuleApp: public CWinApp, public CResModule
{
friend class CMainApp;

public:
	CModuleApp() {}

	virtual ~CModuleApp() {}

	HINSTANCE GetHInstance() override
	{
		return m_hInstance;
	}
	
protected:
	virtual BOOL InitInstance() override;

protected:
	virtual BOOL OnReady(CMainWnd& MainWnd) { return TRUE; }

	virtual BOOL HandleCommand(UINT uID) { return FALSE; }

	virtual BOOL HandleHotkey(const tagHotkeyInfo& HotkeyInfo) { return FALSE; }

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { return 0; }

	virtual BOOL OnQuit() { return TRUE; }
};
