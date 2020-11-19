
#include "StdAfx.h"

#include "MediaResPanel.h"

#include "../dlg/TrackDetailDlg.h"

bool CMediaResList::OnItemRename(UINT uItem, CListObject *pObject, const CString& cstrNewText)
{
	return m_MediaResPanel.OnListItemRename(uItem, pObject, wstring(cstrNewText));
}

enum E_AlbumItemColumn
{
	__Column_Name = 0
	, __Column_Info
	, __Column_Playlist
	, __Column_SingerAlbum
	, __Column_ModifyTime
};

static list<CMediaResPanel*> g_lstMediaResPanels;

void CMediaResPanel::RefreshMediaResPanel()
{
	for (auto pMediaResPanel : g_lstMediaResPanels)
	{
		pMediaResPanel->Refresh();
	}
}

CMediaResPanel::CMediaResPanel(__view& view, bool bSingerPanel)
	: CBasePage(view, IDD_PAGE_MediaResPanel, L"", IDR_MENU_Dir, true)
	, m_FileMenuGuard(view.m_ResModule, IDR_MENU_File, __MenuWidth)
	, m_bSingerPanel(bSingerPanel)
	, m_wndList(*this, bSingerPanel)
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
	
	(void)m_wndList.ModifyStyle(0, LVS_EDITLABELS);

	m_wndList.SetImageList(&m_view.m_ImgMgr.bigImglst(), &m_view.m_ImgMgr.smallImglst());

	auto& globalSize = m_view.m_globalSize;
	
	UINT uColWidth_FileName = globalSize.m_ColWidth_Name;

	TD_ListColumn lstColumns {
		{ _T("文件名称"), uColWidth_FileName }
		,{ _T("类型"), globalSize.m_ColWidth_FileSize, true }
		,{ _T("关联歌单"), globalSize.m_ColWidth_RelatedPlaylist, true }
	};
	
	if (m_bSingerPanel)
	{
		lstColumns.push_back({ L"关联专辑", globalSize.m_ColWidth_RelatedPlaylist, true });
	}
	else
	{
		lstColumns.push_back({ L"歌手" __CNDot L"专辑", globalSize.m_ColWidth_RelatedSingerAlbum, true });
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

	CObjectList::tagListPara ListPara(lstColumns, globalSize.m_uHeadHeight, globalSize.m_fMidFontSize);

	ListPara.fFontSize = globalSize.m_fSmallFontSize;
	ListPara.crText = __Color_Text;
	
	ListPara.eViewType = m_bSingerPanel ? E_ListViewType::LVT_Report : E_ListViewType::LVT_Icon;

	__AssertReturn(m_wndList.InitCtrl(ListPara), FALSE);

	m_wndList.SetCustomDraw([&](tagLVDrawSubItem& lvcd) {
		switch (lvcd.nSubItem)
		{
		case __Column_Info:
		{
			CMediaRes *pMediaRes = (CMediaRes*)lvcd.pObject;
			__EnsureBreak(pMediaRes && !pMediaRes->isDir());

			CDC& dc = lvcd.dc;
			cauto rc = lvcd.rc;
			dc.FillSolidRect(&rc, lvcd.crBkg);

			dc.SetTextColor(lvcd.crText);
			m_wndList.SetCustomFont(dc, -.2f, false);
			RECT rcText = rc;
			rcText.bottom = (rcText.bottom + rcText.top)/2 +6;

			dc.DrawText(pMediaRes->GetExtName().c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			rcText.top = rcText.bottom -9;
			rcText.bottom = rc.bottom;
			dc.DrawText(pMediaRes->fileSizeString(true).c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		lvcd.bSkipDefault = true;
		
		break;
		case __Column_Playlist:
		case __Column_SingerAlbum:
		{
			CMediaRes *pMediaRes = (CMediaRes*)lvcd.pObject;
			__EnsureBreak(pMediaRes);

			WString strMediaset;
			if (__Column_Playlist == lvcd.nSubItem)
			{
				__EnsureBreak(!pMediaRes->isDir());
				strMediaset = pMediaRes->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Playlist);
			}
			else
			{
				if (pMediaRes->isDir())
				{
					strMediaset = pMediaRes->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
				}
				else
				{
					if (m_bSingerPanel)
					{
						strMediaset = pMediaRes->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Album);
					}
					else
					{
						cauto strAlbum = pMediaRes->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Album);
						if (!strAlbum.empty())
						{
							cauto strSinger = pMediaRes->GetRelatedMediaSetName(E_RelatedMediaSet::RMS_Singer);
							strMediaset << strSinger << __CNDot << strAlbum;
						}
					}
				}
			}

			m_wndList.SetCustomFont(lvcd.dc, -.15f, true);
			lvcd.dc.DrawText(strMediaset->c_str(), &lvcd.rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
		
		lvcd.bSkipDefault = true;
		
		break;
		default:
			if (lvcd.nSubItem >= __Column_ModifyTime)
			{
				lvcd.fFontSizeOffset = -.2f;
			}

			break;
		};
	}, [&](tagLVDrawSubItem& lvcd) {
		auto eViewType = m_wndList.GetView();
		if (E_ListViewType::LVT_Report == eViewType || E_ListViewType::LVT_List == eViewType)
		{
			auto pMediaRes = (CMediaRes*)lvcd.pObject;
			__Ensure(pMediaRes);

			POINT pt;
			if (E_ListViewType::LVT_Report == eViewType)
			{
				m_wndList.GetItemPosition(lvcd.uItem, &pt);
			}
			else
			{
				pt.x = lvcd.rc.left;
				pt.y = lvcd.rc.top;
			}

			m_view.m_ImgMgr.smallImglst().Draw(&lvcd.dc, pMediaRes->getImage(), pt, 0);
		}
	});

	m_wndList.SetViewAutoChange([&](E_ListViewType eViewType) {
		m_wndList.UpdateItems();

		m_uIconWidth = m_uTileWidth = 0;
		OnSize(0, m_cx, m_cy);
		
		_asyncTask();
	});

	m_wndStatic.SetParent(&m_wndList);
	m_wndStatic.SetFont(m_wndList.GetFont());

	(void)__super::RegDragableCtrl(m_wndList, [&](tagDragData& DragData) {
		TD_ListObjectList lstObjects;
		m_wndList.GetSelObjects(lstObjects);

		bool bRet = false;
		lstObjects.front([&](CListObject& ListObject) {
			__Ensure(!((CMediaRes&)ListObject).isDir());

			DragData.lstMedias.add(lstObjects);

			DragData.iImage = (int)E_GlobalImage::GI_File;
			
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

		UINT uIconWidth = width / (m_bSingerPanel ? 5:6)-1;
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

void CMediaResPanel::ShowDir(cwstr strPath)
{
	m_strCurrDir = m_strRootDir = strPath;

	CMediaDir *pRootDir = __medialib.subDir(strPath);
	_showDir(pRootDir);
}

void CMediaResPanel::SetDir(cwstr strPath)
{
	m_wndList.DeleteAllItems();

	m_strCurrDir = m_strRootDir = strPath;
}

void CMediaResPanel::Refresh()
{
	CMediaDir *pRootDir = __medialib.subDir(m_strRootDir);
	CMediaDir *pCurrDir = __medialib.subDir(m_strCurrDir);
	_showDir(pRootDir, pCurrDir);
}

void CMediaResPanel::_showDir(CMediaDir *pRootDir, CMediaDir *pCurrDir, CMediaRes *pHitestItem)
{
	__Ensure(m_hWnd);
	
	wstring strPrevCurrDir = m_strCurrDir;
	
	m_pRootDir = pRootDir;

	if (pCurrDir)
	{
		m_pCurrDir = pCurrDir;
		m_strCurrDir = pCurrDir->GetPath();
	}
	else
	{
		m_pCurrDir = m_pRootDir;
		m_strCurrDir = m_strRootDir;
	}

	if (NULL == m_pCurrDir)
	{
		TD_MediaResList paMediaRes;
		_onShowDir(paMediaRes);
		//m_uAddInCount = paMediaRes.size();
		
		if (paMediaRes)
		{
			(void)m_wndStatic.ShowWindow(SW_HIDE);
		}
		else
		{
			(void)m_wndStatic.ShowWindow(SW_SHOW);
		}

		m_wndList.SetObjects(TD_ListObjectList(paMediaRes));

		UpdateTitle(L"");

		return;
	}

	(void)m_wndStatic.ShowWindow(SW_HIDE);

	wstring	strTitle;
	if (!m_strCurrDir.empty())
	{
		if (m_bSingerPanel)
		{
			strTitle.append(__CNDot);
		}
		
		wstring strDirName = fsutil::GetFileName(m_strCurrDir);
		if  (strDirName.size() > __MaxTabTitle)
		{
			strDirName.erase(__MaxTabTitle);
			strDirName.append(L"...");
		}
		strTitle.append(strDirName);
	}

	if (!m_bSingerPanel)
	{
		if (strTitle.empty())
		{
			strTitle.append(L"媒体库");
		}
		strTitle.append(2, ' ');
	}

	UpdateTitle(strTitle);

	__waitCursor;
	CRedrawLock RedrawLock(m_wndList);

	cauto paSubDir = m_pCurrDir->dirs();
	//if (!m_bSingerPanel) {
	cauto singerMgr = m_view.getSingerMgr();
	for (auto pSubDir : paSubDir)
	{
		auto& MediaDir = (CMediaDir&)*pSubDir;
		auto pSinger = singerMgr.matchSingerDir(MediaDir.GetPath());
		if (pSinger)
		{
			MediaDir.SetRelatedMediaSet(E_RelatedMediaSet::RMS_Singer, pSinger->m_uID, pSinger->m_strName);
		}
		else
		{
			MediaDir.ClearRelatedMediaSet(E_RelatedMediaSet::RMS_Singer);
		}
	}
	
	TD_MediaResList paMediaRes;
	_onShowDir(paMediaRes);
	m_uAddInCount = paMediaRes.size();

	paMediaRes.add(paSubDir);
	paMediaRes.add(m_pCurrDir->files());
	m_wndList.SetObjects(TD_ListObjectList(paMediaRes));
	
	if (pHitestItem)
	{
		m_wndList.SelectObject(pHitestItem);
	}
	else
	{
		if (m_strCurrDir != strPrevCurrDir)
		{
			m_wndList.EnsureVisible(0, FALSE);
		}
	}
	
	_asyncTask();
}

void CMediaResPanel::_asyncTask()
{
	if (m_bSingerPanel)
	{
		if (NULL == m_pCurrDir || m_pCurrDir->rootDir() != m_pRootDir)
		{
			return;
		}
	}

	m_wndList.AsyncTask(__AsyncTaskElapse + m_wndList.GetItemCount() / 10, [&](CListObject& object) {
		CMediaRes& mediaRes = (CMediaRes&)object;
		if (!mediaRes.isDir())
		{
			mediaRes.asyncTask();
		}
		return false;
	});
}

BOOL CMediaResPanel::HittestMedia(IMedia& media, CWnd& wnd)
{
	cauto strPath = media.GetPath();
	auto pMediaRes = __medialib.subFile(strPath);
	if (NULL == pMediaRes)
	{
		cauto strDir = fsutil::GetParentDir(strPath);
		if (!strDir.empty())
		{
			CMediaDir *pMediaDir = __medialib.subDir(strDir);
			if (pMediaDir)
			{
				pMediaDir->clear();
				CMediaResPanel::RefreshMediaResPanel();

				pMediaRes = pMediaDir->subFile(strPath);
			}
		}
	}
	
	if (NULL == pMediaRes)
	{
		CMainApp::msgBox(L"未定位到曲目: \n\n\t" + strPath, &wnd);
		return FALSE;
	}

	HittestMediaRes(*pMediaRes);

	return TRUE;
}

void CMediaResPanel::HittestMediaRes(CMediaRes& MediaRes)
{
	_showDir(m_pRootDir, MediaRes.parent(), &MediaRes);

	if (!m_bSingerPanel)
	{
		this->Active();
	}
}

void CMediaResPanel::UpdateRelated(E_RelatedMediaSet eRmsType, const tagMediaSetChanged& MediaSetChanged)
{
	__Ensure(m_pCurrDir);

	if (!m_bSingerPanel)
	{
		if (E_RelatedMediaSet::RMS_Singer == eRmsType)
		{
			(void)m_pCurrDir->UpdateRelatedMediaSet(eRmsType, MediaSetChanged);
		}
	}
	
	UINT uIdx = 0;
	m_pCurrDir->get([&](CMediaRes& MediaRes){
		if (MediaRes.UpdateRelatedMediaSet(eRmsType, MediaSetChanged))
		{
			m_wndList.UpdateItem(m_uAddInCount+uIdx, &MediaRes);
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
		__Ensure(m_pCurrDir && m_pCurrDir != m_pRootDir);
	
		_showDir(m_pRootDir, m_pCurrDir->parent(), m_pCurrDir);
		
		break;
	case ID_OPEN:
		if (0==uVkKey && !lstMediaRes)
		{
			TD_ListObjectList lstAllObject;
			m_wndList.GetAllObjects(lstAllObject);
			TD_MediaResList lsAllMediaRes(lstAllObject);
			lsAllMediaRes([&](CMediaRes& MediaRes){
				if (!MediaRes.isDir())
				{
					lstMediaRes.add(MediaRes);
				}
			});
		}

		lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.isDir())
			{
				_showDir(m_pRootDir, (CMediaDir*)&MediaRes);
			}
			else
			{
				m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(lstMediaRes));
			}
		});

		break;
	case ID_EXPORT:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.isDir())
			{
				m_view.exportDir((CMediaDir&)MediaRes);
			}
			else
			{
				m_view.exportMedia(TD_IMediaList(lstMediaRes));
			}
		}))
		{
			if (NULL != m_pCurrDir)
			{
				m_view.exportDir(*m_pCurrDir);
			}
		}

		break;
	case ID_Snapshot:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.isDir())
			{
				m_view.snapshotDir((CMediaDir&)MediaRes);
			}
		}))
		{
			if (NULL != m_pCurrDir)
			{
				m_view.snapshotDir(*m_pCurrDir);
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
				m_view.exploreMedia(MediaRes);
			});
		}
		else if (NULL != m_pCurrDir)
		{
			m_view.exploreMedia(*m_pCurrDir, false);
		}

		break;
	case ID_FIND:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			m_view.findMedia(MediaRes.GetPath(), MediaRes.isDir());
		}))
		{
			if (NULL != m_pCurrDir)
			{
				m_view.findMedia(m_strCurrDir, true);
			}
		}

		break;
	case ID_CheckSimilar:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.isDir())
			{
				MediaRes.clear();
				CMediaResPanel::RefreshMediaResPanel();

				m_view.checkSimilarFile((CMediaDir&)MediaRes);
			}
		}))
		{
			if (NULL != m_pCurrDir)
			{
				m_pCurrDir->clear();
				CMediaResPanel::RefreshMediaResPanel();

				m_view.checkSimilarFile(*m_pCurrDir);
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
		__Ensure(m_pCurrDir);

		lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (!MediaRes.isDir())
			{
				__Ensure(confirmBox(L"确认删除所选文件?"));

				__waitCursor;
				(void)m_view.getController().removeMediaRes(lstMediaRes);
			}
		});

		break;
	case ID_Detach:
		lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.isDir() && MediaRes.parent() == NULL)
			{
				m_view.getModel().detachDir(MediaRes.GetAbsPath());
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
			CRCueFile cueFile = MediaRes.cueFile();
			if (cueFile)
			{
				CTrackDetailDlg(m_view, cueFile, &MediaRes).DoModal();
				return;
			}
		});

		break;
	case ID_REFRESH:
		if (m_pCurrDir)
		{
			m_pCurrDir->clear();
			CMediaResPanel::RefreshMediaResPanel();
		}
		else
		{
			m_view.getModel().refreshMediaLib();
		}

		break;
	case ID_FormatTitle:
		if (!lstMediaRes.front([&](CMediaRes& MediaRes) {
			if (MediaRes.isDir())
			{
				if (m_view.formatFileTitle((CMediaDir&)MediaRes) > 0)
				{
					MediaRes.clear();
					CMediaResPanel::RefreshMediaResPanel();
				}
			}
		}))
		{
			if (m_pCurrDir)
			{
				if (m_view.formatFileTitle(*m_pCurrDir) > 0)
				{
					m_pCurrDir->clear();
					CMediaResPanel::RefreshMediaResPanel();
				}
			}
		}

		break;
	}
}

