
#include "StdAfx.h"

#include "SingerPage.h"
#include "AlbumPage.h"

#include "dlg/ChooseDirDlg.h"

CSingerPage::CSingerPage(__view& view, CAlbumPage& AlbumPage)
	: CBasePage(view, IDD_PAGE_SINGER, L"歌 手     ", IDR_MENU_SINGEROBJECT, true)
	, m_AlbumPage(AlbumPage)
{
}

BEGIN_MESSAGE_MAP(CSingerPage, CBasePage)
	ON_WM_SIZE()

	ON_NOTIFY(TVN_ENDLABELEDIT, IDC_TREE, &CSingerPage::OnTvnEndlabeleditTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, &CSingerPage::OnTvnSelchangedTree)
	ON_NOTIFY(TVN_ITEMEXPANDED, IDC_TREE, &CSingerPage::OnTvnItemexpandedTree)

	ON_NOTIFY(NM_DBLCLK, IDC_TREE, &CSingerPage::OnNMDblclkTree)
	ON_NOTIFY(NM_RCLICK, IDC_TREE, &CSingerPage::OnNMRclickTree)
END_MESSAGE_MAP()

void CSingerPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_TREE, m_wndTree);

	CPage::DoDataExchange(pDX);
}

BOOL CSingerPage::OnInitDialog()
{
	(void)CPage::OnInitDialog();

	(void)m_wndTree.ModifyStyle(0 | TVS_LINESATROOT | TVS_HASBUTTONS
		, TVS_EDITLABELS | TVS_HASLINES);
	m_wndTree.ModifyStyleEx(0, WS_EX_LEFTSCROLLBAR);

	m_wndTree.SetTextColor(__Color_Text);
	m_wndTree.SetLineColor(RGB(200, 200, 255));

	m_wndTree.SetFontSize(m_view.m_globalSize.m_fMidFontSize);

	m_wndTree.SetImageList(m_view.m_ImgMgr.bigImglst());

	m_wndTree.SetIndent(m_wndTree.GetIndent() - 10);

	m_wndTree.SetCustomDraw([&](tagTVDrawItem& tvcd) {
		CMediaSet *pSingerObject = (CMediaSet *)tvcd.pObject;
		if (pSingerObject)
		{
			if (pSingerObject->property().isDisableDemand() && pSingerObject->property().isDisableExport())
			{
				tvcd.setTextAlpha(200);
			}
			else if (pSingerObject->property().isDisableDemand() || pSingerObject->property().isDisableExport())
			{
				tvcd.setTextAlpha(128);
			}
		}
	});

	(void)__super::RegDragDropCtrl(m_wndTree, [&](tagDragData& DragData) {
		CMediaSet *pSingerObject = (CMediaSet*)m_wndTree.GetSelectedObject();
		__AssertReturn(pSingerObject, false);

		__EnsureReturn(E_MediaSetType::MST_Singer == pSingerObject->m_eType, false);

		DragData.pMediaSet = pSingerObject;

		DragData.iImage = m_view.m_ImgMgr.getSingerImgPos(pSingerObject->m_uID);;

		return true;
	});

	__super::RegMenuHotkey(m_wndTree, VK_RETURN, ID_PLAY);
	__super::RegMenuHotkey(m_wndTree, VK_F2, ID_RENAME);
	__super::RegMenuHotkey(m_wndTree, VK_DELETE, ID_REMOVE);

	(void)this->RefreshTree();

	return TRUE;
}

