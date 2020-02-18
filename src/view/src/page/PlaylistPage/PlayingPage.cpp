
#include "StdAfx.h"

#include "PlayingPage.h"

CPlayingPage::CPlayingPage(__view& view)
	: CBasePage(view, IDD_PAGE_PLAYING, L"ÕýÔÚ²¥·Å    ", IDR_MENU_PlayingPage, true)
	, m_wndList(view)
{
}

BEGIN_MESSAGE_MAP(CPlayingPage, CBasePage)
	ON_WM_SIZE()

	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CPlayingPage::OnNMRclickList)
END_MESSAGE_MAP()

void CPlayingPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);
	
	CPage::DoDataExchange(pDX);
}

BOOL CPlayingPage::OnInitDialog()
{
	CPage::OnInitDialog();

	__AssertReturn(m_wndList.InitCtrl(), FALSE);

	(void)__super::RegDragDropCtrl(m_wndList, [&](tagDragData& DragData) {
		TD_PlayItemList arrSelPlayItem;
		m_wndList.GetSelItems(arrSelPlayItem);
		__EnsureReturn(arrSelPlayItem, false);

		DragData.lstMedias.add(arrSelPlayItem);
		
		DragData.iImage = (int)E_GlobalImage::GI_PlayItem;

		return true;
	});

	__super::RegMenuHotkey(m_wndList, VK_F2, ID_RENAME);
	__super::RegMenuHotkey(m_wndList, VK_DELETE, ID_REMOVE);

	this->RefreshList();
	
	return TRUE;
}

void CPlayingPage::OnMenuCommand(UINT uID, UINT uVkKey)
{
	auto& PlayMgr = m_view.getPlayMgr();

	TD_PlayItemList arrSelPlayItem;
	m_wndList.GetSelItems(arrSelPlayItem);

	switch (uID)
	{
	case ID_PLAY:
	{
		int nSelItem = m_wndList.GetSelItem();
		if (nSelItem >= 0)
		{
			(void)PlayMgr.play(nSelItem);
		}
		else
		{
			PlayMgr.SetPlayStatus(E_PlayStatus::PS_Play);
		}
	}
	
	break;
	case ID_FIND:
		arrSelPlayItem.front([&](auto& playItem) {
			CRedrawLockGuard RedrawLockGuard(m_view.m_MainWnd);
			m_view.findMedia(playItem.GetPath(), false);
		});

		break;
	case ID_HITTEST:
		arrSelPlayItem.front([&](auto& playItem) {
			if (!m_view.hittestRelatedMediaSet(playItem, E_MediaSetType::MST_Singer))
			{
				m_view.m_MediaResPage.HittestMediaRes(playItem, *this);
			}
		});

		break;
	case ID_SETALARMCLOCK:
	{
		__AssertBreak(arrSelPlayItem);

		TD_IMediaList lstMedias(arrSelPlayItem);
		m_view.getDataMgr().addAlarmmedia(lstMedias);
	}

	break;
	case ID_CopyTitle:
		arrSelPlayItem.front([&](auto& playItem) {
			(void)m_view.copyMediaTitle(playItem);
		});

		break;
	case ID_EXPLORE:
		__EnsureBreak(1 == arrSelPlayItem.size());

		arrSelPlayItem.front([](auto& playItem) {
			playItem.ShellExplore();
		});

		break;
	case ID_RENAME:
		m_wndList.rename();
	
		break;
	case ID_REMOVE:
	{
		__EnsureBreak(!arrSelPlayItem.empty());

		auto ePrevPlayStatus = PlayMgr.GetPlayStatus();

		(void)PlayMgr.remove(arrSelPlayItem);

		if (ePrevPlayStatus != E_PlayStatus::PS_Stop && PlayMgr.GetPlayStatus() == E_PlayStatus::PS_Stop)
		{
			m_view.m_PlayCtrl.getPlaySpirit()->clear();
		}
	}
	
	break;
	case ID_CLEAR:
		(void)PlayMgr.clear();

		break;
	}
}