//void CMediaResPanel::OnDeleteDir(CMediaRes& dir)
//{
//	CMediaRes *pTargetPath = m_pCurrDir;
//	while(TRUE)
//	{
//		if (m_pCurrDir == &dir || fsutil::CheckSubPath(dir.GetPath(), m_strCurrDir))
//		{
//			if (pTargetPath == m_pRootDir)
//			{
//				break;
//			}
//			
//			pTargetPath = m_pRootDir;
//			continue;
//		}
//
//		return;
//	}
//
//	ShowDir(NULL);
//}

void CMediaResPanel::OnNMDBblClkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	//__Ensure(m_pCurrDir);

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	if (pNMLV->iItem >= 0)
	{
		m_wndList.DeselectAll();		
		m_wndList.SelectItem(pNMLV->iItem);
		
		CMediaRes *pMediaRes = (CMediaRes*)m_wndList.GetItemObject(pNMLV->iItem);

		this->OnMenuCommand(ID_OPEN);

		if (NULL != pMediaRes)
		{
			CRCueFile cueFile = pMediaRes->cueFile();
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
			if (MediaRes.isDir())
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
	m_MenuGuard.EnableItem({ ID_OPEN, ID_FIND, ID_CopyTitle, ID_RENAME, ID_DELETE, ID_Detach, ID_Attach, ID_FormatTitle }, FALSE);

	if (pSubDir)
	{
		m_MenuGuard.SetItemText(ID_OPEN, _T("打开"));
		m_MenuGuard.EnableItem(ID_OPEN, TRUE);

		m_MenuGuard.EnableItem(ID_FIND, TRUE);

		m_MenuGuard.EnableItem(ID_CopyTitle, TRUE);

		m_MenuGuard.EnableItem(ID_FormatTitle, TRUE);

		//if (pSubDir->rootDir() == m_pRootDir)
		if (pSubDir->parent())
		{
			m_MenuGuard.EnableItem(ID_RENAME, TRUE);
		}
		else
		{
			if (m_bSingerPanel)
			{
				m_MenuGuard.EnableItem(ID_RENAME, TRUE);
			}

			m_MenuGuard.EnableItem(ID_Detach, TRUE);
		}
	}
	else
	{
		m_MenuGuard.SetItemText(ID_OPEN, _T("播放"));

		if (m_pCurrDir)
		{
			m_MenuGuard.EnableItem(ID_OPEN, m_pCurrDir->files());
		}

		m_MenuGuard.EnableItem(ID_FIND, m_pCurrDir && m_pCurrDir != &__medialib);

		m_MenuGuard.EnableItem(ID_EXPLORE, m_pCurrDir);

		bool bFlag = m_wndList.GetItemCount()>0;
		m_MenuGuard.EnableItem(ID_EXPORT, m_pCurrDir && bFlag);
		m_MenuGuard.EnableItem(ID_Snapshot, m_pCurrDir && bFlag);
		m_MenuGuard.EnableItem(ID_CheckSimilar, m_pCurrDir && bFlag);
		
		if (m_pCurrDir == m_pRootDir)
		{
			m_MenuGuard.EnableItem(ID_Attach, TRUE);
		}

		if (m_pCurrDir && m_pCurrDir != &__medialib)
		{
			m_MenuGuard.EnableItem(ID_FormatTitle, m_pCurrDir && bFlag);
		}
	}

	m_MenuGuard.EnableItem(ID_Upward, m_pCurrDir && m_pCurrDir != m_pRootDir);

	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);
}