BOOL CSingerPage::RefreshTree(CMediaSet *pSingerObject)
{
	__EnsureReturn(m_hWnd, FALSE);

	__waitCursor;
	m_AlbumPage.ShowSinger(NULL);

	CRedrawLock RedrawLock(m_wndTree);
	
	m_bRefreshing = true;
	(void)m_wndTree.DeleteAllItems();
	m_bRefreshing = false;

	//(void)m_wndTree.SetFocus();

	auto& SingerMgr = m_view.getSingerMgr();

	TD_MediaSetList lstSubSets;
	SingerMgr.GetSubSets(lstSubSets);
	lstSubSets([&](CMediaSet& SubSet) {		
		m_wndTree.InsertObject(SubSet);

		if (E_MediaSetType::MST_SingerGroup == SubSet.m_eType)
		{
			CSingerGroup& group = (CSingerGroup&)SubSet;
			for (cauto singer : group.singers())
			{
				m_wndTree.InsertObject((CTreeObject&)singer, &group);
			}

			if (m_setExpandedGroupIDs.find(group.m_uID) != m_setExpandedGroupIDs.end())
			{
				(void)m_wndTree.ExpandObject(group);
			}
		}
	});

	if (NULL == pSingerObject || E_MediaSetType::MST_Singer != pSingerObject->m_eType)
	{
		if (0 != m_uSingerID)
		{
			auto pSinger = SingerMgr.FindSubSet(E_MediaSetType::MST_Singer, m_uSingerID);
			if (NULL != pSinger)
			{
				(void)m_wndTree.SelectObject(*pSinger);
				if (NULL == pSingerObject)
				{
					(void)m_wndTree.EnsureVisible(*pSinger);
				}
			}
			else
			{
				m_uSingerID = 0;
			}
		}
	}

	RedrawLock.Unlock();
	
	if (pSingerObject)
	{
		(void)m_wndTree.SelectObject(*pSingerObject);
		(void)m_wndTree.EnsureVisible(*pSingerObject);
	}
	
	return TRUE;
}

void CSingerPage::Active(CSinger& Singer)
{
	__super::Active();

	(void)m_wndTree.SelectObject(Singer);
	(void)m_wndTree.EnsureVisible(Singer);
}

void CSingerPage::OnActive(BOOL bActive)
{
	if (bActive && m_AlbumPage)
	{
		m_AlbumPage.Active();
	}
}

void CSingerPage::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndTree)
	{
		m_wndTree.MoveWindow(0,0,cx,cy);
	}
}

void CSingerPage::_addSinger(CSingerGroup *pGroup)
{
	m_view.getModel().refreshMediaLib();
	CMediaRes *pSrcPath = m_view.showChooseDirDlg(L"选择歌手目录", false);
	__Ensure(pSrcPath);

	BOOL bInitAlbum = confirmBox(L"是否生成专辑列表?");

	__waitCursor;
	
	CSinger *pSinger = m_view.getSingerMgr().AddSinger(*pSrcPath, pGroup, bInitAlbum);
	__Ensure(pSinger);

	__Ensure(this->RefreshTree(pSinger));

	CEdit *pEdit = m_wndTree.EditObject(*pSinger);
	__Assert(pEdit);
	(void)pEdit->SetWindowText(pSrcPath->GetName().c_str());
	pEdit->SetSel(0, -1);
}