void CPlayingPage::RefreshList(int nPlayingItem)
{
	__Ensure(m_hWnd);

	m_wndList.refresh(nPlayingItem);

	CRect rcList;
	m_wndList.GetWindowRect(rcList);
	m_wndList.fixColumnWidth(rcList.Width());

	auto& lstPlayingItems = m_view.getPlayMgr().getPlayingItems();
	m_wndList.AsyncTask(__AsyncTaskElapse, [&](UINT uItem) {
		lstPlayingItems.get(uItem, [&](CPlayItem& PlayItem) {
			PlayItem.AsyncTask();
			m_wndList.Update(uItem);
		});
	});
}

void CPlayingPage::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndList)
	{
		m_wndList.MoveWindow(0, 0, cx, cy);

		m_wndList.fixColumnWidth(cx);
	}
}

DROPEFFECT CPlayingPage::OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndList && lstMedias, DROPEFFECT_NONE);

	if (!m_view.getPlayMgr().getPlayinglist().available())
	{
		return DROPEFFECT_COPY;
	}

	handleDragOver(m_wndList, DragContext);

	DROPEFFECT dwRet = DROPEFFECT_COPY;
	lstMedias.front([&](IMedia& media) {
		if (media.GetMediaSet() == &m_view.getPlayMgr().getPlayinglist())
		{
			dwRet = DROPEFFECT_MOVE;
		}
	});

	return dwRet;
}

BOOL CPlayingPage::OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndList && lstMedias, FALSE);

	UINT uDropPos = DragContext.uTargetPos;
	int nNewPos = 0;

	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.GetMediaSet();
	});

	if (pSrcMediaSet == &m_view.getPlayMgr().getPlayinglist())
	{
		TD_PlayItemList lstPlayItems(lstMedias);
		nNewPos = m_view.getPlayMgr().move(lstPlayItems, uDropPos);
		__EnsureReturn(nNewPos >= 0, FALSE);
	}
	else
	{
		__EnsureReturn(m_view.m_PlayCtrl.addPlayingItem(lstMedias, uDropPos), FALSE);
		nNewPos = uDropPos;
	}

	m_wndList.SelectItems(nNewPos, lstMedias.size());
		
	return TRUE;
}

DROPEFFECT CPlayingPage::OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndList && pMediaSet, DROPEFFECT_NONE);

	TD_MediaList lstMedias;
	pMediaSet->GetAllMedias(lstMedias);
	__EnsureReturn(lstMedias, DROPEFFECT_NONE);

	return OnMediasDragOver(pwndCtrl, TD_IMediaList(lstMedias), DragContext);
}

BOOL CPlayingPage::OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndList && pMediaSet, FALSE);

	TD_MediaList lstMedias;
	pMediaSet->GetAllMedias(lstMedias);
	__EnsureReturn(lstMedias, FALSE);

	return OnMediasDrop(pwndCtrl, TD_IMediaList(lstMedias), DragContext);
}

void CPlayingPage::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (!m_view.getPlayMgr().getPlayinglist().available())
	{
		return;
	}

	UINT nSelCount = m_wndList.GetSelectedCount();

	if (0 == nSelCount)
	{
		m_MenuGuard.EnableItem(ID_SETALARMCLOCK, false);
		
		m_MenuGuard.EnableItem(ID_REMOVE, false);
	}

	if (1 != nSelCount)
	{
		m_MenuGuard.EnableItem(ID_FIND, false);
		m_MenuGuard.EnableItem(ID_HITTEST, false);

		m_MenuGuard.EnableItem(ID_CopyTitle, false);
		m_MenuGuard.EnableItem(ID_EXPLORE, false);
		m_MenuGuard.EnableItem(ID_RENAME, false);
	}

	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);
}
