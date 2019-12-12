
#include "StdAfx.h"

#include "MediaResPanel.h"

#include "../dlg/TrackDetailDlg.h"

enum E_AlbumItemColumn
{
	__Column_Name = 0
	, __Column_Type
	, __Column_Size
	, __Column_Playlist
	, __Column_SingerAlbum
	, __Column_ModifyTime
};

list<CMediaResPanel*> CMediaResPanel::g_lstMediaResPanels;

void CMediaResPanel::RefreshMediaResPanel()
{
	for (auto pMediaResPanel : g_lstMediaResPanels)
	{
		pMediaResPanel->Refresh();
	}
}

CMediaResPanel::CMediaResPanel(__view& view, bool bShowRelatedSinger)
	: CBasePage(view, IDD_PAGE_MediaResPanel, L"", IDR_MENU_Dir, true)
	, m_FileMenuGuard(view.m_ResModule, IDR_MENU_File, __MenuWidth)
	, m_bShowRelatedSinger(bShowRelatedSinger)
	, m_wndList(bShowRelatedSinger)
{
}

BEGIN_MESSAGE_MAP(CMediaResPanel, CBasePage)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LIST1, &CMediaResPanel::OnNMDBblClkList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CMediaResPanel::OnNMRclickList)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CMediaResPanel::OnNMClickList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CMediaResPanel::OnLvnItemchangedList)
END_MESSAGE_MAP()

void CMediaResPanel::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST1, m_wndList);

	DDX_Control(pDX, IDC_STATIC1, m_wndStatic);

	CPage::DoDataExchange(pDX);
}

BOOL CMediaResPanel::Create(CBasePage& wndPage)
{
	m_view.m_ResModule.ActivateResource();
	__AssertReturn(__super::Create(m_lpszTemplateName, &wndPage), FALSE);

	return TRUE;
}

BOOL CMediaResPanel::OnInitDialog()
{
	(void)__super::OnInitDialog();

	_OnInitDialog();

	m_wndStatic.SetParent(&m_wndList);

	(void)m_wndList.ModifyStyle(0, LVS_EDITLABELS);

	m_wndList.SetImageList(&m_view.m_ImgMgr.getImglst(E_GlobalImglst::GIL_Big)
		, &m_view.m_ImgMgr.getImglst(E_GlobalImglst::GIL_Small));

	auto& globalSize = m_view.m_globalSize;
	
	UINT uColWidth_FileName = globalSize.m_ColWidth_Name;

	TD_ListColumn lstColumns = {
		{ _T("文件名称"), uColWidth_FileName }
		,{ _T("类型"), globalSize.m_ColWidth_Type, true }
		,{ _T("大小"), globalSize.m_ColWidth_FileSize, true }
		,{ _T("关联歌单"), globalSize.m_ColWidth_RelatedPlaylist, true }
	};
	
	if (m_bShowRelatedSinger)
	{
		lstColumns.push_back({L"歌手" __CNDot L"专辑", globalSize.m_ColWidth_RelatedSingerAlbum, true });
	}
	else
	{
		lstColumns.push_back({L"关联专辑", globalSize.m_ColWidth_RelatedPlaylist, true });
	}

	lstColumns.insert(lstColumns.end(), {
		{ _T("修改时间"), globalSize.m_ColWidth_Time, true }
	});

	m_Column_ID3 = lstColumns.size();
	lstColumns.insert(lstColumns.end(), {
		{ _T("标题"), globalSize.m_ColWidth_MediaTag_Title }
		,{ _T("艺术家"), globalSize.m_ColWidth_MediaTag_Artist }
		,{ _T("唱片集"), globalSize.m_ColWidth_MediaTag_Album }
		,{ _T("创建时间"), globalSize.m_ColWidth_Time, true }
	});

	CObjectList::tagListPara ListPara;
	ListPara.lstColumns = lstColumns;
	
	ListPara.nHeaderHeight = globalSize.m_uHeadHeight;
	ListPara.fHeaderFontSize = globalSize.m_fBigFontSize;

	ListPara.fFontSize = globalSize.m_fSmallFontSize;
	ListPara.crText = __Color_Text;
	
	ListPara.eViewType = m_bShowRelatedSinger ? E_ListViewType::LVT_Icon : E_ListViewType::LVT_Report;

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	m_wndList.SetCustomDraw([&](tagLVDrawSubItem& lvcd) {
		switch (lvcd.nSubItem)
		{
		case __Column_Name:

			break;
		case __Column_Playlist:
		case __Column_SingerAlbum:
			lvcd.bSetUnderline = true;

			break;
		default:
			lvcd.fFontSizeOffset = -.2f;

			break;
		};
	});

	m_wndList.SetViewAutoChange([&](E_ListViewType eViewType) {
		m_wndList.UpdateItems();

		m_uIconWidth = m_uTileWidth = 0;
		OnSize(0, m_cx, m_cy);

		_asyncTask();
	});
	
	(void)__super::RegDragableCtrl(m_wndList, [&](tagDragData& DragData) {
		TD_ListObjectList lstObjects;
		m_wndList.GetSelObjects(lstObjects);

		bool bRet = false;
		lstObjects.front([&](CListObject& ListObject) {
			__Ensure(!((CMediaRes&)ListObject).IsDir());

			DragData.lstMedias.add(lstObjects);

			DragData.iImage = (int)E_GlobalImage::GI_MediaFile;
			
			bRet = true;
		});

		return bRet;
	});

	(void)__super::RegDropableCtrl(m_wndList);

	__super::RegMenuHotkey(m_wndList, VK_RETURN, ID_OPEN);
	__super::RegMenuHotkey(m_wndList, VK_F2, ID_RENAME);
	__super::RegMenuHotkey(m_wndList, VK_DELETE, ID_DELETE);
	__super::RegMenuHotkey(m_wndList, VK_BACK, ID_Upward);

	g_lstMediaResPanels.push_back(this);

	return TRUE;
}

