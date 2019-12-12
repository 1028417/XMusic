
#include "StdAfx.h"

#include "PlayItemPage.h"

#include "PlayingPage.h"

enum E_PlayItemColumn
{
	__Column_Name = 0
	, __Column_Info
	, __Column_SingerAlbum
	, __Column_Path
	, __Column_AddTime
};

#define __ColumnText_Name CString(_T("歌单曲目"))

CPlayItemPage::CPlayItemPage(__view& view)
	: CBasePage(view, IDD_PAGE_PLAYITEM, L" 歌单", IDR_MENU_PLAYITEM, true)
{
}

BEGIN_MESSAGE_MAP(CPlayItemPage, CBasePage)
	ON_WM_SIZE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CPlayItemPage::OnNMRclickList)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CPlayItemPage::OnNMClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CPlayItemPage::OnNMDblclkList)
END_MESSAGE_MAP()

void CPlayItemPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);

	CPage::DoDataExchange(pDX);
}

#define __XOffset 6

BOOL CPlayItemPage::OnInitDialog()
{
	__super::OnInitDialog();

	(void)m_wndList.ModifyStyle(0, LVS_EDITLABELS);

	m_wndList.SetImageList(NULL, &m_view.m_ImgMgr.getImglst(E_GlobalImglst::GIL_Small));

	auto& globalSize = m_view.m_globalSize;

	CRect rcClient;
	m_view.m_MainWnd.GetClientRect(rcClient);

	CListColumnGuard ColumnGuard(rcClient.Width() - globalSize.m_uLeftDockWidth - 5
		- __XOffset - globalSize.m_uScrollbarWidth);
	ColumnGuard.addDynamic(__ColumnText_Name, 0.63)
		.addFix(L"类型/大小/时长", globalSize.m_ColWidth_Type + globalSize.m_ColWidth_FileSize, true)
		.addFix(L"歌手" __CNDot L"专辑", globalSize.m_ColWidth_RelatedSingerAlbum, true)
		.addDynamic(_T("目录"), 0.37)
		.addFix(_T("加入时间"), globalSize.m_ColWidth_AddTime, true);

	CObjectList::tagListPara ListPara(ColumnGuard);

	ListPara.nHeaderHeight = 0; //globalSize.m_uHeadHeight;
	//ListPara.fHeaderFontSize = globalSize.m_fBigFontSize;

	ListPara.fFontSize = globalSize.m_fSmallFontSize;
	ListPara.crText = __Color_Text;

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	m_wndList.SetCustomDraw([&](tagLVDrawSubItem& lvcd) {
		CPlayItem *pPlayItem = (CPlayItem *)lvcd.pObject;
		if (NULL == pPlayItem)
		{
			return;
		}

		if (pPlayItem->duration() == 0)
		{
			lvcd.setTextAlpha(128);
		}
		
		switch (lvcd.nSubItem)
		{
		case __Column_Info:
		{
			CDC& dc = lvcd.dc;
			cauto rc = lvcd.rc;
			dc.FillSolidRect(&rc, lvcd.crBkg);

			dc.SetTextColor(lvcd.crText);
			m_wndList.SetCustomFont(dc, -.2f, false);
			RECT rcText = rc;
			rcText.right = rcText.left + globalSize.m_ColWidth_Type;

			dc.DrawText(pPlayItem->GetFileTypeString().c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			rcText.left = rcText.right;
			rcText.right = rc.right;
			rcText.bottom = (rcText.bottom + rcText.top) / 2;
			dc.DrawText(pPlayItem->GetFileSizeString().c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			rcText.top = rcText.bottom;
			rcText.bottom = rc.bottom;
			dc.DrawText(pPlayItem->GetDurationString().c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		lvcd.bSkipDefault = true;

		break;
		case __Column_SingerAlbum:
			lvcd.bSetUnderline = true;
			lvcd.fFontSizeOffset = -.15f;

			break;
		case __Column_Path:
			lvcd.bSetUnderline = true;
			lvcd.fFontSizeOffset = -.2f;
			
			break;
		case __Column_AddTime:
			lvcd.fFontSizeOffset = -.2f;

			break;
		default:
			break;
		}
	});

	(void)__super::RegDragDropCtrl(m_wndList, [&](tagDragData& DragData) {
		TD_ListObjectList lstObjects;
		m_wndList.GetSelObjects(lstObjects);
		__EnsureReturn(lstObjects, false);

		DragData.lstMedias.add(lstObjects);

		DragData.iImage = (int)E_GlobalImage::GI_PlayItem;

		return true;
	});

	__super::RegMenuHotkey(m_wndList, VK_RETURN, ID_PLAY);
	__super::RegMenuHotkey(m_wndList, VK_F2, ID_RENAME);
	__super::RegMenuHotkey(m_wndList, VK_DELETE, ID_REMOVE);

	return TRUE;
}

void CPlayItemPage::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndList)
	{
		m_wndList.MoveWindow(__XOffset,0,cx- __XOffset,cy);
	}
}

void CPlayItemPage::ShowPlaylist(CPlaylist *pPlaylist, bool bSetActive)
{
	if (!m_hWnd)
	{
		if (!pPlaylist)
		{
			return;
		}

		m_view.m_ResModule.ActivateResource();
		__Assert(m_view.m_MainWnd.AddPage(*this, E_DockViewType::DVT_DockCenter));
		__Assert(__super::Active());
	}
	else if (bSetActive)
	{
		__Assert(__super::Active());
	}

	bool bChanged = (pPlaylist != m_pPlaylist);

	m_pPlaylist = pPlaylist;

	this->UpdateTitle();

	if (!m_pPlaylist)
	{
		(void)m_wndList.DeleteAllItems();

		this->UpdateHead();

		return;
	}

	(void)m_wndList.SetObjects(TD_ListObjectList(m_pPlaylist->playItems()));
	
	if (bChanged)
	{
		m_wndList.EnsureVisible(0, FALSE);
	}

	this->UpdateHead();

	m_wndList.AsyncTask(100, [](CListObject& object) {
		((CPlayItem&)object).AsyncTask();
		return true;
	});
}

void CPlayItemPage::UpdateTitle()
{
	wstring strTitle;
	if (NULL != m_pPlaylist)
	{
		wstring strPlaylistName = m_pPlaylist->m_strName;
		if (strPlaylistName.size() > __MaxTabTitle)
		{
			strPlaylistName = strPlaylistName.substr(0, __MaxTabTitle) + L"...";
		}

		strTitle = __CNDot + strPlaylistName + L" ";
	}
	(void)this->SetTitle(strTitle);
}

void CPlayItemPage::UpdateHead()
{
	m_view.m_PlaylistPage.Invalidate();

	/*wstring strColumnText = strutil::wcSpace + __ColumnText_Name;

	int iItemCount = m_wndList.GetItemCount();
	if (iItemCount > 0)
	{
		strColumnText.append(L"(").append(to_wstring(iItemCount)).append(L")");
	}
	
	m_wndList.SetColumnText(0, strColumnText);*/
}

/*void CPlayItemPage::removeItem(set<CListObject*>& setPlayItemObjects)
{
	m_wndList.DeleteObjects(setPlayItemObjects);
	
	UpdateHead();
	
	CMainApp::GetMainApp()->DoEvents();
	
	m_wndList.UpdateColumn(0);
}*/

void CPlayItemPage::UpdateRelated(const tagMediaSetChanged& MediaSetChanged)
{
	__Ensure(m_pPlaylist);

	m_pPlaylist->playItems()([&](CPlayItem& PlayItem, size_t uIdx){
		if (PlayItem.UpdateRelatedMediaSet(MediaSetChanged))
		{
			m_wndList.UpdateItem(uIdx, &PlayItem);
		}
	});
}

void CPlayItemPage::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_MenuGuard.EnableItem(ID_PLAY, (m_pPlaylist && m_pPlaylist->available()));

	int nCount = m_wndList.GetSelectedCount();
	m_MenuGuard.EnableItem(ID_FIND, (1 == nCount));
	m_MenuGuard.EnableItem(ID_HITTEST, (1 == nCount));
	m_MenuGuard.EnableItem(ID_SETALARMCLOCK, (nCount > 0));

	m_MenuGuard.EnableItem(ID_CopyTitle, (1 == nCount));
	m_MenuGuard.EnableItem(ID_EXPLORE, (1 == nCount));
	m_MenuGuard.EnableItem(ID_RENAME, (1 == nCount));
	m_MenuGuard.EnableItem(ID_REMOVE, (nCount > 0));

	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMenuFontSize);
}

