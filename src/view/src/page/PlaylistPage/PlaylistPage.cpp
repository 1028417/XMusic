
#include "StdAfx.h"

#include "PlaylistPage.h"

#include "PlayItemPage.h"

#include "PlayingPage.h"

CPlaylistPage::CPlaylistPage(__view& view, CPlayItemPage& PlayItemPage)
	: CBasePage(view, IDD_PAGE_PLAYLIST, L"歌 单     ", IDR_MENU_PLAYLIST, true)
	, m_PlayItemPage(PlayItemPage)
{
}

BEGIN_MESSAGE_MAP(CPlaylistPage, CBasePage)
	ON_WM_SIZE()

	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1, CPlaylistPage::OnLvnEndlabeleditList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, CPlaylistPage::OnLvnItemchangedList)

	ON_NOTIFY(NM_CLICK, IDC_LIST1, CPlaylistPage::OnNMClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, CPlaylistPage::OnNMDclickList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, CPlaylistPage::OnNMRclickList)

	//ON_NOTIFY(NM_SETFOCUS, IDC_LIST1, &CPlaylistPage::OnNMSetFocusList)
END_MESSAGE_MAP()

void CPlaylistPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);

	CPage::DoDataExchange(pDX);
}

void CPlaylistPage::OnActive(BOOL bActive)
{
	if (bActive && m_PlayItemPage)
	{
		m_PlayItemPage.Active();
	}
}

BOOL CPlaylistPage::OnInitDialog()
{
	CPage::OnInitDialog();

	(void)m_wndList.ModifyStyle(0
		, LVS_NOCOLUMNHEADER | LVS_EDITLABELS | LVS_SINGLESEL);
	m_wndList.ModifyStyleEx(0, WS_EX_LEFTSCROLLBAR);

	m_wndList.SetImageList(NULL, &m_view.m_ImgMgr.bigImglst());

	CObjectList::tagListPara ListPara(0);
	
	ListPara.fFontSize = m_view.m_globalSize.m_fMidFontSize;
	ListPara.crText = __Color_Text;

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	__AssertReturn(m_fontBig.create(*this, m_view.m_globalSize.m_fBigFontSize * 3.5f), FALSE);

	float fSmallFontSize = m_view.m_globalSize.m_fSmallFontSize * .5f;
	__AssertReturn(m_fontSmall.create(*this, fSmallFontSize), FALSE);

	m_wndList.SetCustomDraw(NULL, [&](tagLVDrawSubItem& lvcd) {
		CPlaylist *pPlaylist = (CPlaylist *)lvcd.pObject;
		if (pPlaylist)
		{
			CDC& dc = lvcd.dc;

			BYTE uTextAlpha = 0;
			if (pPlaylist->property().isDisableDemand() && pPlaylist->property().isDisableExport())
			{
				uTextAlpha = 200;
			}
			else if (pPlaylist->property().isDisableDemand() || pPlaylist->property().isDisableExport())
			{
				uTextAlpha = 128;
			}
			dc.SetTextColor(lvcd.getTextColor(uTextAlpha));

			RECT rc(lvcd.rc);

			auto offset = (rc.bottom - rc.top +1 - (int)m_view.m_globalSize.m_uBigIconSize) / 2;
			m_view.m_ImgMgr.bigImglst().Draw(&lvcd.dc, (int)E_GlobalImage::GI_Playlist, { offset, rc.top + offset }, 0);

			rc.left += 100;
			rc.top -= 22;
			rc.bottom -= 12;

			dc.DrawText(pPlaylist->m_strName.c_str(), &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

			auto pPrevFont = dc.SelectObject(m_fontBig);
			dc.SetTextColor(lvcd.getTextColor(236));

			cauto strIndex = to_wstring(pPlaylist->index() + 1) + L" ";
			dc.DrawText(strIndex.c_str(), &lvcd.rc, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

			auto uCount = pPlaylist->playItems().size();
			if (uCount != 0)
			{
				dc.SetTextColor(lvcd.getTextColor(MAX(uTextAlpha, 150)));
				dc.SelectObject(m_fontSmall);
				dc.DrawText((to_wstring(uCount) + L"首").c_str(), &rc, DT_LEFT | DT_BOTTOM | DT_SINGLELINE);
			}
			
			dc.SelectObject(pPrevFont);
		}
	});

	(void)__super::RegDragDropCtrl(m_wndList, [&](tagDragData& DragData) {
		CPlaylist *pPlaylist = (CPlaylist*)m_wndList.GetSelObject();
		__EnsureReturn(pPlaylist, false);

		DragData.pMediaSet = pPlaylist;

		DragData.iImage = (int)E_GlobalImage::GI_Playlist;

		return true;
	});
	
	__super::RegMenuHotkey(m_wndList, VK_RETURN, ID_PLAY);
	__super::RegMenuHotkey(m_wndList, VK_F2, ID_Renme);
	__super::RegMenuHotkey(m_wndList, VK_DELETE, ID_REMOVE);

	this->RefreshList();

	return TRUE;
}

void CPlaylistPage::RefreshList()
{
	__Ensure(m_hWnd);

	(void)m_wndList.SetObjects(TD_ListObjectList((SList<CPlaylist>&)m_view.getPlaylistMgr().playlists()));
	
	if (m_view.m_PlayItemPage)
	{
		(void)m_wndList.SelectFirstItem();
	}
}

void CPlaylistPage::fixColumnWidth(int width)
{
	if (m_wndList.GetItemCount() > m_wndList.GetCountPerPage())
	{
		width -= m_view.m_globalSize.m_uScrollbarWidth;
	}
	(void)m_wndList.SetColumnWidth(0, width-1);
}

void CPlaylistPage::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndList)
	{
		if (cx == m_width)
		{
			m_wndList.MoveWindow(0, 0, cx, cy);
			return;
		}

		if (cx > m_width)
		{
			m_wndList.MoveWindow(0, 0, cx, cy, FALSE);
		}

		fixColumnWidth(cx);

		if (cx < m_width)
		{
			m_wndList.MoveWindow(0, 0, cx, cy, TRUE);
		}
		
		m_width = cx;
	}
}

