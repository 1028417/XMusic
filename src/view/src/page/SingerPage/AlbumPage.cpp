
#include "StdAfx.h"

#include "AlbumPage.h"

#include "SingerPage.h"

enum E_AlbumItemColumn
{
	__Column_Name = 0
	, __Column_Type
	, __Column_Size
	, __Column_Duration
	, __Column_Playlist
	, __Column_Path
	, __Column_AddTime
};

#define __BrowseTop (UINT(m_view.m_globalSize.m_uAlbumDockWidth * 3 / 4))

#define __PlaySingerImageElapse 8000

#define __SingerImgRect CRect(m_cx - m_view.m_globalSize.m_uAlbumDockWidth, 0, m_cx, __BrowseTop)

static const RECT g_rcSingerImgMargin{ 3,0,0,0 };

CAlbumPage::CAlbumPage(__view& view)
	: CBasePage(view, IDD_PAGE_ALBUM, L"", IDR_MENU_ALBUMITEM, true)
	, m_AlbumMenuGuard(view.m_ResModule, IDR_MENU_ALBUM, __MenuWidth)
	, m_wndMediaResPanel(view, *this)
{
}

BEGIN_MESSAGE_MAP(CAlbumPage, CBasePage)
	ON_WM_SIZE()
	
	ON_WM_PAINT()

	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BROWSE, &CAlbumPage::OnLvnItemchangedListBrowse)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_BROWSE, &CAlbumPage::OnLvnEndlabeleditListBrowse)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_BROWSE, &CAlbumPage::OnLvnBeginlabeleditListBrowse)

	ON_NOTIFY(NM_RCLICK, IDC_LIST_BROWSE, &CAlbumPage::OnNMRclickListBrowse)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_BROWSE, &CAlbumPage::OnNMDblclkListBrowse)

	ON_NOTIFY(NM_RCLICK, IDC_LIST_EXPLORE, &CAlbumPage::OnNMRclickListExplore)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_EXPLORE, &CAlbumPage::OnNMDblclkListExplore)

	ON_NOTIFY(NM_CLICK, IDC_LIST_EXPLORE, &CAlbumPage::OnNMClickListExplore)
END_MESSAGE_MAP()

void CAlbumPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST_BROWSE, m_wndBrowseList);
	DDX_Control(pDX, IDC_LIST_EXPLORE, m_wndAlbumItemList);

	CPage::DoDataExchange(pDX);
}

