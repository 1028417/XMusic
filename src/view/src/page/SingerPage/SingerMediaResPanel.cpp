
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

UINT CSingerMediaResPanel::_onShowDir()
{
	UINT uItem = 0;
	if (currDir() == rootDir())
	{
		auto pSinger = m_wndAlbumPage.GetSinger();
		if (pSinger)
		{
			for (cauto strDir : pSinger->attachDir())
			{
				auto pMediaDir = __medialib.subDir(strDir);
				if (pMediaDir)
				{
					m_wndList.InsertObject(*pMediaDir, uItem++);
				}
			}
		}
	}

	return uItem;
}

void CSingerMediaResPanel::OnMenuCommand(UINT uID, UINT uVkKey)
{
	if (ID_Attach == uID)
	{
		auto pSinger = m_wndAlbumPage.GetSinger();
		__Ensure(pSinger);

		CMediaDir *pDir = m_view.showChooseDirDlg(L"ѡ�񸽼�Ŀ¼", false);
		__Ensure(pDir);

		cauto strDir = pDir->GetPath();
		__Ensure(m_view.getSingerMgr().AttachDir(*pSinger, strDir));
				
		Refresh();
	}
	else if (ID_Detach == uID)
	{
		auto pSinger = m_wndAlbumPage.GetSinger();
		__Ensure(pSinger);

		auto pMediaDir = (CMediaDir*)m_wndList.GetSelObject();
		if (pMediaDir)
		{
			__Ensure(m_view.getSingerMgr().DetachDir(*pSinger, pMediaDir->GetPath()));
			
			Refresh();
		}
	}
	else
	{
		CMediaResPanel::OnMenuCommand(uID, uVkKey);
	}
}