#define __XOffset_Tile 12
#define __YOffset_Tile 2

#define __XOffset_Icon 10
#define __YOffset_Icon 2

#define __XOffset_List 10
#define __YOffset_List 10

#define __XOffset_Report 6

void CMediaResPanel::OnSize(UINT nType, int cx, int cy)
{
	if (m_wndList)
	{
		m_nXOffset = 0;
		m_nYOffset = 0;

		E_ListViewType eViewType = m_wndList.GetView();
		if (E_ListViewType::LVT_Tile == eViewType)
		{
			m_nXOffset = __XOffset_Tile;
			m_nYOffset = __YOffset_Tile;
		}
		else if (E_ListViewType::LVT_Icon == eViewType)
		{
			m_nXOffset = __XOffset_Icon;
			m_nYOffset = __YOffset_Icon;
		}
		else if (E_ListViewType::LVT_List == eViewType)
		{
			m_nXOffset = __XOffset_List;
			m_nYOffset = __YOffset_List;
		}
		else if (E_ListViewType::LVT_Report == eViewType)
		{
			m_nXOffset = __XOffset_Report;
		}

		UINT width = (UINT)cx - m_nXOffset - m_view.m_globalSize.m_uScrollbarWidth;
		
		UINT uTileWidth = width / 4-1;
		if (uTileWidth > m_uTileWidth + 8 || uTileWidth < m_uTileWidth - 8)
		{
			m_uTileWidth = uTileWidth;

			m_wndList.SetTileSize(uTileWidth, m_view.m_globalSize.m_uTileHeight);
		}

		UINT uIconWidth = width / (m_bShowRelatedSinger?6:5)-1;
		if (uIconWidth > m_uIconWidth + 8 || uIconWidth < m_uIconWidth - 8)
		{
			m_uIconWidth = uIconWidth;

			m_wndList.SetIconSpacing(uIconWidth, m_view.m_globalSize.m_uBigIconSize + m_view.m_globalSize.m_uIconSpace);
		}

		CRect rcPos(m_nXOffset, m_nYOffset, cx, cy);
		m_wndList.MoveWindow(rcPos, cx >= m_cx);
	}

	m_cx = cx;
	m_cy = cy;
}

void CMediaResPanel::ShowPath(const wstring& strPath)
{
	CWaitCursor WaitCursor;

	m_strRootPath = strPath;

	CMediaDir *pRootPath = NULL;
	if (!strPath.empty())
	{
		pRootPath = m_view.getMediaLib().findSubDir(strPath);
	}
	else
	{
		pRootPath = &m_view.getMediaLib();
	}
	_showPath(pRootPath);
}