BOOL CAlbumPage::OnInitDialog()
{
	(void)CPage::OnInitDialog();

	auto& globalSize = m_view.m_globalSize;

	auto nSigngerImgWidth = globalSize.m_uAlbumDockWidth - g_rcSingerImgMargin.left - g_rcSingerImgMargin.right;
	auto nSigngerImgHeight = __BrowseTop - g_rcSingerImgMargin.top - g_rcSingerImgMargin.bottom;

	__AssertReturn(m_imgSinger.InitCompDC(E_ImgFixMode::IFM_Outer, true, nSigngerImgWidth, nSigngerImgHeight), FALSE);

	__AssertReturn(m_imgSingerDefault.InitCompDC(E_ImgFixMode::IFM_Outer, true, nSigngerImgWidth, nSigngerImgHeight), FALSE);
	wstring strDefaultImg = m_view.m_ImgMgr.getImgDir() + L"singerdefault.png";	
	__AssertReturn(m_imgSingerDefault.LoadEx(strDefaultImg), FALSE);

	(void)m_wndBrowseList.ModifyStyle(0, LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_EDITLABELS);
	
	m_wndBrowseList.SetImageList(NULL, &m_view.m_ImgMgr.getImglst(E_GlobalImglst::GIL_Big));

	CObjectList::tagListPara ListPara({ { _T(""), globalSize.m_uAlbumDockWidth } });

	ListPara.fFontSize = m_view.m_globalSize.m_fBigFontSize;
	ListPara.crText = __Color_Text;

	__AssertReturn(m_wndBrowseList.InitCtrl(ListPara), FALSE);

	m_wndBrowseList.SetCustomDraw([&](tagLVCustomDraw& lvcd) {
		CAlbum *pAlbum = (CAlbum *)lvcd.pObject;
		if (NULL != pAlbum)
		{
			if (pAlbum->property().isDisableDemand())
			{
				lvcd.uTextAlpha = pAlbum->property().isDisableExport() ? 200 : 128;
			}
			else if (pAlbum->property().isDisableExport())
			{
				lvcd.uTextAlpha = 128;
			}
		}
	});

	(void)m_wndAlbumItemList.ModifyStyle(WS_VISIBLE | LVS_ALIGNLEFT
		, LVS_AUTOARRANGE | LVS_EDITLABELS);

	m_wndAlbumItemList.SetImageList(&m_view.m_ImgMgr.getImglst(E_GlobalImglst::GIL_Big)
		, &m_view.m_ImgMgr.getImglst(E_GlobalImglst::GIL_Small));

	CRect rcClient;
	m_view.m_MainWnd.GetClientRect(rcClient);
	UINT width = rcClient.Width() - globalSize.m_uLeftDockWidth - globalSize.m_uAlbumDockWidth - globalSize.m_uScrollbarWidth/2;
	
	CListColumnGuard ColumnGuard(width + globalSize.m_ColWidth_AddTime);
	ColumnGuard.addDynamic(_T("专辑曲目"), 0.62)
		.addFix(_T("类型"), globalSize.m_ColWidth_Type, true)
		.addFix(_T("大小"), globalSize.m_ColWidth_FileSize, true)
		.addFix(_T("时长"), globalSize.m_ColWidth_Duration, true)
		.addFix(_T("关联列表"), globalSize.m_ColWidth_RelatedPlaylist, true)
		.addDynamic(_T("目录"), 0.38)
		.addFix(_T("加入时间"), globalSize.m_ColWidth_AddTime, true);

	ListPara = CObjectList::tagListPara(ColumnGuard);

	ListPara.uHeaderHeight = globalSize.m_uHeadHeight;
	ListPara.fHeaderFontSize = globalSize.m_fBigFontSize;

	ListPara.fFontSize = globalSize.m_fSmallFontSize;
	ListPara.crText = __Color_Text;

	width -= m_view.m_globalSize.m_uScrollbarWidth;
	ListPara.uTileWidth = width / 4-1;
	ListPara.uTileHeight = globalSize.m_uTileHeight;

	__AssertReturn(m_wndAlbumItemList.InitCtrl(ListPara), FALSE);

	m_wndAlbumItemList.SetCustomDraw([&](tagLVCustomDraw& lvcd) {
		CAlbumItem *pAlbumItem = (CAlbumItem *)lvcd.pObject;
		if (NULL != pAlbumItem)
		{
			if (pAlbumItem->GetFileSize() < 0 || pAlbumItem->GetDuration() == 0)
			{
				lvcd.uTextAlpha = 128;
			}
		}

		switch (lvcd.nSubItem)
		{
		case __Column_Playlist:
		case __Column_Path:
			lvcd.bSetUnderline = true;

			if (__Column_Path == lvcd.nSubItem)
			{
				lvcd.fFontSizeOffset = -.2f;
			}

			break;
		case __Column_Name:

			break;
		default:
			lvcd.fFontSizeOffset = -.2f;
			
			break;
		};
	});

	m_wndAlbumItemList.SetViewAutoChange([&](E_ListViewType eViewType) {
		m_wndAlbumItemList.UpdateItems();

		_asyncTask();
	});

	m_wndAlbumItemList.SetIconSpacing(width / 5-1, m_view.m_globalSize.m_uBigIconSize + m_view.m_globalSize.m_uIconSpace);


	__AssertReturn(m_wndMediaResPanel.Create(*this), FALSE);

	(void)__super::RegDragDropCtrl(m_wndBrowseList, [&](tagDragData& DragData) {
		CAlbum *pAlbum = (CAlbum*)m_wndBrowseList.GetSelObject();
		__EnsureReturn(pAlbum, false);

		__EnsureReturn(0 != pAlbum->m_uID, false);

		DragData.pMediaSet = pAlbum;

		DragData.iImage = (int)E_GlobalImage::GI_Album;

		return true;
	});

	(void)__super::RegDragDropCtrl(m_wndAlbumItemList, [&](tagDragData& DragData) {
		__EnsureReturn(m_pAlbum, false);

		TD_ListObjectList lstObjects;
		m_wndAlbumItemList.GetSelObjects(lstObjects);
		__EnsureReturn(lstObjects, false);

		DragData.lstMedias.add(lstObjects);

		DragData.iImage = (int)E_GlobalImage::GI_AlbumItem;

		return true;
	});

	__super::RegMenuHotkey(m_wndBrowseList, VK_RETURN, ID_PLAY_ALBUM);
	__super::RegMenuHotkey(m_wndBrowseList, VK_F2, ID_RENAME_ALBUM);
	__super::RegMenuHotkey(m_wndBrowseList, VK_DELETE, ID_REMOVE_ALBUM);

	__super::RegMenuHotkey(m_wndAlbumItemList, VK_RETURN, ID_PLAY_ALBUMITEM);
	__super::RegMenuHotkey(m_wndAlbumItemList, VK_F2, ID_RENAME_ALBUMITEM);
	__super::RegMenuHotkey(m_wndAlbumItemList, VK_DELETE, ID_REMOVE_ALBUMITEM);
	
	return TRUE;
}

void CAlbumPage::OnPaint()
{
	if (NULL == m_pSinger)
	{
		__super::OnPaint();
		return;
	}

	CPaintDC dc(this);

	CRect rcSingerImg(__SingerImgRect);
	dc.FillSolidRect(rcSingerImg, __Color_White);

	rcSingerImg.left += g_rcSingerImgMargin.left;
	rcSingerImg.top += g_rcSingerImgMargin.top;
	rcSingerImg.right -= g_rcSingerImgMargin.right;
	rcSingerImg.bottom -= g_rcSingerImgMargin.bottom;
	m_imgSinger.StretchBltEx(dc, rcSingerImg);
}

#define __XOffset 6