void CSingerPage::OnMenuCommand(UINT uID, UINT uVkKey)
{
	CMediaSet *pSingerObject = (CMediaSet*)m_wndTree.GetSelectedObject();

	switch (uID)
	{
	case ID_VERIFY:
		__Assert(pSingerObject);
		m_view.verifyMedia(*pSingerObject);

		break;
	case ID_EXPORT:
		__Assert(pSingerObject);
		m_view.exportMediaSet(*pSingerObject);

		break;
	case ID_ADD_GROUP:
	{
		CSingerGroup *pGroup = m_view.getSingerMgr().AddGroup();
		__EnsureBreak(pGroup);

		__EnsureBreak(this->RefreshTree(pGroup));

		(void)m_wndTree.EditObject(*pGroup);
	}

	break;
	case ID_ADD_SINGER:
	{
		CSingerGroup *pGroup = NULL;
		if (pSingerObject)
		{
			if (E_MediaSetType::MST_Singer == pSingerObject->m_eType)
			{					
				pGroup = (CSingerGroup*)(CMediaSet*)m_wndTree.GetParentObject(*(CTreeObject*)pSingerObject);
			}
			else
			{
				pGroup = (CSingerGroup*)pSingerObject;
			}
		}

		_addSinger(pGroup);
	}

	break;
	case ID_PLAY:
		__EnsureBreak(pSingerObject && E_MediaSetType::MST_Singer == pSingerObject->m_eType);
		
		m_view.m_PlayCtrl.addPlayingItem(*(CSinger*)pSingerObject);
		
		break;
	case ID_RENAME:
		__EnsureBreak(pSingerObject);
		(void)m_wndTree.EditObject(*pSingerObject);

		break;
	case ID_REMOVE:
		__EnsureBreak(pSingerObject);
		
		__EnsureBreak(confirmBox(E_MediaSetType::MST_Singer == pSingerObject->m_eType ?
				L"确认删除所选歌手?": L"确认删除选中的组?"));

		m_AlbumPage.ShowSinger(NULL);

		__EnsureBreak(m_view.getController().removeMediaSet(*pSingerObject));
		
		m_view.m_ImgMgr.removeSingerImg(pSingerObject->m_uID, pSingerObject->m_strName);

		(void)this->RefreshTree();
		
		break;
	case ID_ADD_IMAGE:
	{
		__AssertBreak(pSingerObject && E_MediaSetType::MST_Singer == pSingerObject->m_eType);

		static auto& strFilter = L"所有支持图片|*.Jpg;*.Jpeg;*.Jfif;*.Png;*.Gif;*.Bmp\
			|Jpg文件|*.Jpg|Jpeg文件|*.Jpeg|Jpeg文件|*.Jfif \
			|Png文件|*.Png|Gif文件|*.Gif|位图文件(*.Bmp)|*.Bmp|";

		tagFileDlgOpt FileDlgOpt;
		FileDlgOpt.strTitle = L"选择歌手头像";
		FileDlgOpt.strFilter = strFilter;
		static CFileDlgEx fileDlg(FileDlgOpt);

		list<wstring> lstFiles;
		(void)fileDlg.ShowOpenMulti(lstFiles);
		if (!lstFiles.empty())
		{
			m_view.addSingerImage(*(CSinger*)pSingerObject, lstFiles);

			m_wndTree.UpdateImage(*pSingerObject);

			m_AlbumPage.UpdateTitle();
			m_AlbumPage.UpdateSingerImage();
		}
	}

	break;
	case ID_FIND:
		m_view.findMedia(((CSinger*)pSingerObject)->GetBaseDir(), true, pSingerObject->m_strName);
		
		break;
	case ID_DisableDemand:
	case ID_DisableExport:
	case ID_CNLanguage:
	case ID_HKLanguage:
	case ID_KRLanguage:
	case ID_JPLanguage:
	case ID_TAILanguage:
	case ID_ENLanguage:
	case ID_EURLanguage:
	{
		__AssertBreak(pSingerObject);
		CMediasetProperty property = pSingerObject->property();

		if (ID_DisableDemand == uID)
		{
			bool bDisableDemand = !property.isDisableDemand();
			property.setDisableDemand(bDisableDemand);
			property.setDisableExport(bDisableDemand);
			m_wndTree.RedrawObject(*pSingerObject);
		}
		else if (ID_DisableExport == uID)
		{
			property.setDisableExport(!property.isDisableExport());
			m_wndTree.RedrawObject(*pSingerObject);
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
			case ID_TAILanguage:
				eLanguage = E_LanguageType::LT_TAI;
				break;
			case ID_ENLanguage:
				eLanguage = E_LanguageType::LT_EN;
				break;
			case ID_EURLanguage:
				eLanguage = E_LanguageType::LT_EUR;
				break;
			default:
				return;
			};

			if (property.language() & (UINT)eLanguage)
			{
				property.unsetLanguage(eLanguage);
			}
			else
			{
				property.setLanguage(eLanguage);
			}
		};

		(void)m_view.getDataMgr().updateMediaSetProperty(*pSingerObject, property);
	}
	break;
	};
}

void CSingerPage::OnTvnEndlabeleditTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);

	wstring strNewName = pTVDispInfo->item.pszText;

	CMediaSet *pSingerObject = (CMediaSet*)m_wndTree.GetItemObject(pTVDispInfo->item.hItem);
	__Assert(pSingerObject);
	wstring strOldName = pSingerObject->m_strName;

	auto eRetCode = m_view.getController().renameMediaSet(*pSingerObject, strNewName);
	if (E_RenameRetCode::RRC_Success != eRetCode)
	{
		if (E_RenameRetCode::RRC_InvalidName == eRetCode)
		{
			msgBox(L"名称含特殊字符！");
		}
		else if (E_RenameRetCode::RRC_NameExists == eRetCode)
		{
			msgBox(E_MediaSetType::MST_Singer == pSingerObject->m_eType ?
				L"重命名失败，存在同名歌手！" : L"重命名失败，存在同名组！");
		}

		return;
	}

	m_view.getSingerImgMgr().renameSinger(strOldName, strNewName);

	(void)m_wndTree.SetItemText(pTVDispInfo->item.hItem, strNewName.c_str());

	if (m_AlbumPage)
	{
		m_AlbumPage.UpdateSingerName();
	}
}