void CMediaResPanel::Refresh()
{
	CMediaDir *pRootPath = m_view.getMediaLib().findSubDir(m_strRootPath);
	CMediaDir *pCurrPath = m_view.getMediaLib().findSubDir(m_strCurrPath);
	if (NULL == pCurrPath)
	{
		pCurrPath = pRootPath;
	}

	_showPath(pRootPath, pCurrPath);

	/*if (nPrevTopItem > 0)
	{
		auto eViewType = m_wndList.GetView();
		if (E_ListViewType::LVT_Report == eViewType)
		{
			CRect rcItem;
			m_wndList.GetItemRect(nPrevTopItem, rcItem, 0);
			//m_wndList.Scroll({ 0, nPrevTopItem * rcItem.Height() });
		}
		else if (E_ListViewType::LVT_List == eViewType)
		{
			CPoint ptItem;
			m_wndList.GetItemPosition(nPrevTopItem, &ptItem);
			//m_wndList.Scroll({ ptItem.x, 0 });
		}
	}*/
}

void CMediaResPanel::_showPath(CMediaDir *pRootPath, CMediaDir *pCurrPath, CMediaRes *pHitestItem)
{
	__Ensure(m_hWnd);

	wstring strPrevCurrPath = m_strCurrPath;
	
	m_pCurrPath = m_pRootPath = pRootPath;
	m_strCurrPath = m_strRootPath;
	if (NULL == m_pCurrPath)
	{
		(void)m_wndStatic.ShowWindow(SW_SHOW);

		m_wndList.DeleteAllItems();

		return;
	}

	(void)m_wndStatic.ShowWindow(SW_HIDE);

	if (pCurrPath)
	{
		m_pCurrPath = pCurrPath;
		m_strCurrPath = pCurrPath->GetPath();
	}

	wstring strTitle;
	
	wstring strOppPath = fsutil::GetOppPath(m_strCurrPath, m_strRootPath);
	if (strOppPath.empty())
	{
		if (m_bShowRelatedSinger)
		{
			strTitle = L"媒体库";
		}
	}
	else
	{
		size_t uMaxTabTitle = __MaxTabTitle;
		if (!m_bShowRelatedSinger)
		{
			strTitle = __wcDirSeparator;
			uMaxTabTitle -= 5;
		}
		
		wstring strDirName = fsutil::getFileTitle(strOppPath);
		if (strDirName.size() > uMaxTabTitle)
		{
			strDirName = strDirName.substr(0, uMaxTabTitle) + L"...";
		}

		strTitle.append(strDirName);
	}

	if (m_bShowRelatedSinger)
	{
		strTitle.append(2, ' ');
	}

	UpdateTitle(strTitle);
	
	_showPath();

	if (NULL != pHitestItem)
	{
		m_wndList.SelectObject(pHitestItem);
	}
	else
	{
		if (m_strCurrPath != strPrevCurrPath)
		{
			m_wndList.EnsureVisible(0, FALSE);
		}
	}
}

void CMediaResPanel::_showPath()
{
	CWaitCursor WaitCursor;

	CRedrawLockGuard RedrawLockGuard(m_wndList);
	
	m_wndList.SetPath(*m_pCurrPath);
		
	cauto paSubDir = m_pCurrPath->dirs();
	if (m_bShowRelatedSinger && paSubDir)
	{
		map<wstring, pair<UINT, wstring>> mapSingerInfo;
		m_view.getSingerMgr().GetSinger(m_pCurrPath->GetPath(), mapSingerInfo);
		
		paSubDir([&](CPath& subDir, size_t uIdx) {
			auto itr = mapSingerInfo.find(subDir.name());

			auto& MediaRes = ((CMediaRes&)subDir);
			if (itr != mapSingerInfo.end())
			{
				MediaRes.SetRelatedMediaSet(E_MediaSetType::MST_Singer, itr->second.first, itr->second.second);
			}
			else
			{
				MediaRes.ClearRelatedMediaSet(E_MediaSetType::MST_Singer);
			}

			m_wndList.UpdateItem(uIdx, &MediaRes);
		});
	}

	_asyncTask();
}

