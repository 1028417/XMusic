
#include "StdAfx.h"

#include "SingerMediaResPanel.h"

void CSingerMediaResPanel::_OnInitDialog()
{
	auto pParent = this->GetParent();
	m_wndList.SetFont(pParent->GetFont());
}

void CSingerMediaResPanel::UpdateTitle(const wstring& strTitle)
{
	//__super::UpdateTitle(strTitle);

	m_wndAlbumPage.UpdateTitle(strTitle + L" ");
}
