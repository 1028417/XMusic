
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

void CSingerMediaResPanel::SetSinger(const CSinger& singer)
{
	SetDir(singer.dir());

	m_lstAttachDir.clear();
	for (auto& strDir : singer.attachDir())
	{
		m_lstAttachDir.emplace_back(strDir);
	}
}

UINT CSingerMediaResPanel::_onShowDir()
{
	UINT uItem = 0;
	if (currDir() == rootDir())
	{
		/*auto pSinger = m_wndAlbumPage.GetSinger();
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
		}*/

		for (auto& mediaDir : m_lstAttachDir)
		{
			m_wndList.InsertObject(mediaDir, uItem++);
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

		m_lstAttachDir.emplace_back(strDir);

		Refresh(); //m_wndList.InsertObject(m_lstAttachDir.back(), m_lstAttachDir.size());
	}
	else if (ID_Detach == uID)
	{
		auto pSinger = m_wndAlbumPage.GetSinger();
		__Ensure(pSinger);

		auto pListObject = m_wndList.GetSelObject();
		/*if (pListObject)
		{
			__Ensure(m_view.getSingerMgr().DetachDir(*pSinger, ((CMediaDir*)pListObject)->GetPath()));
			
			Refresh();
		}*/

		for (auto itr = m_lstAttachDir.begin(); itr != m_lstAttachDir.end(); ++itr)
		{
			if (&*itr == pListObject)
			{
				__Ensure(m_view.getSingerMgr().DetachDir(*pSinger, itr->GetAbsPath()));

				m_lstAttachDir.erase(itr);

				Refresh(); //m_wndList.DeleteItem(itr-m_lstAttachDir.begin());

				break;
			}
		}
	}
	else
	{
		CMediaResPanel::OnMenuCommand(uID, uVkKey);
	}
}