void CAlbumPage::OnSize(UINT nType, int cx, int cy)
{
	m_cx = cx;

	int x = cx - m_view.m_globalSize.m_uAlbumDockWidth;

	CRect rcPos(0, 0, x, cy);
	if (m_wndMediaResPanel)
	{
		m_wndMediaResPanel.MoveWindow(rcPos);
	}

	if (m_wndAlbumItemList)
	{
		rcPos.left = __XOffset;
		m_wndAlbumItemList.MoveWindow(rcPos);
		
		m_wndBrowseList.MoveWindow(x-1, __BrowseTop, m_view.m_globalSize.m_uAlbumDockWidth
			, cy - __BrowseTop + m_view.m_globalSize.m_uScrollbarWidth);

		CRect rcSingerImg(__SingerImgRect);
		rcSingerImg.left -= 50;
		this->RedrawWindow(rcSingerImg, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}
}

void CAlbumPage::ShowSinger(CSinger *pSinger, CMedia *pAlbumItem, IMedia *pIMedia)
{
	if (!m_hWnd)
	{
		if (!pSinger)
		{
			return;
		}

		m_view.m_ResModule.ActivateResource();
		__Assert(m_view.m_MainWnd.AddPage(*this, E_DockViewType::DVT_DockCenter));
		__Assert(__super::Active());
	}

	m_pAlbum = NULL;

	bool bSingerChanged = (pSinger != m_pSinger);
	m_pSinger = pSinger;
	if (NULL == m_pSinger)
	{
		UpdateTitle();

		(void)m_wndBrowseList.ShowWindow(SW_HIDE);
		(void)m_wndBrowseList.DeleteAllItems();

		(void)m_wndAlbumItemList.ShowWindow(SW_HIDE);
		(void)m_wndAlbumItemList.DeleteAllItems();

		m_wndMediaResPanel.ShowPath();
		(void)m_wndMediaResPanel.ShowWindow(SW_SHOW);
		
		// TODO m_view.m_MainWnd.RemvePage(*this);
		this->Invalidate();

		return;
	}

	(void)m_wndBrowseList.ShowWindow(SW_SHOW);

	if (bSingerChanged)
	{
		static CWinTimer s_timer;
		s_timer.kill();

		if (_playSingerImage(true))
		{
			s_timer.set(__PlaySingerImageElapse, [&]() {
				_playSingerImage(false);
				return true;
			});
		}
		else
		{
			m_imgSingerDefault.StretchBltEx(m_imgSinger);
			this->InvalidateRect(__SingerImgRect);
		}

		(void)m_wndBrowseList.SetObjects(TD_ListObjectList(m_pSinger->albums()));

		(void)m_wndBrowseList.InsertItem(0, (wsutil::wcSpace + pSinger->m_strName).c_str(), (int)E_GlobalImage::GI_Dir);

		m_wndMediaResPanel.ShowPath(m_pSinger->GetBaseDir());
	}
		
	if (pAlbumItem)
	{
		m_pAlbum = (CAlbum*)pAlbumItem->m_pParent;

		m_wndBrowseList.SelectObject(m_pAlbum);

		m_wndAlbumItemList.DeselectAll();
		m_wndAlbumItemList.SelectObject(pAlbumItem);
	}
	else
	{
		(void)m_wndBrowseList.SelectFirstItem();
	}

	if (pAlbumItem)
	{
		(void)m_wndAlbumItemList.SetFocus();
	}
	else if (NULL != pIMedia)
	{
		m_wndMediaResPanel.HittestMediaRes(*pIMedia);
		(void)m_wndMediaResPanel.SetFocus();
	}
}

bool CAlbumPage::_playSingerImage(bool bReset)
{
	__AssertReturn(m_pSinger, false);
	
	static UINT uSingerImgIdx = 0;
	if (bReset)
	{
		uSingerImgIdx = 0;
	}

	wstring strSingerImg;
	if (m_view.getModel().getSingerImgMgr().getSingerImg(m_pSinger->m_strName, uSingerImgIdx, strSingerImg))
	{
		(void)m_imgSinger.LoadEx(strSingerImg);
		this->InvalidateRect(__SingerImgRect);

		uSingerImgIdx++;

		return true;
	}

	if (uSingerImgIdx > 1)
	{
		return _playSingerImage(true);
	}
	
	return false;

}

void CAlbumPage::UpdateSingerImage()
{
	_playSingerImage(true);
}

void CAlbumPage::UpdateTitle(const wstring& strTitle)
{
	m_strBaseTitle = L" 歌手";
	if (NULL != m_pSinger)
	{
		m_strBaseTitle.append(__CNDot + m_pSinger->m_strName);
	}

	if (NULL != m_pAlbum)
	{
		SetTitle(__CNDot + m_pAlbum->m_strName + L" ");
	}
	else
	{
		SetTitle(strTitle);		
	}
}

void CAlbumPage::UpdateSingerName(const wstring& strSingerName)
{
	m_wndBrowseList.SetItemText(0, 0, (L" " + strSingerName).c_str());

	UpdateTitle();
}

int CAlbumPage::GetTabImage()
{
	UINT uImgPos = (UINT)E_GlobalImage::GI_SingerDefault;

	if (NULL != m_pSinger)
	{
		uImgPos = m_view.m_ImgMgr.getSingerImgPos(m_pSinger->m_uID);
	}

	return uImgPos;
}

void CAlbumPage::OnNMRclickListBrowse(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNM = (LPNMLISTVIEW)pNMHDR;
	BOOL bEnable = (0 < lpNM->iItem);

	m_AlbumMenuGuard.EnableItem(ID_RENAME_ALBUM, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_REMOVE_ALBUM, bEnable);

	m_AlbumMenuGuard.EnableItem(ID_ADD_ALBUMITEM, bEnable);

	m_AlbumMenuGuard.EnableItem(ID_DisableDemand, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_DisableExport, bEnable);

	m_AlbumMenuGuard.EnableItem(ID_CNLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_HKLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_KRLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_JPLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_TAILanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_ENLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_EURLanguage, bEnable);

	bool bAvalible = false;
	if (bEnable)
	{
		CAlbum *pAlbum = (CAlbum*)m_wndBrowseList.GetItemObject(lpNM->iItem);
		__Assert(pAlbum);
		const CMediasetProperty& property = pAlbum->property();

		m_AlbumMenuGuard.CheckItem(ID_DisableDemand, property.isDisableDemand());
		m_AlbumMenuGuard.CheckItem(ID_DisableExport, property.isDisableExport());

		m_AlbumMenuGuard.CheckItem(ID_CNLanguage, property.isCNLanguage());
		m_AlbumMenuGuard.CheckItem(ID_HKLanguage, property.isHKLanguage());
		m_AlbumMenuGuard.CheckItem(ID_KRLanguage, property.isKRLanguage());
		m_AlbumMenuGuard.CheckItem(ID_JPLanguage, property.isJPLanguage());
		m_AlbumMenuGuard.CheckItem(ID_TAILanguage, property.isTAILanguage());
		m_AlbumMenuGuard.CheckItem(ID_ENLanguage, property.isENLanguage());
		m_AlbumMenuGuard.CheckItem(ID_EURLanguage, property.isEURLanguage());

		bAvalible = pAlbum->available();
	}

	m_AlbumMenuGuard.EnableItem(ID_PLAY_ALBUM, bAvalible);
	m_AlbumMenuGuard.EnableItem(ID_VERIFY_ALBUM, bAvalible);
	m_AlbumMenuGuard.EnableItem(ID_EXPORT_ALBUM, bAvalible);

	(void)m_AlbumMenuGuard.EnableItem(ID_ADD_ALBUM, NULL != m_pSinger);

	(void)m_AlbumMenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMenuFontSize);
}