void CPlaylistPage::OnMenuCommand(UINT uID, UINT uVkKey)
{
	int nItem = m_wndList.GetSelItem();
	CPlaylist *pPlaylist = (CPlaylist*)m_wndList.GetSelObject();		

	switch (uID)
	{
	case ID_PLAY:
		__Ensure(pPlaylist);
		m_view.m_PlayCtrl.addPlayingItem(*pPlaylist);

		break;
	case ID_VERIFY:
		__Assert(pPlaylist);
		m_view.verifyMedia(*pPlaylist);

		break;
	case ID_Export:
		__Assert(pPlaylist);
		m_view.exportMediaSet(*pPlaylist);

		break;
	case ID_ADD_PLAYITEM:
		m_view.m_PlayItemPage.OnMenuCommand(ID_ADD_PLAYITEM);
		
		break;
	case ID_ADD_PLAYLIST:
		if (nItem < 0)
		{
			nItem = 0;
		}
		pPlaylist = m_view.getPlaylistMgr().AddPlaylist(nItem);
		__AssertBreak(pPlaylist);

		nItem = m_wndList.InsertObject(*pPlaylist, nItem);

		fixColumnWidth(m_width);

		m_wndList.UpdateColumn(0);

		m_wndList.EditLabel(nItem);
		
		break;
	case ID_Renme:
		__EnsureBreak(nItem >= 0);
		m_wndList.EditLabel(nItem);

		break;
	case ID_REMOVE:
	{
		__Assert(pPlaylist);
		__EnsureBreak(nItem >= 0);

		__EnsureBreak(confirmBox(L"确认删除所选列表?"));

		(void)m_wndList.DeleteItem(nItem);

		fixColumnWidth(m_width);

		m_wndList.UpdateColumn(0);

		int nItemCount = m_wndList.GetItemCount();
		if (0 == nItemCount)
		{
			m_PlayItemPage.ShowPlaylist(NULL);
		}
		else if (nItem < nItemCount)
		{
			m_wndList.SelectItem(nItem);
		}
		else
		{
			m_wndList.SelectItem(nItemCount - 1);
		}

		__EnsureBreak(m_view.getController().removeMediaSet(*pPlaylist));

		m_wndList.UpdateColumn(0);
	}
	
	break;
	case ID_DisableDemand:
	case ID_DisableExport:
	case ID_CNLanguage:
	case ID_HKLanguage:
	case ID_KRLanguage:
	case ID_JPLanguage:
	case ID_ENLanguage:
	case ID_OtherLanguage:
	case ID_TlLanguage:
	case ID_RsLanguage:
	case ID_FrLanguage:
	{
		__AssertBreak(pPlaylist);
		CMediasetProperty property(pPlaylist->property());
		if (ID_DisableDemand == uID)
		{
			bool bDisableDemand = !property.isDisableDemand();
			property.setDisableDemand(bDisableDemand);
			property.setDisableExport(bDisableDemand);
			m_wndList.UpdateItem(nItem);
		}
		else if (ID_DisableExport == uID)
		{
			property.setDisableExport(!property.isDisableExport());
			m_wndList.UpdateItem(nItem);
		}
		else
		{
			E_LanguageType eLanguage = E_LanguageType::LT_None;
			switch (uID)
			{
			case ID_CNLanguage:
				eLanguage = E_LanguageType::LT_CN;
				break;
			case ID_HKLanguage:
				eLanguage = E_LanguageType::LT_HK;
				break;
			case ID_KRLanguage:
				eLanguage = E_LanguageType::LT_KR;
				break;
			case ID_JPLanguage:
				eLanguage = E_LanguageType::LT_JP;
				break;
			case ID_ENLanguage:
				eLanguage = E_LanguageType::LT_EN;
				break;
			case ID_OtherLanguage:
				eLanguage = E_LanguageType::LT_Other;
				break;
			case ID_TlLanguage:
				eLanguage = E_LanguageType::LT_TL;
				break;
			case ID_RsLanguage:
				eLanguage = E_LanguageType::LT_RS;
				break;
			case ID_FrLanguage:
				eLanguage = E_LanguageType::LT_FR;
				break;
			default:
				return;
			};

			if ((property.language() & (UINT)eLanguage) == (UINT)eLanguage)
			{
				property.unsetLanguage(eLanguage);
			}
			else
			{
				property.setLanguage(eLanguage);
			}
		};

		(void)m_view.getDataMgr().updateMediaSetProperty(*pPlaylist, property);
	}
	break;
	}
}