BOOL CMediaResPanel::HittestMediaRes(IMedia& media, CWnd& wnd)
{
	CMediaRes *pMediaRes = m_view.getMediaLib().findSubPath(media);
	if (NULL == pMediaRes)
	{
		cauto strDir = fsutil::GetParentDir(media.GetPath());
		if (!strDir.empty())
		{
			CMediaDir *pMediaDir = m_view.getMediaLib().findSubDir(strDir);
			if (pMediaDir)
			{
				pMediaDir->clear();
				pMediaRes = (CMediaRes*)pMediaDir->findSubFile(media.GetName());
			}
		}
	}
	
	if (NULL == pMediaRes)
	{
		CMainApp::showMsg(L"未定位到曲目: \n\n\t" + media.GetPath(), &wnd);
		return FALSE;
	}

	HittestMediaRes(*pMediaRes);

	return TRUE;
}

void CMediaResPanel::HittestMediaRes(CMediaRes& MediaRes)
{
	_showPath(m_pRootPath, MediaRes.parent(), &MediaRes);

	if (m_bShowRelatedSinger)
	{
		this->Active();
	}
}

void CMediaResPanel::UpdateRelated(const tagMediaSetChanged& MediaSetChanged)
{
	__Ensure(m_pCurrPath);

	if (m_bShowRelatedSinger)
	{
		if (E_MediaSetType::MST_Singer == MediaSetChanged.eMediaSetType)
		{
			(void)m_pCurrPath->UpdateRelatedMediaSet(MediaSetChanged);
		}
	}
	
	UINT uIdx = 0;
	m_pCurrPath->subMediaRes([&](CMediaRes& MediaRes){
		if (MediaRes.UpdateRelatedMediaSet(MediaSetChanged))
		{
			m_wndList.UpdateItem(uIdx, &MediaRes);
		}

		uIdx++;
	});
}

void CMediaResPanel::OnMenuCommand(UINT uID, UINT uVkKey)
{
	TD_ListObjectList lstObjects;
	m_wndList.GetSelObjects(lstObjects);

	TD_MediaResList lstMediaRes(lstObjects);

	switch (uID)
	{
	case ID_Upward:
		__Ensure(m_pCurrPath && m_pCurrPath != m_pRootPath);
	
		_showPath(m_pRootPath, m_pCurrPath->parent(), m_pCurrPath);
		
		break;
	case ID_OPEN:
		if (0==uVkKey && !lstMediaRes)
		{
			TD_ListObjectList lstAllObject;
			m_wndList.GetAllObjects(lstAllObject);
			TD_MediaResList lsAllMediaRes(lstAllObject);
			lsAllMediaRes([&](CMediaRes& MediaRes){
				if (!MediaRes.IsDir())
				{
					lstMediaRes.add(MediaRes);
				}
			});
		}

		lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.IsDir())
			{
				_showPath(m_pRootPath, (CMediaDir*)&MediaRes);
			}
			else
			{
				m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(lstMediaRes));
			}
		});

		break;
	case ID_EXPORT:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.IsDir())
			{
				m_view.exportDir((CMediaDir&)MediaRes);
			}
		}))
		{
			if (NULL != m_pCurrPath)
			{
				m_view.exportDir(*m_pCurrPath);
	}	}

		break;
	case ID_Snapshot:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.IsDir())
			{
				m_view.snapshotDir((CMediaDir&)MediaRes);
			}
		}))
		{
			if (NULL != m_pCurrPath)
			{
				m_view.snapshotDir(*m_pCurrPath);
			}
		}

		break;
	case ID_CopyTitle:
		lstMediaRes.front([&](CMediaRes& MediaRes) {
			(void)m_view.copyMediaTitle(MediaRes);
		});

		break;
	case ID_EXPLORE:
		if (lstMediaRes.size()==1)
		{
			lstMediaRes.front([&](CMediaRes& MediaRes) {
				MediaRes.ShellExplore(); 
			});
		}
		else if (NULL != m_pCurrPath)
		{
			m_pCurrPath->ShellExplore(false);
		}

		break;
	case ID_FIND:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			m_view.findMedia(MediaRes.GetPath(), MediaRes.IsDir());
		}))
		{
			if (NULL != m_pCurrPath)
			{
				m_view.findMedia(m_pCurrPath->GetPath(), true);
			}
		}

		break;
	case ID_CheckSimilar:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.IsDir())
			{
				MediaRes.clear();
				CMediaResPanel::RefreshMediaResPanel();

				m_view.checkSimilarFile((CMediaDir&)MediaRes);
			}
		}))
		{
			if (NULL != m_pCurrPath)
			{
				m_pCurrPath->clear();
				CMediaResPanel::RefreshMediaResPanel();

				m_view.checkSimilarFile(*m_pCurrPath);
			}
		}

		break;
	case ID_RENAME:
		__EnsureBreak(1 == lstObjects.size());

		lstObjects.front([&](CListObject& ListObject) {
			(void)m_wndList.EditLabel(m_wndList.GetObjectItem(&ListObject));
		});
		
		break;
	case ID_DELETE:
		__Ensure(m_pCurrPath);

		lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (!MediaRes.IsDir())
			{
				__Ensure(CMainApp::showConfirmMsg(L"确认删除所选文件?", *this));

				CWaitCursor WaitCursor;
				(void)m_view.getController().removeMediaRes(lstMediaRes);
			}
		});

		break;
	case ID_Detach:
		lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.IsDir())
			{
				if (MediaRes.parent() == NULL)
				{
					m_view.getModel().detachDir(MediaRes.GetAbsPath());
				}
			}
		});
		
		break;
	case ID_Attach:
		attachDir();

		break;
	case ID_SETALARMCLOCK:
		__AssertBreak(lstMediaRes);

		m_view.getDataMgr().addAlarmmedia(TD_IMediaList(lstMediaRes));

		break;
	case ID_ViewTrack:
		lstMediaRes.front([&](CMediaRes& MediaRes) {
			CRCueFile cueFile = MediaRes.getCueFile();
			if (cueFile)
			{
				CTrackDetailDlg(m_view, cueFile, &MediaRes).DoModal();
				return;
			}
		});

		break;
	case ID_REFRESH:
		if (NULL != m_pCurrPath)
		{
			m_pCurrPath->clear();
			CMediaResPanel::RefreshMediaResPanel();
		}
		else
		{
			m_view.getModel().refreshMediaLib();
		}

		break;
	}
}