void CAlbumPage::OnMenuCommand_Album(UINT uID)
{
	int nItem = m_wndBrowseList.GetSelItem();
	CAlbum *pAlbum = (CAlbum*)m_wndBrowseList.GetSelObject();

	switch (uID)
	{
	case ID_VERIFY_ALBUM:
		__Assert(pAlbum);
		m_view.verifyMedia(*pAlbum);

		break;
	case ID_EXPORT_ALBUM:
		__Assert(pAlbum);
		m_view.exportMediaSet(*pAlbum);

		break;
	case ID_PLAY_ALBUM:
		__Ensure(pAlbum);
		m_view.m_PlayCtrl.addPlayingItem(*pAlbum);
		
		break;
	case ID_ADD_ALBUMITEM:
		{
			__Ensure(pAlbum);
			
			tagFileDlgOpt FileDlgOpt;
			FileDlgOpt.strTitle = L"添加专辑曲目";
			FileDlgOpt.strFilter = __MediaFilter;
			FileDlgOpt.strInitialDir = m_view.getMediaLib().toAbsPath(m_pSinger->GetBaseDir(), true);
			CFileDlgEx fileDlg(FileDlgOpt);

			list<wstring> lstFiles;
			fileDlg.ShowOpenMulti(lstFiles);
			__Ensure(!lstFiles.empty());

			int iRet = m_view.getController().addAlbumItems(lstFiles, *pAlbum);
			if (iRet > 0)
			{
				m_wndBrowseList.SelectObject(pAlbum);

				this->_showAlbum(pAlbum);

				UINT uSelectCount = (UINT)iRet;
				m_wndAlbumItemList.SelectItems(m_wndAlbumItemList.GetItemCount() - uSelectCount, uSelectCount);
			}
		}

		break;
	case ID_ADD_ALBUM:
		__AssertBreak(m_pSinger);

		pAlbum = m_view.getSingerMgr().AddAlbum(*m_pSinger);
		__EnsureBreak(pAlbum);

		nItem = m_wndBrowseList.InsertObject(*pAlbum);
		(void)m_wndBrowseList.EditLabel(nItem);

		break;
	case ID_RENAME_ALBUM:
		__EnsureBreak(0 < nItem);

		(void)m_wndBrowseList.EditLabel(nItem);

		break;

	case ID_REMOVE_ALBUM:
		__AssertBreak(pAlbum);

		__EnsureBreak(CMainApp::showConfirmMsg(L"确认删除所选专辑?", *this));

		(void)m_wndBrowseList.DeleteItem(nItem);
		(void)m_wndBrowseList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

		__EnsureBreak(m_view.getController().removeMediaSet(*pAlbum));

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
		__AssertBreak(pAlbum);
		CMediasetProperty property = pAlbum->property();
		if (ID_DisableDemand == uID)
		{
			bool bDisableDemand = !property.isDisableDemand();
			property.setDisableDemand(bDisableDemand);
			property.setDisableExport(bDisableDemand);
			m_wndBrowseList.UpdateItem(nItem);
		}
		else if (ID_DisableExport == uID)
		{
			property.setDisableExport(!property.isDisableExport());
			m_wndBrowseList.UpdateItem(nItem);
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

			if (property.checkLanguage(eLanguage))
			{
				property.unsetLanguage(eLanguage);
			}
			else
			{
				property.setLanguage(eLanguage);
			}
		};

		(void)m_view.getDataMgr().updateMediaSetProperty(*pAlbum, property);
	}
	break;
	}
}