void CPlayItemPage::OnMenuCommand(UINT uID, UINT uVkKey)
{
	TD_ListObjectList lstObjects;
	m_wndList.GetSelObjects(lstObjects);

	TD_MediaList lstPlayItems(lstObjects);

	switch (uID)
	{
	case ID_ADD_PLAYITEM:
	{
		__Ensure(m_pPlaylist);

		tagFileDlgOpt FileDlgOpt;
		FileDlgOpt.strTitle = L"添加歌单曲目";
		FileDlgOpt.strFilter = __MediaFilter;
		FileDlgOpt.strInitialDir = m_view.getMediaLib().GetAbsPath();
		CFileDlgEx fileDlg(FileDlgOpt);

		list<wstring> lstFiles;
		wstring strDir = fileDlg.ShowOpenMulti(lstFiles);		
		__Ensure(!lstFiles.empty());

		int iRet = m_view.getController().addPlayItems(lstFiles, *m_pPlaylist);
		if (iRet > 0)
		{
			m_wndList.SelectItems(0, (UINT)iRet);
		}
	}
	
	break;
	case ID_PLAY:
		if (lstPlayItems)
		{
			m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(lstPlayItems));
		}
		else
		{
			if (0 == uVkKey)
			{
				__AssertBreak(m_pPlaylist);
				m_view.m_PlayCtrl.addPlayingItem(*m_pPlaylist);
			}
		}

		break;
	case ID_FIND:
		lstPlayItems.front([&](CMedia& media) {
			m_view.showFindDlg(media.GetTitle(), false);
		});

		break;
	case ID_HITTEST:
		lstPlayItems.front([&](CMedia& media) {
			m_view.m_MediaResPage.HittestMediaRes(media, *this);
		});

		break;
	case ID_SETALARMCLOCK:
	{
		__AssertBreak(lstPlayItems);

		TD_IMediaList lstMedias(lstPlayItems);
		m_view.getDataMgr().addAlarmmedia(lstMedias);
	}

	break;
	case ID_CopyTitle:
		lstPlayItems.front([&](CMedia& media) {
			(void)m_view.copyMediaTitle(media);
		});

		break;
	case ID_EXPLORE:
		if (lstPlayItems.size() == 1)
		{
			lstPlayItems.front([](CMedia& media) {
				media.ShellExplore();
			});
		}

		break;
	case ID_REMOVE:
		__EnsureBreak(lstPlayItems);

		__EnsureBreak(CMainApp::showConfirmMsg(L"确认移除选中的曲目?", *this));

		__EnsureBreak(m_view.getModel().removeMedia(lstPlayItems));

		break;	
	case ID_RENAME:
		__EnsureBreak(1 == lstPlayItems.size());

		lstPlayItems.front([&](CMedia& media) {
			int nItem = m_wndList.GetObjectItem(&media);
			__Ensure(nItem >= 0);
			m_wndList.EditLabel(nItem);
		});

		break;
	}
}

