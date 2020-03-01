
#include "StdAfx.h"

#include "SingerMediaResPanel.h"

BEGIN_MESSAGE_MAP(CSingerMediaResPanel, CMediaResPanel)
ON_NOTIFY(NM_SETFOCUS, IDC_LIST1, &CSingerMediaResPanel::OnNMSetFocusList)
END_MESSAGE_MAP()

void CSingerMediaResPanel::_OnInitDialog()
{
	auto pParent = this->GetParent();
	m_wndList.SetFont(pParent->GetFont());
}

void CSingerMediaResPanel::OnNMSetFocusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	m_wndAlbumPage.m_wndAlbumList.SelectItem(0);
}

void CSingerMediaResPanel::UpdateTitle(const wstring& strTitle)
{
	__super::UpdateTitle(strTitle);

	m_wndAlbumPage.UpdateTitle();
}

void CSingerMediaResPanel::HittestMediaRes(CMediaRes& MediaRes)
{
	m_wndAlbumPage.m_wndAlbumList.SelectFirstItem();
	__super::HittestMediaRes(MediaRes);
	this->SetFocus();
}