void CAlbumPage::OnMenuCommand_AlbumItem(UINT uID, UINT uVkKey)
{
	TD_ListObjectList lstObjects;
	m_wndAlbumItemList.GetSelObjects(lstObjects);

	TD_MediaList lstAlbumItems(lstObjects);

	switch (uID)
	{
	case ID_ADD_ALBUMITEM:
		OnMenuCommand_Album(ID_ADD_ALBUMITEM);

		break;
	case ID_PLAY_ALBUMITEM:
		if (lstAlbumItems)
		{
			m_view.m_PlayCtrl.addPlayingItem(TD_IMediaList(lstAlbumItems));
		}
		else
		{
			if (0 == uVkKey)
			{
				__AssertBreak(m_pAlbum);
				m_view.m_PlayCtrl.addPlayingItem(*m_pAlbum);
			}
		}

		break;
	case ID_FIND_ALBUMITEM:
		__AssertBreak(1 == lstAlbumItems.size());
		lstAlbumItems.front([&](auto& media) {
			m_view.showFindDlg(media.GetTitle(), false);
		});

		break;
	case ID_HITTEST_ALBUMITEM:
		__AssertBreak(1 == lstAlbumItems.size());
		
		(void)m_wndBrowseList.SelectFirstItem();
		lstAlbumItems.front([&](auto& media) {
			if (m_wndMediaResPanel.HittestMediaRes(media))
			{
				(void)m_wndMediaResPanel.SetFocus();
			}
		});

		break;
	case ID_SETALARMCLOCK:
		{
			__AssertBreak(lstAlbumItems);

			TD_IMediaList lstMedias(lstAlbumItems);
			m_view.getDataMgr().addAlarmmedia(lstMedias);
		}

		break;
	case ID_CopyTitle:
		lstAlbumItems.front([&](auto& media) {
			(void)m_view.copyMediaTitle(media);
		});

		break;
	case ID_EXPLORE_ALBUMITEM:
		if (lstAlbumItems.size() == 1)
		{
			lstAlbumItems.front([](CMedia& media) {
				media.ShellExplore();
			});
		}

		break;
	case ID_REMOVE_ALBUMITEM:
		__EnsureBreak(lstAlbumItems);

		__EnsureBreak(CMainApp::showConfirmMsg(L"确认移除选中的曲目?", *this));

		__EnsureBreak(m_view.getModel().removeMedia(lstAlbumItems));

		break;
	case ID_RENAME_ALBUMITEM:
		__EnsureBreak(1 == lstObjects.size());

		lstObjects.front([&](CListObject& ListObject) {
			(void)m_wndAlbumItemList.EditLabel(m_wndAlbumItemList.GetObjectItem(&ListObject));
		});
	}
}

void CAlbumPage::OnMenuCommand(UINT uID, UINT uVkKey)
{
	CWnd *pWndFocus = CWnd::GetFocus();
	if (&m_wndBrowseList == pWndFocus)
	{
		this->OnMenuCommand_Album(uID);
	}
	else if (&m_wndAlbumItemList == pWndFocus)
	{
		this->OnMenuCommand_AlbumItem(uID, uVkKey);
	}
}

void CAlbumPage::OnLvnItemchangedListBrowse(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	__Ensure(pNMLV->uChanged & LVIF_STATE)

	__Ensure(pNMLV->uNewState & LVIS_SELECTED);

	CAlbum *pAlbum = (CAlbum*)m_wndBrowseList.GetItemObject(pNMLV->iItem);
	_showAlbum(pAlbum);
}

void CAlbumPage::_showAlbum(CAlbum *pAlbum)
{
	__Ensure(m_hWnd);

	bool bChanged = pAlbum != m_pAlbum;

	m_pAlbum = pAlbum;
	if (NULL == m_pAlbum)
	{
		this->UpdateTitle();

		m_wndMediaResPanel.Refresh();
		(void)m_wndMediaResPanel.ShowWindow(SW_SHOW);

		(void)m_wndAlbumItemList.ShowWindow(SW_HIDE);
		(void)m_wndAlbumItemList.DeleteAllItems();

		return;
	}

	this->UpdateTitle();

	(void)m_wndAlbumItemList.SetObjects(TD_ListObjectList(m_pAlbum->albumItems()));

	if (bChanged)
	{
		m_wndAlbumItemList.EnsureVisible(0, FALSE);
	}
	
	(void)m_wndAlbumItemList.ShowWindow(SW_SHOW);
	(void)m_wndMediaResPanel.ShowWindow(SW_HIDE);
	
	_asyncTask();
}

void CAlbumPage::RefreshAlbum()
{
	if (NULL != m_pAlbum)
	{
		_showAlbum(m_pAlbum);
	}
}

void CAlbumPage::OnLvnBeginlabeleditListBrowse(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	NMLVDISPINFO *pLVDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	
	if (0 == pLVDispInfo->item.iItem)
	{
		*pResult = TRUE;
	}
}