void CPlaylistPage::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNM = (LPNMLISTVIEW)pNMHDR;	
	BOOL bEnable = (lpNM->iItem >= 0);

	m_MenuGuard.EnableItem(ID_ADD_PLAYITEM, bEnable);	
	m_MenuGuard.EnableItem(ID_Renme, bEnable);
	m_MenuGuard.EnableItem(ID_REMOVE, bEnable);

	m_MenuGuard.EnableItem(ID_DisableDemand, bEnable);
	m_MenuGuard.EnableItem(ID_DisableExport, bEnable);

	m_MenuGuard.EnableItem(ID_CNLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_HKLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_KRLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_JPLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_ENLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_OtherLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_TlLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_RsLanguage, bEnable);
	m_MenuGuard.EnableItem(ID_FrLanguage, bEnable);

	bool bAvalible = false;
	if (bEnable)
	{
		CPlaylist *pPlaylist = (CPlaylist*)m_wndList.GetItemObject(lpNM->iItem);
		__Assert(pPlaylist);
		const CMediasetProperty& property = pPlaylist->property();

		m_MenuGuard.CheckItem(ID_DisableDemand, property.isDisableDemand());
		m_MenuGuard.CheckItem(ID_DisableExport, property.isDisableExport());
		
		m_MenuGuard.CheckItem(ID_CNLanguage, property.isCnLanguage());
		m_MenuGuard.CheckItem(ID_HKLanguage, property.isHkLanguage());
		m_MenuGuard.CheckItem(ID_KRLanguage, property.isKrLanguage());
		m_MenuGuard.CheckItem(ID_JPLanguage, property.isJpLanguage());
		m_MenuGuard.CheckItem(ID_ENLanguage, property.isEnLanguage());

		m_MenuGuard.CheckItem(ID_OtherLanguage, property.isOtherLanguage());
		if (property.isTlLanguage() || property.isRsLanguage() || property.isFrLanguage())
		{
			m_MenuGuard.EnableItem(ID_OtherLanguage, false);
		}

		m_MenuGuard.CheckItem(ID_TlLanguage, property.isTlLanguage());
		m_MenuGuard.CheckItem(ID_RsLanguage, property.isRsLanguage());
		m_MenuGuard.CheckItem(ID_FrLanguage, property.isFrLanguage());

		bAvalible = pPlaylist->playItems();
	}

	m_MenuGuard.EnableItem(ID_PLAY, bAvalible);
	m_MenuGuard.EnableItem(ID_VERIFY, bAvalible);
	m_MenuGuard.EnableItem(ID_Export, bAvalible);

	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);
}