DROPEFFECT CPlayItemPage::OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	__AssertReturn(m_pPlaylist && pwndCtrl == &m_wndList && lstMedias, DROPEFFECT_NONE);
	
	handleDragOver(m_wndList, DragContext);

	DROPEFFECT dwRet = DROPEFFECT_COPY;
	lstMedias.front([&](IMedia& media) {
		if (media.GetMediaSet() == m_pPlaylist)
		{
			dwRet = DROPEFFECT_MOVE;
		}
	});
	
	return dwRet;
}

BOOL CPlayItemPage::OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	UINT uTargetPos = DragContext.uTargetPos;
	
	__AssertReturn(pwndCtrl == &m_wndList && lstMedias, FALSE);

	int nNewPos = 0;

	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.GetMediaSet();
	});

	if (pSrcMediaSet != m_pPlaylist)
	{
		__EnsureReturn(m_view.getPlaylistMgr().AddPlayItems(lstMedias, *m_pPlaylist, uTargetPos), FALSE);

		nNewPos = uTargetPos;
	}
	else
	{
		list<UINT> lstSelectedItems;
		m_wndList.GetSelItems(lstSelectedItems);
		__AssertReturn(!lstSelectedItems.empty(), FALSE);

		if (uTargetPos >= lstSelectedItems.front() && uTargetPos <= lstSelectedItems.back() + 1)
		{
			if (lstSelectedItems.back() - lstSelectedItems.front() + 1 == lstSelectedItems.size())
			{
				return FALSE;
			}
		}

		nNewPos = m_view.getPlaylistMgr().RepositPlayItem(*m_pPlaylist, lstMedias, uTargetPos);
		__EnsureReturn(nNewPos >= 0, FALSE);

		this->ShowPlaylist(m_pPlaylist);
	}	

	m_wndList.SelectItems(nNewPos, (int)lstMedias.size());

	return TRUE;
}

void CPlayItemPage::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	int iSubItem = lpNMList->iSubItem;

	if (__Column_SingerAlbum == iSubItem || __Column_Path == iSubItem)
	{
		int iItem = lpNMList->iItem;

		m_wndList.AsyncLButtondown([=]() {
			CMedia *pPlayItem = (CMedia*)m_wndList.GetItemObject(iItem);
			__Ensure(pPlayItem);
			pPlayItem->AsyncTask();
			m_wndList.UpdateItem(iItem, pPlayItem);

			if (__Column_SingerAlbum == iSubItem)
			{
				if (!m_view.hittestRelatedMediaSet(*pPlayItem, E_MediaSetType::MST_Album))
				{
					(void)m_view.hittestRelatedMediaSet(*pPlayItem, E_MediaSetType::MST_Singer);
				}
			}
			else
			{
				this->OnMenuCommand(ID_HITTEST);
			}
		});
	}
}

void CPlayItemPage::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(lpNMList->iItem >= 0);

	m_wndList.DeselectAll();
	m_wndList.SelectItem(lpNMList->iItem);

	this->OnMenuCommand(ID_PLAY);
}

void CPlayItemPage::HittestMedia(CMedia *pMedia)
{
	m_wndList.DeselectAll();
	m_wndList.SelectObject(pMedia);

	(void)Active();
	//(void)SetFocus();
}