//void CMediaResPanel::OnDeleteDir(CMediaRes& dir)
//{
//	CMediaRes *pTargetPath = m_pCurrPath;
//	while(TRUE)
//	{
//		if (m_pCurrPath == &dir || fsutil::CheckSubPath(dir.GetPath(), m_strCurrPath))
//		{
//			if (pTargetPath == m_pRootPath)
//			{
//				break;
//			}
//			
//			pTargetPath = m_pRootPath;
//			continue;
//		}
//
//		return;
//	}
//
//	ShowPath(NULL);
//}

void CMediaResPanel::OnNMDBblClkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	if (NULL == m_pCurrPath)
	{
		return;
	}

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem >= 0)
	{
		m_wndList.DeselectAll();		
		m_wndList.SelectItem(pNMLV->iItem);
		
		CMediaRes *pMediaRes = (CMediaRes*)m_wndList.GetItemObject(pNMLV->iItem);

		this->OnMenuCommand(ID_OPEN);

		if (NULL != pMediaRes)
		{
			CRCueFile cueFile = pMediaRes->getCueFile();
			if (cueFile)
			{
				CTrackDetailDlg(m_view, cueFile, pMediaRes).DoModal();
			}
		}
	}
	else
	{
		this->OnMenuCommand(ID_Upward);
	}
}

void CMediaResPanel::OnNMRclickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem >= 0)
	{
		TD_ListObjectList lstObjects;
		m_wndList.GetSelObjects(lstObjects);

		CListObject *pObject = m_wndList.GetItemObject(pNMLV->iItem);
		if (!lstObjects.includes(pObject))
		{
			m_wndList.DeselectAll();
			m_wndList.SelectItem(pNMLV->iItem);

			lstObjects.assign(pObject);
		}
		
		TD_MediaResList lstMediaRes(lstObjects);
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.IsDir())
			{
				_showDirMenu((CMediaDir*)&MediaRes);
			}
			else
			{
				_showFileMenu(lstMediaRes);
			}
		}))
		{
			_showDirMenu(NULL);
		}
	}
	else
	{
		m_wndList.DeselectAll();

		_showDirMenu(NULL);
	}
}