void CMediaResPanel::_showFileMenu(TD_MediaResList& lstMediaRes)
{
	UINT lpIDMenuItem[] { ID_FIND, ID_ViewTrack, ID_CopyTitle, ID_RENAME };
	for (UINT uIndex = 0; uIndex < sizeof(lpIDMenuItem) / sizeof(lpIDMenuItem[0]); uIndex++)
	{
		m_FileMenuGuard.EnableItem(lpIDMenuItem[uIndex], FALSE);
	}

	if (1 == lstMediaRes.size())
	{
		m_FileMenuGuard.EnableItem(ID_FIND, TRUE);

		lstMediaRes.front([&](CMediaRes& subFile) {
			if (subFile.cueFile())
			{
				m_FileMenuGuard.EnableItem(ID_ViewTrack, TRUE);
			}
		});

		m_FileMenuGuard.EnableItem(ID_CopyTitle, TRUE);

		m_FileMenuGuard.EnableItem(ID_RENAME, TRUE);
	}

	m_FileMenuGuard.EnableItem(ID_Upward, m_pCurrDir != m_pRootDir);
	
	(void)m_FileMenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);
}

void CMediaResPanel::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	int iSubItem = lpNMList->iSubItem;
	if (__Column_Playlist == iSubItem || __Column_SingerAlbum == iSubItem)
	{
		__Ensure(m_wndList.GetSelectedCount()<=1 && !CMainApp::getKeyState(VK_SHIFT) && !CMainApp::getKeyState(VK_CONTROL));

		int iItem = lpNMList->iItem;
		m_wndList.AsyncLButtondown([=]() {
			CMediaRes* pMediaRes = (CMediaRes*)m_wndList.GetItemObject(iItem);
			__Ensure(pMediaRes);

			if (!pMediaRes->isDir())
			{
				if (__Column_Playlist == iSubItem)
				{
					pMediaRes->findRelatedPlayItem();
				}
				else
				{
					pMediaRes->findRelatedAlbumItem();
				}
				m_wndList.UpdateItem(iItem, pMediaRes);
			}

			if (__Column_Playlist == iSubItem)
			{
				m_view.hittestRelatedPlaylist(*pMediaRes);
			}
			else
			{
				if (!m_view.hittestRelatedAlbum(*pMediaRes))
				{
					if (!m_bSingerPanel)
					{
						(void)m_view.hittestRelatedSinger(*pMediaRes);
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
			if (!((CMediaRes*)pr.second)->isDir())
			{
				bHasFile = true;
				break;
			}
		}

		if (bHasFile)
		{
			for (auto& pr : mapObjects)
			{
				if (((CMediaRes*)pr.second)->isDir())
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
	__EnsureReturn(m_pCurrDir && lstMedias, DROPEFFECT_NONE);

	bool bScroll = DragScroll(m_wndList, DragContext.x, DragContext.y);

	__EnsureReturn(DragContext.y > (int)m_wndList.GetHeaderHeight(), DROPEFFECT_NONE);

	DROPEFFECT dwRet = DROPEFFECT_NONE;
	lstMedias.front([&](IMedia& media) {
		if (E_MediaType::MT_MediaRes != media.type())
		{
			dwRet = DROPEFFECT_MOVE;
		}
	});

	DragContext.pTargetObj = m_pCurrDir;

	int nItem = m_wndList.HitTest(CPoint(DragContext.x, DragContext.y));
	if (nItem >= 0)
	{
		CMediaRes *pDragOverPath = (CMediaRes*)m_wndList.GetItemObject(nItem);
		if (pDragOverPath && pDragOverPath->isDir())
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
	__waitCursor;

	CMediaRes *pDstDir = (CMediaRes*)DragContext.pTargetObj;
	__AssertReturn(pDstDir, FALSE);

	(void)m_view.getController().moveMediaFile(lstMedias, pDstDir->GetPath());

	return TRUE;
}

int CMediaResPanel::GetTabImage()
{
	if (m_pCurrDir)
	{
		if (m_pCurrDir->rootDir() != &__medialib)
		{
			return (int)E_GlobalImage::GI_AttachDir;
		}
	}

	return (int)E_GlobalImage::GI_Dir;
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