void CPlaylistPage::OnLvnEndlabeleditList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	NMLVDISPINFO *pLVDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	int nItem = pLVDispInfo->item.iItem;

	wstring strNewName = pLVDispInfo->item.pszText;

	CPlaylist *pPlaylist = (CPlaylist*)m_wndList.GetItemObject(nItem);
	__Assert(pPlaylist);

	auto eRetCode = m_view.getController().renameMediaSet(*pPlaylist, strNewName);
	if (E_RenameRetCode::RRC_Success != eRetCode)
	{
		if (E_RenameRetCode::RRC_InvalidName == eRetCode)
		{
			msgBox(L"名称含特殊字符！");
		}
		else if (E_RenameRetCode::RRC_NameExists == eRetCode)
		{
			msgBox(L"重命名失败，存在同名列表！");
		}

		return;
	}

	(void)m_wndList.UpdateItem(pLVDispInfo->item.iItem);

	m_PlayItemPage.UpdateTitle();
}

DROPEFFECT CPlaylistPage::OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{	
	__AssertReturn(pwndCtrl == &m_wndList && pMediaSet, DROPEFFECT_NONE);

	__EnsureReturn(E_MediaSetType::MST_Playlist == pMediaSet->type(), DROPEFFECT_NONE);
	
	int nDragingItem = m_wndList.GetObjectItem(pMediaSet);
	__AssertReturn(nDragingItem >= 0, DROPEFFECT_NONE);

	handleDragOver(m_wndList, DragContext);
	
	return DROPEFFECT_MOVE;
}

BOOL CPlaylistPage::OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndList && pMediaSet && E_MediaSetType::MST_Playlist == pMediaSet->type(), FALSE);

	int nDragingItem = m_wndList.GetObjectItem(pMediaSet);
	__AssertReturn(nDragingItem >= 0, FALSE);

	int nTargetPos = DragContext.uTargetPos;
	if (nTargetPos > nDragingItem)
	{
		nTargetPos--;
	}
	__EnsureReturn(nTargetPos != nDragingItem, FALSE);

	CRedrawLock RedrawLock(m_PlayItemPage.m_wndList);
	m_PlayItemPage.ShowPlaylist(NULL);

	CPlaylist *pPlaylist = m_view.getPlaylistMgr().RepositPlaylist(pMediaSet->m_uID, (UINT)nTargetPos);
	__EnsureReturn(pPlaylist, FALSE);
	
	(void)m_wndList.DeleteItem(nDragingItem);
	(void)m_wndList.InsertObject(*pPlaylist, nTargetPos);
	m_wndList.SelectItem(nTargetPos);

	m_wndList.UpdateColumn(0);

	return TRUE;
}