DROPEFFECT CSingerPage::OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndTree && pMediaSet, DROPEFFECT_NONE);

	__EnsureReturn(E_MediaSetType::MST_Singer == pMediaSet->m_eType, DROPEFFECT_NONE);
	
	CMediaSet *pDstSingerObject = NULL;

	UINT uFlag = TVHT_NOWHERE;
	HTREEITEM hItem = m_wndTree.HitTest(CPoint(DragContext.x, DragContext.y), &uFlag);
	__EnsureReturn(NULL != hItem, DROPEFFECT_NONE);
	//if (!hItem)
	//{
	//	//pDstSingerObject = m_view.getSingerMgr().GetLastObject();
	//	if (!m_view.getSingerMgr().m_lstRootSingers.empty())
	//	{
	//		pDstSingerObject = &m_view.getSingerMgr().m_lstRootSingers.back();

	//		if (pMediaSet == pDstSingerObject)
	//		{
	//			DragContext.pTargetObj = pDstSingerObject;
	//		}
	//	}
	//	else
	//	{
	//		__AssertReturn(!m_view.getSingerMgr().m_lstGroups.empty(), DROPEFFECT_NONE);

	//		auto& lastGroup = m_view.getSingerMgr().m_lstGroups.back();
	//		if (!lastGroup.m_lstSingers.empty())
	//		{
	//			pDstSingerObject = &lastGroup.m_lstSingers.back();
	//		}
	//		else
	//		{
	//			pDstSingerObject = &lastGroup;
	//		}
	//	}

	//	CRect rcItem;
	//	(void)m_wndTree.GetItemRect(pDstSingerObject->m_hTreeItem, &rcItem, FALSE);

	//	DragContext.y = rcItem.bottom;

	//	DragContext.DrawDragOverHLine(DragContext.y);

	//	return DROPEFFECT_MOVE;
	//}

	pDstSingerObject = (CMediaSet*)m_wndTree.GetItemObject(hItem);
	__AssertReturn(pDstSingerObject, DROPEFFECT_NONE);

	DragContext.pTargetObj = pDstSingerObject;

	if (DragContext.y < 30)
	{
		if (m_wndTree.GetScrollPos(SB_VERT) > 0)
		{
			(void)m_wndTree.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEUP, 0));
		}
	}
	else
	{
		CRect rcClient;
		m_wndTree.GetClientRect(rcClient);
		if (DragContext.y > rcClient.bottom-30)
		{
			int iMinPos = 0;
			int iMaxPos = 0;
			m_wndTree.GetScrollRange(SB_VERT, &iMinPos, &iMaxPos);

			if (m_wndTree.GetScrollPos(SB_VERT) < iMaxPos)
			{
				(void)m_wndTree.SendMessage(WM_VSCROLL, MAKELONG(SB_LINEDOWN, 0));
			}
		}
	}

	CRect rcItem;
	(void)m_wndTree.GetItemRect(m_wndTree.getTreeItem(pDstSingerObject), &rcItem, FALSE);

	if (E_MediaSetType::MST_Singer == pDstSingerObject->m_eType)
	{
		if (DragContext.y < rcItem.CenterPoint().y)
		{
			DragContext.y = rcItem.top;
			
			m_eDropPositionFlag = DPF_UP;
		}
		else
		{
			DragContext.y = rcItem.bottom;

			m_eDropPositionFlag = DPF_DOWN;
		}

		DragContext.DrawDragOverHLine(DragContext.y);
	}
	else
	{
		DragContext.DrawDragOverRect(rcItem.top, rcItem.bottom);
	}

	return DROPEFFECT_MOVE;
}

BOOL CSingerPage::OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndTree, FALSE);

	CMediaSet *pTarget = (CMediaSet*)DragContext.pTargetObj;
	__AssertReturn(pTarget, FALSE);

	__AssertReturn(pMediaSet && E_MediaSetType::MST_Singer == pMediaSet->m_eType, FALSE);

	__waitCursor;
	CSinger *pSinger = m_view.getSingerMgr().RepositSinger(*(CSinger*)pMediaSet, *pTarget, DPF_DOWN == m_eDropPositionFlag);
	__EnsureReturn(pSinger, FALSE);
	(void)this->RefreshTree(pSinger);

	return TRUE;
}