void CAlbumPage::OnLvnEndlabeleditListBrowse(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	NMLVDISPINFO *pLVDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	CAlbum *pAlbum = (CAlbum*)m_wndBrowseList.GetItemObject(pLVDispInfo->item.iItem);
	__Ensure(pAlbum);


	CString cstrNewName(pLVDispInfo->item.pszText);
	(void)cstrNewName.Trim();
	__Ensure(!cstrNewName.IsEmpty());

	wstring strNewName = cstrNewName;
	__Ensure(strNewName != pAlbum->m_strName);

	auto eRetCode = m_view.getController().renameMediaSet(*pAlbum, strNewName);
	if (E_RenameRetCode::RRC_Success != eRetCode)
	{
		if (E_RenameRetCode::RRC_InvalidName == eRetCode)
		{
			CMainApp::showMsg(L"名称含特殊字符！", *this);
		}
		else if (E_RenameRetCode::RRC_NameExists == eRetCode)
		{
			CMainApp::showMsg(L"重命名失败，存在同名专辑！", *this);
		}
		return;
	}

	(void)m_wndBrowseList.SetItemText(pLVDispInfo->item.iItem, 0, cstrNewName);

	this->UpdateTitle();
}

DROPEFFECT CAlbumPage::OnMediasDragOverExploreList(const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	__EnsureReturn(_checkMediasDropable(lstMedias), DROPEFFECT_NONE);

	DROPEFFECT dwRet = 0;
	lstMedias.front([&](IMedia& media) {
		if (media.GetMediaSet() == m_pAlbum)
		{
			dwRet = DROPEFFECT_MOVE;
		}
		else
		{
			dwRet = DROPEFFECT_COPY;
		}
	});

	auto eViewType = m_wndAlbumItemList.GetView();
	if (E_ListViewType::LVT_Report == eViewType)
	{
		handleDragOver(m_wndAlbumItemList, DragContext);

		return dwRet;
	}

	int nItem = 0;

	CRect rcItem;
	(void)m_wndAlbumItemList.GetItemRect(m_wndAlbumItemList.GetItemCount() - 1, &rcItem, LVIR_BOUNDS);
	
	bool bOutofRange = false;
	if (E_ListViewType::LVT_List == eViewType)
	{
		if (DragContext.x > rcItem.right || (DragContext.x >= rcItem.left && DragContext.y >= rcItem.bottom))
		{
			bOutofRange = true;
		}
	}
	else if (DragContext.y > rcItem.bottom || (DragContext.y >= rcItem.top && DragContext.x >= rcItem.right))
	{
		bOutofRange = true;
	}

	if (bOutofRange)
	{
		nItem = m_wndAlbumItemList.GetItemCount()-1;
		DragContext.x = rcItem.right;
		DragContext.y = rcItem.bottom;
	}
	else
	{
#define __Margin 10

		tagPOINT lpPoint[4] = {
			{DragContext.x - __Margin, DragContext.y - __Margin}
			, {DragContext.x + __Margin, DragContext.y - __Margin}
			, {DragContext.x - __Margin, DragContext.y + __Margin}
			, {DragContext.x + __Margin, DragContext.y + __Margin}
		};

		map<int, CRect> mapItems;

		for (UINT uIndex = 0; uIndex < sizeof(lpPoint)/sizeof(tagPOINT); ++uIndex)
		{
			nItem = m_wndAlbumItemList.HitTest(lpPoint[uIndex]);
			if (0 <= nItem && mapItems.find(nItem) == mapItems.end())
			{
				(void)m_wndAlbumItemList.GetItemRect(nItem, &mapItems[nItem], LVIR_BOUNDS);
			}
		}

		__EnsureReturn(!mapItems.empty(), DROPEFFECT_NONE);

		UINT uMinDistance = 0;
		for (map<int, CRect>::iterator itItem = mapItems.begin()
			; itItem != mapItems.end(); ++itItem)
		{
			int nDx = itItem->second.CenterPoint().x - DragContext.x;
			int nDy = itItem->second.CenterPoint().y - DragContext.y;
			UINT uDistance = nDx * nDx + nDy * nDy;

			if (0 == uMinDistance || uDistance < uMinDistance)
			{
				uMinDistance = uDistance;

				nItem = itItem->first;

				rcItem = itItem->second;
			}
		}
	}

	DragContext.uTargetPos = nItem;

	if (DragContext.x < rcItem.CenterPoint().x)
	{
		DragContext.x = rcItem.left;
	}
	else
	{
		DragContext.x = rcItem.right+4;

		DragContext.uTargetPos++;
	}

	if (DragContext.x < 1)
	{
		DragContext.x = 1;
	}

	DragContext.DrawDragOverVLine(DragContext.x, rcItem.top, rcItem.bottom);

	return dwRet;
}

