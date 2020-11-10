
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

void CSingerMediaResPanel::UpdateTitle(cwstr strTitle)
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

void CSingerMediaResPanel::_showDir()
{
	CMediaResPanel::_showDir();

	for (auto itr = m_lstAttachDir.rbegin(); itr != m_lstAttachDir.rend(); ++itr)
	{
		m_wndList.InsertObject(*itr, 0);
	}
}

void CSingerMediaResPanel::OnMenuCommand(UINT uID, UINT uVkKey)
{
	auto pSinger = m_wndAlbumPage.GetSinger();
	__Ensure(pSinger);

	if (ID_Attach == uID)
	{
		CMediaDir *pDir = m_view.showChooseDirDlg(L"ѡ�񸽼�Ŀ¼", false);
		__Ensure(pDir);

		cauto strDir = pDir->GetPath();
		__Ensure(m_view.getSingerMgr().AttachDir(*pSinger, strDir));

		m_lstAttachDir.emplace_back(strDir);
	}
	else if (ID_Detach == uID)
	{
		CMediaDir *pDir = NULL;

		for (auto itr = m_lstAttachDir.begin(); itr != m_lstAttachDir.end(); ++itr)
		{
			if (&*itr == pDir)
			{
				__Ensure(m_view.getSingerMgr().DetachDir(*pSinger, pDir->GetPath()));
				m_lstAttachDir.erase(itr);
				break;
			}
		}

	}
	else
	{
		CMediaResPanel::OnMenuCommand(uID, uVkKey);
	}
}