DROPEFFECT CPlaylistPage::OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndList && lstMedias, DROPEFFECT_NONE);

	bool bScroll = DragScroll(m_wndList, DragContext.x, DragContext.y);

	int nItem = m_wndList.HitTest(CPoint(5, DragContext.y));
	__EnsureReturn(nItem >= 0, DROPEFFECT_NONE);
	CPlaylist *pDragOverPlaylist = (CPlaylist*)m_wndList.GetItemObject(nItem);
	__AssertReturn(pDragOverPlaylist, DROPEFFECT_NONE);

	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.mediaSet();
	});

	__EnsureReturn(pSrcMediaSet != pDragOverPlaylist, DROPEFFECT_NONE);
	DragContext.pTargetObj = pDragOverPlaylist;

	if (!bScroll)
	{
		CRect rcItem;
		(void)m_wndList.GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
		DragContext.DrawDragOverRect(rcItem.top, rcItem.bottom);
	}

	if (pSrcMediaSet && E_MediaSetType::MST_Playlist == pSrcMediaSet->type())
	{
		if (0 == (DragContext.dwKeyState & MK_CONTROL))
		{
			return DROPEFFECT_MOVE;
		}
	}
	
	return DROPEFFECT_COPY;
}

BOOL CPlaylistPage::OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndList && lstMedias, FALSE);

	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.mediaSet();
	});

	CPlaylist *pDragOverPlaylist = (CPlaylist*)DragContext.pTargetObj;
	__AssertReturn(pDragOverPlaylist && pSrcMediaSet != pDragOverPlaylist, FALSE);

	m_wndList.SelectObject(pDragOverPlaylist);
	
	__EnsureReturn(m_view.getPlaylistMgr().AddPlayItems(lstMedias, *pDragOverPlaylist, 0), FALSE);
	
	if (DROPEFFECT_MOVE == DragContext.nDropEffect && pSrcMediaSet && E_MediaSetType::MST_Playlist == pSrcMediaSet->type())
	{
		TD_PlayItemList lstPlayItems(lstMedias);
		__EnsureReturn(m_view.getPlaylistMgr().RemovePlayItems((ConstPtrArray<CPlayItem>&)lstPlayItems), FALSE);
	}
		
	int	nSelectCount = (int)lstMedias.size();
	m_PlayItemPage.m_wndList.SelectItems(0, nSelectCount);
	m_PlayItemPage.Active();

	return TRUE;
}

void CPlaylistPage::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	__Ensure(pNMLV->uChanged & LVIF_STATE)

	__Ensure(pNMLV->uNewState & LVIS_SELECTED)

	auto pPlaylist = (CPlaylist*)m_wndList.GetItemObject(pNMLV->iItem);
	if (pPlaylist != m_PlayItemPage.m_pPlaylist)
	{
		m_PlayItemPage.ShowPlaylist(pPlaylist);
		(void)this->SetFocus();
	}
}

void CPlaylistPage::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (!::IsWindowVisible(m_PlayItemPage))
	{
		LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

		CPlaylist *pPlaylist = (CPlaylist*)m_wndList.GetItemObject(pNMItemActivate->iItem);
		if (pPlaylist)
		{
			m_PlayItemPage.ShowPlaylist(pPlaylist);
			(void)this->SetFocus();
		}
	}
}

void CPlaylistPage::OnNMDclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	OnNMClickList(pNMHDR, pResult);

	this->OnMenuCommand(ID_PLAY);
}

void CPlaylistPage::OnNMSetFocusList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (m_view.m_PlayItemPage.m_pPlaylist)
	{
		m_wndList.SelectObject(m_view.m_PlayItemPage.m_pPlaylist);
	}
}