void CSingerPage::OnTvnSelchangedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (m_bRefreshing)
	{
		return;
	}
		
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	__Ensure(pNMTreeView->itemNew.hItem);
	
	CMediaSet *pSingerObject = (CMediaSet*)m_wndTree.GetItemObject(pNMTreeView->itemNew.hItem);
	__Assert(pSingerObject);
	__Ensure(E_MediaSetType::MST_Singer == pSingerObject->m_eType);

	m_uSingerID = pSingerObject->m_uID;

	m_AlbumPage.ShowSinger((CSinger*)pSingerObject);

	m_AlbumPage.Active(false);
	(void)this->SetFocus();
}

CMediaSet* CSingerPage::_trySelectObject()
{
	CPoint ptPos;
	GetCursorPos(&ptPos);
	m_wndTree.ScreenToClient(&ptPos);

	UINT uFlags = TVHT_NOWHERE;
	auto hItem = m_wndTree.HitTest(ptPos, &uFlags);
	__EnsureReturn(NULL != hItem, NULL);

	m_wndTree.SelectItem(hItem);

	CMediaSet *pSingerObject = (CMediaSet*)m_wndTree.GetItemObject(hItem);
	if (NULL != pSingerObject && E_MediaSetType::MST_Singer == pSingerObject->m_eType)
	{
		if (!::IsWindowVisible(m_AlbumPage))
		{
			m_AlbumPage.Active(false);
			(void)this->SetFocus();
		}
	}

	return pSingerObject;
}

void CSingerPage::OnNMDblclkTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	this->OnMenuCommand(ID_PLAY);
}

void CSingerPage::OnNMRclickTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	CMediaSet *pSingerObject = _trySelectObject();

	BOOL bSinger = (pSingerObject && E_MediaSetType::MST_Singer == pSingerObject->m_eType);
	m_MenuGuard.EnableItem(ID_ADD_IMAGE, bSinger);

	m_MenuGuard.EnableItem(ID_FIND, bSinger);

	m_MenuGuard.EnableItem(ID_RENAME, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_REMOVE, NULL != pSingerObject);

	m_MenuGuard.EnableItem(ID_DisableDemand, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_DisableExport, NULL != pSingerObject);

	m_MenuGuard.EnableItem(ID_CNLanguage, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_HKLanguage, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_KRLanguage, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_JPLanguage, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_TAILanguage, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_ENLanguage, NULL != pSingerObject);
	m_MenuGuard.EnableItem(ID_EURLanguage, NULL != pSingerObject);

	bool bAvalible = false;
	if (NULL != pSingerObject)
	{
		const CMediasetProperty& property = pSingerObject->property();

		m_MenuGuard.CheckItem(ID_DisableDemand, property.isDisableDemand());
		m_MenuGuard.CheckItem(ID_DisableExport, property.isDisableExport());

		m_MenuGuard.CheckItem(ID_CNLanguage, property.isCNLanguage());
		m_MenuGuard.CheckItem(ID_HKLanguage, property.isHKLanguage());
		m_MenuGuard.CheckItem(ID_KRLanguage, property.isKRLanguage());
		m_MenuGuard.CheckItem(ID_JPLanguage, property.isJPLanguage());
		m_MenuGuard.CheckItem(ID_TAILanguage, property.isTAILanguage());
		m_MenuGuard.CheckItem(ID_ENLanguage, property.isENLanguage());
		m_MenuGuard.CheckItem(ID_EURLanguage, property.isEURLanguage());

		bAvalible = pSingerObject->playable();
	}

	m_MenuGuard.EnableItem(ID_PLAY, bSinger && bAvalible);
	m_MenuGuard.EnableItem(ID_VERIFY, bAvalible);
	m_MenuGuard.EnableItem(ID_EXPORT, bAvalible);

	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);
}

void CSingerPage::OnTvnItemexpandedTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	CSingerGroup *pSingerGroup = (CSingerGroup*)m_wndTree.GetItemObject(pNMTreeView->itemNew.hItem);

	if (TVIS_EXPANDED & pNMTreeView->itemNew.state)
	{
		m_setExpandedGroupIDs.insert(pSingerGroup->m_uID);
	}
	else
	{
		m_setExpandedGroupIDs.erase(pSingerGroup->m_uID);
	}

	*pResult = 0;
}

BOOL CSingerPage::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		_trySelectObject();
	}

	return __super::PreTranslateMessage(pMsg);
}