BOOL CAlbumPage::OnMediasDropExploreList(const TD_IMediaList& lstMedias, UINT uTargetPos, DROPEFFECT nDropEffect)
{
	__EnsureReturn(lstMedias, FALSE);
	__EnsureReturn(m_pAlbum, FALSE);

	int nNewPos = 0;

	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.GetMediaSet();
	});
	if (pSrcMediaSet != m_pAlbum)
	{
		__EnsureReturn(m_view.getSingerMgr().AddAlbumItems(lstMedias, *m_pAlbum, uTargetPos), FALSE);

		nNewPos = uTargetPos;
	}
	else
	{
		list<UINT> lstSelectedItems;
		m_wndAlbumItemList.GetSelItems(lstSelectedItems);
		__AssertReturn(!lstSelectedItems.empty(), FALSE);

		if (uTargetPos >= lstSelectedItems.front() && uTargetPos <= lstSelectedItems.back() + 1)
		{
			if (lstSelectedItems.back() - lstSelectedItems.front() + 1 == lstSelectedItems.size())
			{
				return FALSE;
			}
		}

		nNewPos = m_view.getSingerMgr().RepositAlbumItem(*m_pAlbum, lstMedias, uTargetPos);
		__EnsureReturn(0 <= nNewPos, FALSE);
	}

	_showAlbum(m_pAlbum);
	m_wndAlbumItemList.SelectItems(nNewPos, lstMedias.size());

	return TRUE;
}

BOOL CAlbumPage::_checkMediasDropable(const TD_IMediaList& lstMedias)
{
	__EnsureReturn(m_pSinger, FALSE);
	
	BOOL bRet = TRUE;
	lstMedias.front([&](IMedia& media) {
		if (E_MediaSetType::MST_Album != media.GetMediaSetType())
		{
			lstMedias([&](IMedia& media) {
				if (!fsutil::CheckSubPath(m_pSinger->GetBaseDir(), media.GetPath()))
				{
					bRet = FALSE;
					return false;
				}

				return true;
			});
		}
	});

	return bRet;
}

DROPEFFECT CAlbumPage::OnMediasDragOverBrowseList(const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	bool bScroll = DragScroll(m_wndBrowseList, DragContext.x, DragContext.y);

	__EnsureReturn(_checkMediasDropable(lstMedias), DROPEFFECT_NONE);

	int nItem = m_wndBrowseList.HitTest(CPoint(5, DragContext.y));
	__EnsureReturn(1 <= nItem, DROPEFFECT_NONE);	
	CAlbum *pDropHilightAlbum = (CAlbum*)m_wndBrowseList.GetItemObject(nItem);
	__AssertReturn(pDropHilightAlbum, DROPEFFECT_NONE);
	
	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.GetMediaSet();
	});
	__EnsureReturn(pSrcMediaSet != pDropHilightAlbum, DROPEFFECT_NONE);
	DragContext.pTargetObj = pDropHilightAlbum;

	if (!bScroll)
	{
		CRect rcItem;
		(void)m_wndBrowseList.GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
		DragContext.DrawDragOverRect(rcItem.top, rcItem.bottom);
	}

	if (pSrcMediaSet && E_MediaSetType::MST_Album==pSrcMediaSet->m_eType)
	{
		if (0 == (DragContext.dwKeyState & MK_CONTROL))
		{
			return DROPEFFECT_MOVE;
		}
	}
	
	return DROPEFFECT_COPY;
}

BOOL CAlbumPage::OnMediasDropBrowseList(const TD_IMediaList& lstMedias, CAlbum *pTargetAlbum, DROPEFFECT nDropEffect)
{
	__EnsureReturn(lstMedias, FALSE);
	__AssertReturn(pTargetAlbum, FALSE);

	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.GetMediaSet();
	});
	if (pSrcMediaSet && E_MediaSetType::MST_Album == pSrcMediaSet->m_eType)
	{
		__EnsureReturn(pTargetAlbum != pSrcMediaSet, FALSE);
		
		__EnsureReturn(m_view.getSingerMgr().AddAlbumItems(lstMedias, *pTargetAlbum), FALSE);

		if (DROPEFFECT_MOVE == nDropEffect)
		{
			TD_AlbumItemList lstAlbumItems(lstMedias);
			__EnsureReturn(m_view.getSingerMgr().RemoveAlbumItems(lstAlbumItems), FALSE);
		}
	}
	else
	{
		__EnsureReturn(m_view.getSingerMgr().AddAlbumItems(lstMedias, *pTargetAlbum), FALSE);
	}

	m_wndBrowseList.SelectObject(pTargetAlbum);

	_showAlbum(pTargetAlbum);

	int nSelectCount = (int)lstMedias.size();
	m_wndAlbumItemList.SelectItems(m_wndAlbumItemList.GetItemCount()-nSelectCount, nSelectCount);

	(void)m_wndAlbumItemList.SetFocus();

	return TRUE;
}

DROPEFFECT CAlbumPage::OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__EnsureReturn(pwndCtrl == &m_wndBrowseList, DROPEFFECT_NONE);
	
	__EnsureReturn(pMediaSet && E_MediaSetType::MST_Album == pMediaSet->m_eType, DROPEFFECT_NONE);
	
	int nDragingItem = m_wndBrowseList.GetObjectItem(pMediaSet);
	__AssertReturn(0 <= nDragingItem, DROPEFFECT_NONE);
	
	DROPEFFECT dwRet = DROPEFFECT_MOVE;
	handleDragOver(m_wndBrowseList, DragContext, [&](int& iDragOverPos) {
		if (iDragOverPos < 1)
		{
			dwRet = DROPEFFECT_NONE;
			return false;
		}

		if (iDragOverPos > nDragingItem)
		{
			iDragOverPos--;
		}

		return true;
	});

	return dwRet;
}

