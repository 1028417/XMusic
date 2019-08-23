#pragma once

#include "PlayerMainWnd.h"

#pragma warning(disable: 4251)
#pragma warning(disable: 4275)

#ifdef _DEBUG
	#import "../../bin/PlaySpirit_dev.exe" no_namespace, named_guids
	#define LIBID_PlaySpiritPrj LIBID_PlaySpiritPrj_dev
	#define __PlaySpirit_clsid L"PlaySpiritPrj_dev.PlaySpirit"

#else
	#import "../../bin/PlaySpirit.exe" no_namespace, named_guids
	#define __PlaySpirit_clsid L"PlaySpiritPrj.PlaySpirit"
#endif

#include <atlbase.h>
#include <atlcom.h>

#include <functional>
using namespace std;

enum class E_PlaySpiritEvent
{
	PSE_RButtonUp
	, PSE_DblClick
	, PSE_ButtonClick
};
using CB_PlaySpiritEvent = function<void(E_PlaySpiritEvent, UINT uButton, short para)>;

class CPlaySpirit : public _PlaySpiritPtr
	, public IDispEventImpl<0, CPlaySpirit, &DIID___PlaySpirit, &LIBID_PlaySpiritPrj, 1, 0>
{
public:
	CPlaySpirit(CPlayerMainWnd& MainWnd)
		: m_MainWnd(MainWnd)
	{
	}

	BEGIN_SINK_MAP(CPlaySpirit)
		SINK_ENTRY_EX(0, DIID___PlaySpirit, 1, OnRButtonUp)
		SINK_ENTRY_EX(0, DIID___PlaySpirit, 2, OnDblClick)
		SINK_ENTRY_EX(0, DIID___PlaySpirit, 3, OnButton)
	END_SINK_MAP()

private:
	CPlayerMainWnd& m_MainWnd;

	CB_PlaySpiritEvent m_cb;

	bool m_bDocked = false;

	HWND m_hWndPlaySpirit = NULL;
	HWND m_hWndShadow = NULL;

private:
	BOOL _create(int iPosX, int iPosY);

	HRESULT __stdcall OnRButtonUp();
	HRESULT __stdcall OnDblClick();
	HRESULT __stdcall OnButton(ST_PlaySpiritButton eButton, short para);

public:
	void move(int x, int y)
	{
		(void)::SetWindowPos(m_hWndPlaySpirit, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	const CRect& rect() const
	{
		static CRect rcPos;
		(void)::GetWindowRect(m_hWndPlaySpirit, rcPos);
		return rcPos;
	}

	BOOL Create(const CB_PlaySpiritEvent& cb, int iPosX, int iPosY);
	
	void Destroy();

	void Dock(bool bDock = TRUE);

	bool IsDocked()
	{
		return m_bDocked;
	}

	void SetSkin(const wstring& strSkinPath);
};
