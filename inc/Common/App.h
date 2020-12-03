
#pragma once

#include "ModuleApp.h"

#define __waitCursor CWaitCursor WaitCursor

#define __appSync CMainApp::GetMainApp()->sync

class IView
{
public:
	IView() = default;

	virtual ~IView() = default;

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
	IController() = default;

public:
	virtual void start() {}
	
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

	virtual void stop() {}
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
	
protected:
	virtual BOOL InitInstance() override;
	
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;

private:
	void _run();

	static BOOL _RegGlobalHotkey(HWND hWnd, const tagHotkeyInfo &HotkeyInfo);
	
	bool _HandleHotkey(LPARAM lParam);
	bool _HandleHotkey(tagHotkeyInfo &HotkeyInfo);

	BOOL OnCommand(UINT uID);

	void _sync(const fn_void& fn);

public:
	static bool checkRuning();

	void Quit();
	
	BOOL DoEvent(bool bBlock, UINT& uMsg);
	BOOL DoEvent(bool bBlock)
	{
		UINT uMsg = 0;
		return DoEvent(bBlock, uMsg);
	}

	E_DoEventsResult DoEvents(bool bOnce = false);

	static UINT removeMsg(UINT uMsg);

	template <typename FN, typename = checkCBVoid_t<FN>>
	void sync(const FN& fn, bool bBlock=true)
	{
		DWORD dwThreadID = ::GetCurrentThreadId();
		if (bBlock)
		{
			if (dwThreadID == GetMainApp()->m_nThreadID)
			{
				fn();
				return;
			}
		}

		_sync([=]{
			fn();

			if (bBlock)
			{
				mtutil::apcWakeup(dwThreadID);
			}
		});

		if (bBlock)
		{
			::SleepEx(-1, TRUE);
		}
	}

	template <typename FN, typename = checkCBNotVoid_t<FN>, typename RET = decltype(declval<FN>()())>
	RET sync(const FN& fn, bool bBlock=true)
	{
		RET ret;
		sync([&]{
			ret = fn();
		}, bBlock);

		return ret;
	}

	template <typename FN, typename = checkCBVoid_t<FN>>
	void concurrence(const FN& fn)
	{
		bool bExit = false;
		std::thread thr([&]{
			fn();

			bExit = true;

			this->PostThreadMessage(WM_NULL, 0, 0);
		});

		while (!bExit)
		{
			if (!DoEvent(true))
			{
				break;
			}
		}

		thr.join();
	}

	template <typename FN, typename = checkCBNotVoid_t<FN>, typename RET = decltype(declval<FN>()())>
	RET concurrence(const FN& fn)
	{
		RET ret;
		concurrence([&]{
			ret = fn();
		});

		return ret;
	}
	
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

	static void foregroundWnd(HWND hWnd);

	static int msgBox(cwstr strMsg, cwstr strTitle, UINT nType, CWnd *pWnd = NULL);
	static void msgBox(cwstr strMsg, cwstr strTitle, CWnd *pWnd = NULL)
	{
		(void)msgBox(strMsg, strTitle, MB_OK, pWnd);
	}
	static void msgBox(cwstr strMsg, CWnd *pWnd = NULL)
	{
		msgBox(strMsg, L"Ã· æ", pWnd);
	}

	static bool confirmBox(cwstr strMsg, cwstr strTitle, CWnd *pWnd = NULL)
	{
		return IDYES == msgBox(strMsg, strTitle, MB_YESNO, pWnd);
	}
	static bool confirmBox(cwstr strMsg, CWnd *pWnd = NULL)
	{
		return confirmBox(strMsg, L"æØ∏Ê", pWnd);
	}
};