BOOL CAlbumPage::OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndBrowseList && pMediaSet && E_MediaSetType::MST_Album == pMediaSet->m_eType, DROPEFFECT_NONE);

	int nDragingItem = m_wndBrowseList.GetObjectItem(pMediaSet);
	__AssertReturn(0 <= nDragingItem, FALSE);
	
	int iTargetPos = DragContext.uTargetPos;
	CAlbum *pAlbum = m_view.getSingerMgr().RepositAlbum(*(CAlbum*)pMediaSet, iTargetPos-1);
	__EnsureReturn(pAlbum, FALSE);

	(void)m_wndBrowseList.DeleteItem(nDragingItem);
	(void)m_wndBrowseList.InsertObject(*pAlbum, iTargetPos);
	m_wndBrowseList.SelectItem(iTargetPos);
	
	return TRUE;
}

DROPEFFECT CAlbumPage::OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	if (pwndCtrl == &m_wndBrowseList)
	{
		return OnMediasDragOverBrowseList(lstMedias, DragContext);
	}
	else if (pwndCtrl == &m_wndAlbumItemList)
	{
		return OnMediasDragOverExploreList(lstMedias, DragContext);
	}

	return DROPEFFECT_NONE;
}

BOOL CAlbumPage::OnMediasDrop(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	if (pwndCtrl == &m_wndBrowseList)
	{
		return OnMediasDropBrowseList(lstMedias, (CAlbum*)DragContext.pTargetObj, DragContext.nDropEffect);
	}
	else if (pwndCtrl == &m_wndAlbumItemList)
	{
		return OnMediasDropExploreList(lstMedias, DragContext.uTargetPos, DragContext.nDropEffect);
	}

	return FALSE;
}

void CAlbumPage::OnNMRclickListExplore(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_MenuGuard.EnableItem(ID_PLAY_ALBUMITEM, (m_pAlbum && m_pAlbum->available()));

	int nCount = m_wndAlbumItemList.GetSelectedCount();
	m_MenuGuard.EnableItem(ID_FIND_ALBUMITEM, (1 == nCount));
	m_MenuGuard.EnableItem(ID_HITTEST_ALBUMITEM, (1 == nCount));
	m_MenuGuard.EnableItem(ID_SETALARMCLOCK, (0 < nCount));
	
	m_MenuGuard.EnableItem(ID_CopyTitle, (1 == nCount));
	m_MenuGuard.EnableItem(ID_EXPLORE_ALBUMITEM, (1 == nCount));
	m_MenuGuard.EnableItem(ID_RENAME_ALBUMITEM, (1 == nCount));
	m_MenuGuard.EnableItem(ID_REMOVE_ALBUMITEM, (0 < nCount));

	m_MenuGuard.EnableItem(ID_ADD_ALBUMITEM, (0 == nCount));
	
	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMenuFontSize);
}

void CAlbumPage::OnNMDblclkListBrowse(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	this->OnMenuCommand(ID_PLAY_ALBUM);
}

void CAlbumPage::OnNMDblclkListExplore(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMLISTVIEW lpNMList = (LPNMLISTVIEW)pNMHDR;
	__Ensure(0 <= lpNMList->iItem);

	m_wndAlbumItemList.DeselectAll();
	m_wndAlbumItemList.SelectItem(lpNMList->iItem);

	this->OnMenuCommand(ID_PLAY_ALBUMITEM);
}

void CAlbumPage::OnNMClickListExplore(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
	LPNMITEMACTIVATE lpNMList = (LPNMITEMACTIVATE)pNMHDR;
	int iSubItem = lpNMList->iSubItem;

	if (__Column_Playlist == iSubItem || __Column_Path == iSubItem)
	{
		int iItem = lpNMList->iItem;

		m_wndAlbumItemList.AsyncLButtondown([=]() {
			CMedia *pAlbumItem = (CMedia*)m_wndAlbumItemList.GetItemObject(iItem);
			__Ensure(pAlbumItem);
			pAlbumItem->AsyncTask();
			m_wndAlbumItemList.UpdateItem(iItem, pAlbumItem);

			if (__Column_Playlist == iSubItem)
			{
				m_view.hittestRelatedMediaSet(*pAlbumItem, E_MediaSetType::MST_Playlist);
			}
			else
			{
				this->OnMenuCommand_AlbumItem(ID_HITTEST_ALBUMITEM);
			}
		});
	}
}

void CAlbumPage::UpdateRelated(const tagMediaSetChanged& MediaSetChanged)
{
	if (NULL != m_pSinger)
	{
		m_wndMediaResPanel.UpdateRelated(MediaSetChanged);

		if (NULL != m_pAlbum)
		{
			size_t uIdx = 0;
			for (CAlbumItem& AlbumItem : m_pAlbum->albumItems())
			{
				if (AlbumItem.UpdateRelatedMediaSet(MediaSetChanged))
				{
					m_wndAlbumItemList.UpdateItem(uIdx, &AlbumItem);
				}
				uIdx++;
			}
		}
	}
}

void CAlbumPage::_asyncTask()
{
	if (m_wndAlbumItemList.isReportView())
	{
		m_wndAlbumItemList.AsyncTask(100, [&](UINT uItem) {
			__Ensure(m_pAlbum);

			m_pAlbum->albumItems().get(uItem, [&](CAlbumItem& AlbumItem) {
				AlbumItem.AsyncTask();
				m_wndAlbumItemList.UpdateItem(uItem, &AlbumItem); // , { 2,3,__Column_Playlist });
			});
		});
	}
}
