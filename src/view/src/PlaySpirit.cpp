
#include "stdafx.h"

#include "PlaySpirit.h"

static CComModule g_ComModule;

BOOL CPlaySpirit::Create()
{
	//(void)CoInitialize(NULL);

	HRESULT hResult = __super::CreateInstance(__PlaySpirit_clsid);
	if (S_OK != hResult)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CPlaySpirit::Show(const CB_PlaySpiritEvent& cb, HWND hWndOwner, int nPosX, int nPosY)
{
	//必须
	HRESULT hResult = this->DispEventAdvise(*this);
	if (S_OK != hResult)
	{
		if (*this)
		{
			this->Destroy();
		}
		return FALSE;
	}

	m_cb = cb;

	m_hWndPlaySpirit = (HWND)(LONG_PTR)(*this)->Show(-1000, -1000);
	m_hWndShadow = (HWND)(*this)->GetShadowHandle();

	(void)::SetWindowPos(m_hWndPlaySpirit, NULL, nPosX, nPosY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	m_hWndOwner = hWndOwner;
	Dock();

	return TRUE;
}

void CPlaySpirit::Destroy()
{
	(void)::SetWindowLong(m_hWndPlaySpirit, GWLP_HWNDPARENT, 0);

	(*this)->Destroy();

	(void)this->DispEventUnadvise(*this);

	((_com_ptr_t*)this)->Release();

	m_hWndPlaySpirit = NULL;
}

void CPlaySpirit::Dock(bool bDock)
{
	m_bDocked = bDock;

	for (auto hwnd : { m_hWndShadow, m_hWndPlaySpirit })
	{
		if (m_bDocked)
		{
			(void)::SetWindowLong(hwnd, GWLP_HWNDPARENT, (LONG)m_hWndOwner);
			(void)::SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
		else
		{
			(void)::SetWindowLong(hwnd, GWLP_HWNDPARENT, (LONG)::GetDesktopWindow());
			(void)::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
	}
}

void CPlaySpirit::SetSkin(cwstr strSkinPath)
{
	cauto rcPos = rect();

	(*this)->LoadSkin(bstr_t(strSkinPath.c_str()));

	move(rcPos.left, rcPos.top);

	Dock(m_bDocked);
}

HRESULT __stdcall CPlaySpirit::OnRButtonUp()
{
	m_cb(E_PlaySpiritEvent::PSE_RButtonUp, 0, 0);
	
	return S_OK;
}

HRESULT __stdcall CPlaySpirit::OnDblClick()
{
	m_cb(E_PlaySpiritEvent::PSE_DblClick, 0, 0);

	return S_OK;
}

HRESULT __stdcall CPlaySpirit::OnButton(ST_PlaySpiritButton eButton, short para)
{
	m_cb(E_PlaySpiritEvent::PSE_ButtonClick, (UINT)eButton, para);

	return S_OK;
}