void CMediaResPanel::_showDirMenu(CMediaDir *pSubDir)
{
	m_MenuGuard.EnableItem({ ID_OPEN, ID_FIND, ID_CopyTitle, ID_RENAME, ID_DELETE, ID_Detach, ID_Attach }, FALSE);

	if (NULL != pSubDir)
	{
		m_MenuGuard.SetItemText(ID_OPEN, _T("打开"));
		m_MenuGuard.EnableItem(ID_OPEN, TRUE);

		m_MenuGuard.EnableItem(ID_FIND, TRUE);

		m_MenuGuard.EnableItem(ID_CopyTitle, TRUE);

		if (pSubDir->parent() != NULL)
		{
			m_MenuGuard.EnableItem(ID_RENAME, TRUE);
		}
		else
		{
			m_MenuGuard.EnableItem(ID_Detach, TRUE);
		}
	}
	else
	{
		m_MenuGuard.SetItemText(ID_OPEN, _T("播放"));

		if (m_pCurrPath)
		{
			m_MenuGuard.EnableItem(ID_OPEN, m_pCurrPath->files());
		}

		m_MenuGuard.EnableItem(ID_FIND, m_pCurrPath && m_pCurrPath != &m_view.getMediaLib());

		bool bFlag = m_wndList.GetItemCount()>0;
		m_MenuGuard.EnableItem(ID_EXPORT, m_pCurrPath && bFlag);
		m_MenuGuard.EnableItem(ID_Snapshot, m_pCurrPath && bFlag);
		m_MenuGuard.EnableItem(ID_CheckSimilar, m_pCurrPath && bFlag);

		m_MenuGuard.EnableItem(ID_EXPLORE, m_pCurrPath);
		
		if (&m_view.getMediaLib() == m_pCurrPath)
		{
			m_MenuGuard.EnableItem(ID_Attach, TRUE);
		}
	}

	m_MenuGuard.EnableItem(ID_Upward, m_pCurrPath&& m_pCurrPath != m_pRootPath);

	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMenuFontSize);
}

void CMediaResPanel::_showFileMenu(TD_MediaResList& lstMediaRes)
{
	UINT lpIDMenuItem[] = { ID_FIND, ID_ViewTrack, ID_CopyTitle, ID_RENAME };
	for (UINT uIndex = 0; uIndex < sizeof(lpIDMenuItem) / sizeof(lpIDMenuItem[0]); uIndex++)
	{
		m_FileMenuGuard.EnableItem(lpIDMenuItem[uIndex], FALSE);
	}

	if (1 == lstMediaRes.size())
	{
		m_FileMenuGuard.EnableItem(ID_FIND, TRUE);

		lstMediaRes.front([&](CMediaRes& subFile) {
			if (subFile.getCueFile())
			{
				m_FileMenuGuard.EnableItem(ID_ViewTrack, TRUE);
			}
		});

		m_FileMenuGuard.EnableItem(ID_CopyTitle, TRUE);

		m_FileMenuGuard.EnableItem(ID_RENAME, TRUE);
	}

	m_FileMenuGuard.EnableItem(ID_Upward, m_pCurrPath != m_pRootPath);
	
	(void)m_FileMenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMenuFontSize);
}

void CMediaResPanel::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	int iSubItem = lpNMList->iSubItem;

	if (__Column_Playlist == iSubItem || __Column_SingerAlbum == iSubItem)
	{
		int iItem = lpNMList->iItem;

		m_wndList.AsyncLButtondown([=]() {
			CMediaRes* pMediaRes = (CMediaRes*)m_wndList.GetItemObject(iItem);
			__Ensure(pMediaRes);

			if (!pMediaRes->IsDir())
			{
				pMediaRes->AsyncTask();
				m_wndList.UpdateItem(iItem, pMediaRes);
			}

			if (__Column_Playlist == iSubItem)
			{
				m_view.hittestRelatedMediaSet(*pMediaRes, E_MediaSetType::MST_Playlist);
			}
			else
			{
				if (!m_view.hittestRelatedMediaSet(*pMediaRes, E_MediaSetType::MST_Album))
				{
					if (m_bShowRelatedSinger)
					{
						(void)m_view.hittestRelatedMediaSet(*pMediaRes, E_MediaSetType::MST_Singer);
					}
				}
			}
		});
	}
}

void CMediaResPanel::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	__Ensure(LVIF_STATE & pNMLV->uChanged);

	if ((pNMLV->uOldState & LVIS_SELECTED) != (pNMLV->uNewState & LVIS_SELECTED))
	{
		map<UINT, CListObject*> mapObjects;
		m_wndList.GetSelObjects(mapObjects);

		__Ensure(mapObjects.size() > 1);

		bool bHasFile = false;
		for (auto& pr : mapObjects)
		{
			if (!((CMediaRes*)pr.second)->IsDir())
			{
				bHasFile = true;
				break;
			}
		}

		if (bHasFile)
		{
			for (auto& pr : mapObjects)
			{
				if (((CMediaRes*)pr.second)->IsDir())
				{
					(void)m_wndList.SetItemState(pr.first, 0, LVIS_SELECTED | LVIS_FOCUSED);
				}
			}
		}
		else
		{
			if (pNMLV->uNewState & LVIS_SELECTED)
			{
				for (auto& pr : mapObjects)
				{
					if (pr.first != pNMLV->iItem)
					{
						(void)m_wndList.SetItemState(pr.first, 0, LVIS_SELECTED | LVIS_FOCUSED);
					}
				}
			}
		}
	}
}

DROPEFFECT CMediaResPanel::OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	__EnsureReturn(m_pCurrPath && lstMedias, DROPEFFECT_NONE);

	bool bScroll = DragScroll(m_wndList, DragContext.x, DragContext.y);

	__EnsureReturn(DragContext.y > (int)m_wndList.GetHeaderHeight(), DROPEFFECT_NONE);

	DROPEFFECT dwRet = DROPEFFECT_NONE;

	lstMedias.front([&](IMedia& media) {
		if (E_MediaSetType::MST_Null != media.GetMediaSetType())
		{
			dwRet = DROPEFFECT_MOVE;
		}
	});

	DragContext.pTargetObj = m_pCurrPath;

	int nItem = m_wndList.HitTest(CPoint(DragContext.x, DragContext.y));
	if (nItem >= 0)
	{
		CMediaRes *pDragOverPath = (CMediaRes*)m_wndList.GetItemObject(nItem);
		if (pDragOverPath && pDragOverPath->IsDir())
		{
			if (!bScroll)
			{
				CRect rcItem;
				(void)m_wndList.GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
				DragContext.DrawDragOverRect(rcItem);
			}

			DragContext.pTargetObj = pDragOverPath;

			dwRet = DROPEFFECT_MOVE;
		}
	}

	return dwRet;
}

BOOL CMediaResPanel::OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	CWaitCursor WaitCursor;

	CMediaRes *pDstDir = (CMediaRes*)DragContext.pTargetObj;
	__AssertReturn(pDstDir, FALSE);

	(void)m_view.getController().moveMediaFile(lstMedias, pDstDir->GetPath());

	return TRUE;
}

int CMediaResPanel::GetTabImage()
{
	if (NULL != m_pCurrPath && m_pCurrPath != &m_view.getMediaLib())
	{
		if (m_pCurrPath->parent() == NULL)
		{
			return (int)E_GlobalImage::GI_DirLink;
		}
	}

	return (int)E_GlobalImage::GI_Dir;
}

void CMediaResPanel::_asyncTask()
{
	if (m_wndList.isReportView())
	{
		m_wndList.AsyncTask(200, [](CListObject& object) {
			CMediaRes& mediaRes = (CMediaRes&)object;
			if (!mediaRes.IsDir())
			{
				mediaRes.AsyncTask();
				return true;
			}

			return false;
		});
	}
}

void CMediaResPanel::attachDir()
{
	while (true)
	{
		static CFolderDlgEx FolderDlg;
		wstring strDir = FolderDlg.Show(L"挂载目录", L"请选择需要挂载的目录");
		if (strDir.empty())
		{
			break;
		}

		CMediaDir *pMediaDir = m_view.getController().attachDir(strDir);
		if (NULL != pMediaDir)
		{
			this->HittestMediaRes(*pMediaDir);
			break;
		}
	}
}
