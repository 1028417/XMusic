
#include "StdAfx.h"

#include "AlbumPage.h"

#include "SingerPage.h"

enum E_AlbumItemColumn
{
	__Column_Name = 0
	, __Column_Info
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

	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_BROWSE, &CAlbumPage::OnNMSetFocusListBrowse)
	ON_NOTIFY(NM_SETFOCUS, IDC_LIST_EXPLORE, &CAlbumPage::OnNMSetFocusListExplore)
	
	ON_NOTIFY(NM_RCLICK, IDC_LIST_EXPLORE, &CAlbumPage::OnNMRclickListExplore)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_EXPLORE, &CAlbumPage::OnNMDblclkListExplore)

	ON_NOTIFY(NM_CLICK, IDC_LIST_EXPLORE, &CAlbumPage::OnNMClickListExplore)
END_MESSAGE_MAP()

void CAlbumPage::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX, IDC_LIST_BROWSE, m_wndAlbumList);
	DDX_Control(pDX, IDC_LIST_EXPLORE, m_wndAlbumItemList);

	CPage::DoDataExchange(pDX);
}

BOOL CAlbumPage::OnInitDialog()
{
	(void)CPage::OnInitDialog();

	cauto strSingerImg = m_view.m_ImgMgr.getImgPath(L"singerdefault");
	__AssertReturn(m_imgSingerDefault.Load(strSingerImg), FALSE);

	auto& globalSize = m_view.m_globalSize;
	m_wndAlbumList.SetImageList(NULL, &m_view.m_ImgMgr.bigImglst());
	
	(void)m_wndAlbumList.ModifyStyle(0, LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_EDITLABELS);
	
	CObjectList::tagListPara ListPara(globalSize.m_uAlbumDockWidth);

	ListPara.fFontSize = m_view.m_globalSize.m_fMidFontSize;
	ListPara.crText = __Color_Text;

	__AssertReturn(m_wndAlbumList.InitCtrl(ListPara), FALSE);
	
	m_wndAlbumList.SetCustomDraw([&](tagLVDrawSubItem& lvcd) {
		CAlbum *pAlbum = (CAlbum*)lvcd.pObject;
		if (pAlbum)
		{
			if (pAlbum->property().isDisableDemand() && pAlbum->property().isDisableExport())
			{
				lvcd.setTextAlpha(200);
			}
			else if (pAlbum->property().isDisableDemand() || pAlbum->property().isDisableExport())
			{
				lvcd.setTextAlpha(128);
			}
		}
	}, [=](tagLVDrawSubItem& lvcd){
		__Ensure(m_pSinger);

		auto eImage = E_GlobalImage::GI_Album;
		CAlbum *pAlbum = (CAlbum*)lvcd.pObject;
		if (pAlbum)
		{
			if (pAlbum->albumType() == E_AlbumType::AT_Dir)
			{
				eImage = E_GlobalImage::GI_AttachDir;
			}
		}
		else
		{
			eImage = E_GlobalImage::GI_Dir;
		}

		auto& rc = lvcd.rc;
		auto offset = (rc.bottom - rc.top - (int)m_view.m_globalSize.m_uBigIconSize) / 2;

		CDC& dc = lvcd.dc;
		m_view.m_ImgMgr.bigImglst().Draw(&dc, (int)eImage, { offset, rc.top + offset }, 0);

		if (NULL == pAlbum)
		{
			rc.left = rc.bottom - 20;

			dc.SetTextColor(lvcd.crText);
			dc.DrawText(m_pSinger->m_strName.c_str(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}
	});

	(void)m_wndAlbumItemList.ModifyStyle(WS_VISIBLE | LVS_ALIGNLEFT
		, LVS_AUTOARRANGE | LVS_EDITLABELS | LVS_NOCOLUMNHEADER);

	m_wndAlbumItemList.SetImageList(&m_view.m_ImgMgr.bigImglst()
		, &m_view.m_ImgMgr.smallImglst());

	CRect rcClient;
	m_view.m_MainWnd.GetClientRect(rcClient);
	UINT width = rcClient.Width() - globalSize.m_uLeftDockWidth - globalSize.m_uAlbumDockWidth - globalSize.m_uScrollbarWidth;
	
	CListColumnGuard ColumnGuard(width + globalSize.m_ColWidth_AddTime);
	ColumnGuard.addDynamic(_T("专辑曲目"), 0.63)
		.addFix(_T("类型/大小/时长"), globalSize.m_ColWidth_Type + globalSize.m_ColWidth_FileSize, true)
		.addFix(_T("关联歌单"), globalSize.m_ColWidth_RelatedPlaylist, true)
		.addDynamic(_T("目录"), 0.37)
		.addFix(_T("加入时间"), globalSize.m_ColWidth_AddTime, true);

	ListPara = CObjectList::tagListPara(ColumnGuard, 0);
	ListPara.fFontSize = globalSize.m_fSmallFontSize;
	ListPara.crText = __Color_Text;

	width -= m_view.m_globalSize.m_uScrollbarWidth;
	ListPara.uTileWidth = width/4-1;
	ListPara.uTileHeight = globalSize.m_uTileHeight;

	__AssertReturn(m_wndAlbumItemList.InitCtrl(ListPara), FALSE);

	m_wndAlbumItemList.SetCustomDraw([&](tagLVDrawSubItem& lvcd) {
		switch (lvcd.nSubItem)
		{
		case __Column_Name:
		{
			CAlbumItem *pAlbumItem = (CAlbumItem *)lvcd.pObject;
			__EnsureBreak(pAlbumItem);

			if (pAlbumItem->notExist())
			{
				lvcd.setTextAlpha(128);
			}
		}
		
		break;
		case __Column_Info:
		{
			CAlbumItem *pAlbumItem = (CAlbumItem *)lvcd.pObject;
			__EnsureBreak(pAlbumItem);

			CDC& dc = lvcd.dc;
			//dc.FillSolidRect(&rc, lvcd.crBkg);

			m_wndAlbumItemList.SetCustomFont(dc, -.2f, false);

			BYTE uAlpha = m_view.genByteRateAlpha(*pAlbumItem);
			dc.SetTextColor(lvcd.getTextColor(uAlpha));

			cauto rc = lvcd.rc;
			RECT rcText = rc;
			rcText.right = rcText.left + globalSize.m_ColWidth_Type;

			dc.DrawText(pAlbumItem->GetExtName().c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			dc.SetTextColor(lvcd.crText);

			rcText.left = rcText.right;
			rcText.right = rc.right;
			rcText.bottom = (rcText.bottom + rcText.top)/2 +6;
			dc.DrawText(pAlbumItem->displayFileSizeString(true).c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

			rcText.top = rcText.bottom -9;
			rcText.bottom = rc.bottom;
			dc.DrawText(pAlbumItem->displayDurationString().c_str(), &rcText, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		}

		lvcd.bSkipDefault = true;

		break;
		case __Column_Playlist:
			lvcd.bSetUnderline = true;
			lvcd.fFontSizeOffset = -.15f;

			break;
		case __Column_Path:
			lvcd.bSetUnderline = true;
			lvcd.fFontSizeOffset = -.2f;

			break;
		case __Column_AddTime:
			lvcd.fFontSizeOffset = -.2f;

			lvcd.setTextAlpha(128);

			break;
		default:
			break;
		};
	}, [&](tagLVDrawSubItem& lvcd) {
		auto eViewType = m_wndAlbumItemList.GetView();
		if (E_ListViewType::LVT_Report == eViewType || E_ListViewType::LVT_List == eViewType)
		{
			CAlbumItem *pAlbumItem = (CAlbumItem *)lvcd.pObject;
			__Ensure(pAlbumItem);

			auto iImage = pAlbumItem->cueFile() ? (int)E_GlobalImage::GI_WholeTrack : (int)E_GlobalImage::GI_AlbumItem;

			POINT pt;
			if (E_ListViewType::LVT_Report == eViewType)
			{
				m_wndAlbumItemList.GetItemPosition(lvcd.uItem, &pt);
			}
			else
			{
				pt.x = lvcd.rc.left;
				pt.y = lvcd.rc.top;
			}

			m_view.m_ImgMgr.smallImglst().Draw(&lvcd.dc, iImage, pt, 0);
		}
	});

	m_wndAlbumItemList.SetViewAutoChange([&](E_ListViewType eViewType) {
		m_wndAlbumItemList.UpdateItems();
		
		_asyncTask();
	});

	m_wndAlbumItemList.SetIconSpacing(width / 5-1, m_view.m_globalSize.m_uBigIconSize + m_view.m_globalSize.m_uIconSpace);


	__AssertReturn(m_wndMediaResPanel.Create(*this), FALSE);

	(void)__super::RegDragDropCtrl(m_wndAlbumList, [&](tagDragData& DragData) {
		CAlbum *pAlbum = (CAlbum*)m_wndAlbumList.GetSelObject();
		__EnsureReturn(pAlbum, false);

		__EnsureReturn(pAlbum->albumType() == E_AlbumType::AT_Normal, false);

		DragData.pMediaSet = pAlbum;

		DragData.iImage = (int)E_GlobalImage::GI_Album;

		return true;
	});

	(void)__super::RegDragDropCtrl(m_wndAlbumItemList, [&](tagDragData& DragData) {
		__EnsureReturn(m_pAlbum && m_pAlbum->albumType() == E_AlbumType::AT_Normal, false);

		TD_ListObjectList lstObjects;
		m_wndAlbumItemList.GetSelObjects(lstObjects);
		__EnsureReturn(lstObjects, false);

		DragData.lstMedias.add(lstObjects);

		DragData.iImage = (int)E_GlobalImage::GI_AlbumItem;

		return true;
	});

	__super::RegMenuHotkey(m_wndAlbumList, VK_RETURN, ID_PLAY_ALBUM);
	__super::RegMenuHotkey(m_wndAlbumList, VK_F2, ID_RENAME_ALBUM);
	__super::RegMenuHotkey(m_wndAlbumList, VK_DELETE, ID_REMOVE_ALBUM);

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
	dc.FillSolidRect(rcSingerImg, __crWhite);

	rcSingerImg.left += g_rcSingerImgMargin.left;
	rcSingerImg.top += g_rcSingerImgMargin.top;
	rcSingerImg.right -= g_rcSingerImgMargin.right;
	rcSingerImg.bottom -= g_rcSingerImgMargin.bottom;

	auto& img = m_imgSinger.IsNull()?m_imgSingerDefault:m_imgSinger;
	img.StretchBltEx(dc, rcSingerImg, E_ImgFixMode::IFM_Outer);
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
		
		m_wndAlbumList.MoveWindow(x-1, __BrowseTop, m_view.m_globalSize.m_uAlbumDockWidth
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
		this->Active();
	}

	m_pAlbum = NULL;

	bool bSingerChanged = (pSinger != m_pSinger);
	m_pSinger = pSinger;
	if (NULL == m_pSinger)
	{
		UpdateTitle();

		(void)m_wndAlbumList.ShowWindow(SW_HIDE);
		(void)m_wndAlbumList.DeleteAllItems();

		(void)m_wndAlbumItemList.ShowWindow(SW_HIDE);
		(void)m_wndAlbumItemList.DeleteAllItems();

		m_wndMediaResPanel.ShowDir();
		(void)m_wndMediaResPanel.ShowWindow(SW_SHOW);
		
		// TODO this->DestryWindow(); m_view.m_MainWnd.RemvePage(*this);
		this->Invalidate();

		return;
	}

	(void)m_wndAlbumList.ShowWindow(SW_SHOW);

	if (bSingerChanged)
	{
		UpdateSingerImage();

		(void)m_wndAlbumList.SetObjects(TD_ListObjectList((list<CAlbum>&)m_pSinger->albums()));

		(void)m_wndAlbumList.InsertItem(0, L"", -1);// , (int)E_GlobalImage::GI_Dir);

		m_wndMediaResPanel.SetSinger(*m_pSinger); //SetDir(m_pSinger->dir());
	}
		
	if (pAlbumItem)
	{
		auto pAlbum = (CAlbum*)pAlbumItem->m_pParent;
		m_wndAlbumList.SelectObject(pAlbum);
		m_pAlbum = pAlbum;

		m_wndAlbumItemList.DeselectAll();
		m_wndAlbumItemList.SelectObject(pAlbumItem);

		(void)m_wndAlbumItemList.SetFocus();
	}
	else
	{
		_showAlbum(NULL);
		(void)m_wndAlbumList.SelectFirstItem();

		if (pIMedia)
		{
			m_wndMediaResPanel.HittestMedia(*pIMedia, *this);
			(void)m_wndMediaResPanel.SetFocus();
		}
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

	auto hBitmap = m_view.getSingerImgMgr().getSingerImg(m_pSinger->m_strName, uSingerImgIdx);
	if (hBitmap)
	{
		m_imgSinger.Destroy();
		m_imgSinger.Attach(hBitmap);

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
	static CWinTimer s_timer;
	if (_playSingerImage(true))
	{
		s_timer.set(__PlaySingerImageElapse, [&]{
			_playSingerImage(false);
			return true;
		});
	}
	else
	{
		s_timer.kill();

		m_imgSinger.Destroy();
		this->InvalidateRect(__SingerImgRect);
	}
}

void CAlbumPage::UpdateTitle()
{
	m_strBaseTitle = L" ";

	wstring strTitle;
	if (m_pSinger)
	{
		if (m_pAlbum)
		{
			strTitle = __CNDot + m_pAlbum->m_strName;
			m_strBaseTitle.append(m_pSinger->m_strName);
		}
		else
		{
			strTitle = m_wndMediaResPanel.GetTitle();
			if (strTitle.empty())
			{
				m_strBaseTitle.append(m_pSinger->m_pParent->m_strName + __CNDot + m_pSinger->m_strName);
			}
			else
			{
				m_strBaseTitle.append(m_pSinger->m_strName);
			}
		}
	}
	else
	{
		m_strBaseTitle.append(L"歌手");
	}

	strTitle.push_back(' ');
	SetTitle(strTitle);
}

void CAlbumPage::UpdateSingerName()
{
	m_wndAlbumList.Update(0);
	//if (m_pSinger) m_wndAlbumList.SetItemText(0, 0, (L" " + m_pSinger->m_strName).c_str());
	
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
	__Ensure(m_pSinger);

	*pResult = 0;
	LPNMLISTVIEW lpNM = (LPNMLISTVIEW)pNMHDR;
	BOOL bEnable = (lpNM->iItem > 0);

	m_AlbumMenuGuard.EnableItem(ID_RENAME_ALBUM, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_REMOVE_ALBUM, bEnable);

	m_AlbumMenuGuard.EnableItem(ID_CNLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_HKLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_KRLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_JPLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_ENLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_OtherLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_TlLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_RsLanguage, bEnable);
	m_AlbumMenuGuard.EnableItem(ID_FrLanguage, bEnable);

	bool bPlayable = false;
	bool bNormalAlbum = false;
	if (bEnable)
	{
		CAlbum *pAlbum = (CAlbum*)m_wndAlbumList.GetItemObject(lpNM->iItem);
		__Assert(pAlbum);

		const CMediasetProperty& property = pAlbum->property();

		m_AlbumMenuGuard.CheckItem(ID_DisableDemand, property.isDisableDemand());
		m_AlbumMenuGuard.CheckItem(ID_DisableExport, property.isDisableExport());

		m_AlbumMenuGuard.CheckItem(ID_CNLanguage, property.isCnLanguage());
		m_AlbumMenuGuard.CheckItem(ID_HKLanguage, property.isHkLanguage());
		m_AlbumMenuGuard.CheckItem(ID_KRLanguage, property.isKrLanguage());
		m_AlbumMenuGuard.CheckItem(ID_JPLanguage, property.isJpLanguage());
		m_AlbumMenuGuard.CheckItem(ID_ENLanguage, property.isEnLanguage());

		m_AlbumMenuGuard.CheckItem(ID_OtherLanguage, property.isOtherLanguage());
		if (property.isTlLanguage() || property.isRsLanguage() || property.isFrLanguage())
		{
			m_AlbumMenuGuard.EnableItem(ID_OtherLanguage, false);
		}

		m_AlbumMenuGuard.CheckItem(ID_TlLanguage, property.isTlLanguage());
		m_AlbumMenuGuard.CheckItem(ID_RsLanguage, property.isRsLanguage());
		m_AlbumMenuGuard.CheckItem(ID_FrLanguage, property.isFrLanguage());

		bPlayable = pAlbum->albumItems();
		bNormalAlbum = E_AlbumType::AT_Normal == pAlbum->albumType();
	}
	
	m_AlbumMenuGuard.EnableItem(ID_PLAY_ALBUM, bPlayable);
	m_AlbumMenuGuard.EnableItem(ID_VERIFY_ALBUM, bPlayable && bNormalAlbum);
	m_AlbumMenuGuard.EnableItem(ID_EXPORT_ALBUM, bPlayable && bNormalAlbum);
	
	m_AlbumMenuGuard.EnableItem(ID_ADD_ALBUMITEM, bNormalAlbum);
	
	m_AlbumMenuGuard.EnableItem(ID_DisableDemand, bNormalAlbum);
	m_AlbumMenuGuard.EnableItem(ID_DisableExport, bNormalAlbum);

	(void)m_AlbumMenuGuard.EnableItem(ID_ADD_ALBUM, true);
	(void)m_AlbumMenuGuard.EnableItem(ID_ATTACH_DIR, true);
	(void)m_AlbumMenuGuard.EnableItem(ID_ATTACH_WHOLETRACK, true);

	(void)m_AlbumMenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);
}

void CAlbumPage::OnMenuCommand_Album(UINT uID)
{
	int nItem = m_wndAlbumList.GetSelItem();
	CAlbum *pAlbum = (CAlbum*)m_wndAlbumList.GetSelObject();

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

			cauto strSingerDir = __xmedialib.toAbsPath(m_pSinger->dir(), true);
			if (!fsutil::existDir(strSingerDir))
			{
				msgBox(L"歌手目录不存在！");
				return;
			}

			tagFileDlgOpt FileDlgOpt;
			FileDlgOpt.strTitle = L"添加专辑曲目";
			FileDlgOpt.strFilter = __MediaFilter;
			FileDlgOpt.strInitialDir = strSingerDir;
			CFileDlgEx fileDlg(FileDlgOpt);

			list<wstring> lstFiles;
			fileDlg.ShowOpenMulti(lstFiles);
			__Ensure(!lstFiles.empty());

			int nRet = m_view.getController().AddAlbumItems(lstFiles, *pAlbum);
			__Ensure(nRet > 0);

			this->_showAlbum(pAlbum);

			UINT uSelectCount = (UINT)nRet;
			m_wndAlbumItemList.SelectItems(m_wndAlbumItemList.GetItemCount() - uSelectCount, uSelectCount);
		}

		break;
	//case ID_ATTACH_WHOLETRACK:
	case ID_ATTACH_DIR:
	{
		__AssertBreak(m_pSinger);

		CMediaDir *pDir = m_view.showChooseDirDlg(L"选择附加目录", false);
		__EnsureBreak(pDir);

		if (pDir->files())
		{
			auto pAlbum = m_view.getSingerMgr().AddAlbum(*m_pSinger, pDir->fileName(), NULL, E_AlbumType::AT_Dir, pDir->GetPath());
			__EnsureBreak(pAlbum);

			m_wndAlbumList.InsertObject(*pAlbum);
		}

		pDir->dirs()([&](CPath& dir){
			if (dir.files())
			{
				auto pAlbum = m_view.getSingerMgr().AddAlbum(*m_pSinger, ((CMediaDir&)dir).fileName()
					, NULL, E_AlbumType::AT_Dir, ((CMediaDir&)dir).GetPath());
				if (pAlbum)
				{
					m_wndAlbumList.InsertObject(*pAlbum);
				}
			}
		});
	}

	break;
	case ID_ADD_ALBUM:
	{
		auto pAlbum = m_view.getSingerMgr().AddAlbum(*m_pSinger, L"");
		__EnsureBreak(pAlbum);

		auto nItem = m_wndAlbumList.InsertObject(*pAlbum);
		(void)m_wndAlbumList.EditLabel(nItem);
	}
	break;
	case ID_RENAME_ALBUM:
		__EnsureBreak(nItem > 0);

		(void)m_wndAlbumList.EditLabel(nItem);

		break;

	case ID_REMOVE_ALBUM:
		__AssertBreak(pAlbum);

		__EnsureBreak(confirmBox(L"确认删除所选专辑?"));

		(void)m_wndAlbumList.DeleteItem(nItem);
		(void)m_wndAlbumList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

		__EnsureBreak(m_view.getController().removeMediaSet(*pAlbum));

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
		__AssertBreak(pAlbum);
		CMediasetProperty property = pAlbum->property();
		if (ID_DisableDemand == uID)
		{
			bool bDisableDemand = !property.isDisableDemand();
			property.setDisableDemand(bDisableDemand);
			property.setDisableExport(bDisableDemand);
			m_wndAlbumList.UpdateItem(nItem);
		}
		else if (ID_DisableExport == uID)
		{
			property.setDisableExport(!property.isDisableExport());
			m_wndAlbumList.UpdateItem(nItem);
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
		
		lstAlbumItems.front([&](auto& media) {
			if (m_pAlbum->albumType() == E_AlbumType::AT_Normal)
			{
				(void)m_wndMediaResPanel.HittestMedia(media, *this);
			}
			else
			{
				m_view.m_MediaResPage.HittestMedia(media, *this);
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
			lstAlbumItems.front([&](CMedia& media) {
				m_view.exploreMedia(media);
			});
		}

		break;
	case ID_EXPORT_ALBUMITEM:
		if (lstAlbumItems)
		{
			m_view.exportMedia(TD_MediaList(lstAlbumItems));
		}
		else
		{
			TD_MediaList paMedias((ArrList<CAlbumItem>&)m_pAlbum->albumItems());
			m_view.exportMedia(paMedias);
		}

		break;
	case ID_REMOVE_ALBUMITEM:
		__EnsureBreak(lstAlbumItems);

		__EnsureBreak(confirmBox(L"确认移除选中的曲目?"));

		__EnsureBreak(m_view.getModel().removeMedia(lstAlbumItems));

		break;
	case ID_RENAME_ALBUMITEM:
		__EnsureBreak(1 == lstObjects.size());

		lstObjects.front([&](CListObject& ListObject) {
			(void)m_wndAlbumItemList.EditLabel(m_wndAlbumItemList.GetObjectItem(&ListObject));
		});
	}
}

/*void CAlbumPage::OnActive(BOOL bActive)
{
	if (bActive)
	{
		if (m_pAlbum)
		{
			_asyncTask();
		}
		else
		{
			m_wndMediaResPanel.OnActive(true);
		}
	}
}*/

void CAlbumPage::OnMenuCommand(UINT uID, UINT uVkKey)
{
	CWnd *pWndFocus = CWnd::GetFocus();
	if (&m_wndAlbumList == pWndFocus)
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

	auto pAlbum = (CAlbum*)m_wndAlbumList.GetItemObject(pNMLV->iItem);
	if (pAlbum != m_pAlbum)
	{
		_showAlbum(pAlbum);
	}
}

void CAlbumPage::_showAlbum(CAlbum *pAlbum)
{
	__Ensure(m_hWnd);

	if (NULL == pAlbum)
	{
		m_pAlbum = NULL;
		this->UpdateTitle();

		m_wndMediaResPanel.Refresh();
		(void)m_wndMediaResPanel.ShowWindow(SW_SHOW);

		(void)m_wndAlbumItemList.ShowWindow(SW_HIDE);
		(void)m_wndAlbumItemList.DeleteAllItems();

		return;
	}

	bool bChanged = pAlbum != m_pAlbum;
	m_pAlbum = pAlbum;
	this->UpdateTitle();

	if (E_AlbumType::AT_Normal == m_pAlbum->albumType())
	{
		m_pAlbum->albumItems()([&](cauto AlbumItem) {
			((CAlbumItem&)AlbumItem).findRelatedMedia();
		});
	}

	(void)m_wndAlbumItemList.SetObjects(TD_ListObjectList((ArrList<CAlbumItem>&)m_pAlbum->albumItems()));
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
	if (m_pAlbum)
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

	CAlbum *pAlbum = (CAlbum*)m_wndAlbumList.GetItemObject(pLVDispInfo->item.iItem);
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
			msgBox(L"名称含特殊字符！");
		}
		else if (E_RenameRetCode::RRC_NameExists == eRetCode)
		{
			msgBox(L"重命名失败，存在同名专辑！");
		}
		return;
	}

	(void)m_wndAlbumList.SetItemText(pLVDispInfo->item.iItem, 0, cstrNewName);

	this->UpdateTitle();
}

DROPEFFECT CAlbumPage::OnMediasDragOverExploreList(const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	//__EnsureReturn(_checkMediasDropable(lstMedias), DROPEFFECT_NONE);

	DROPEFFECT dwRet = 0;
	lstMedias.front([&](IMedia& media) {
		if (media.mediaSet() == m_pAlbum)
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

		tagPOINT lpPoint[4] {
			{DragContext.x - __Margin, DragContext.y - __Margin}
			, {DragContext.x + __Margin, DragContext.y - __Margin}
			, {DragContext.x - __Margin, DragContext.y + __Margin}
			, {DragContext.x + __Margin, DragContext.y + __Margin}
		};

		map<int, CRect> mapItems;

		for (UINT uIndex = 0; uIndex < sizeof(lpPoint)/sizeof(tagPOINT); ++uIndex)
		{
			nItem = m_wndAlbumItemList.HitTest(lpPoint[uIndex]);
			if (nItem >= 0 && mapItems.find(nItem) == mapItems.end())
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
		pSrcMediaSet = media.mediaSet();
	});

	UINT uCount = 0;
	if (pSrcMediaSet != m_pAlbum)
	{
		int nRet = m_view.getController().AddAlbumItems(lstMedias, *m_pAlbum, uTargetPos);
		__EnsureReturn(nRet > 0, FALSE);
		uCount = (UINT)nRet;

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
		__EnsureReturn(nNewPos >= 0, FALSE);

		uCount = lstMedias.size();
	}

	_showAlbum(m_pAlbum);
	m_wndAlbumItemList.SelectItems(nNewPos, uCount);

	return TRUE;
}

BOOL CAlbumPage::_checkMediasDropable(const TD_IMediaList& lstMedias)
{
	__EnsureReturn(m_pSinger, FALSE);
	cauto strSingerDir = m_pSinger->dir();

	BOOL bRet = TRUE;
	lstMedias.front([&](IMedia& media) {
		if (media.type() != E_MediaType::MT_AlbumItem)
		{
			lstMedias([&](IMedia& media) {
				if (!fsutil::CheckSubPath(strSingerDir, media.GetPath()))
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
	bool bScroll = DragScroll(m_wndAlbumList, DragContext.x, DragContext.y);

	//__EnsureReturn(_checkMediasDropable(lstMedias), DROPEFFECT_NONE);

	int nItem = m_wndAlbumList.HitTest(CPoint(5, DragContext.y));
	__EnsureReturn(1 <= nItem, DROPEFFECT_NONE);	
	CAlbum *pDropHilightAlbum = (CAlbum*)m_wndAlbumList.GetItemObject(nItem);
	__AssertReturn(pDropHilightAlbum, DROPEFFECT_NONE);
	
	CMediaSet *pSrcMediaSet = NULL;
	lstMedias.front([&](IMedia& media) {
		pSrcMediaSet = media.mediaSet();
	});
	__EnsureReturn(pSrcMediaSet != pDropHilightAlbum, DROPEFFECT_NONE);
	DragContext.pTargetObj = pDropHilightAlbum;

	if (!bScroll)
	{
		CRect rcItem;
		(void)m_wndAlbumList.GetItemRect(nItem, &rcItem, LVIR_BOUNDS);
		DragContext.DrawDragOverRect(rcItem.top, rcItem.bottom);
	}

	if (pSrcMediaSet && E_MediaSetType::MST_Album==pSrcMediaSet->type())
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
		pSrcMediaSet = media.mediaSet();
	});

	UINT uCount = 0;
	if (pSrcMediaSet && E_MediaSetType::MST_Album == pSrcMediaSet->type())
	{
		__EnsureReturn(pTargetAlbum != pSrcMediaSet, FALSE);

		auto len = pTargetAlbum->GetSinger().dir().size();
		TD_AlbumItemList lstAlbumItems(lstMedias);
		cauto lstOppPath = lstAlbumItems.map([&](const CAlbumItem& AlbumItem) {
			auto strPath = AlbumItem.GetPath();
			strPath.erase(0, len);
			return strPath;
		});
		__EnsureReturn(m_view.getSingerMgr().AddAlbumItems(lstOppPath, *pTargetAlbum), FALSE);
		uCount = lstAlbumItems.size();

		if (DROPEFFECT_MOVE == nDropEffect)
		{
			__EnsureReturn(m_view.getSingerMgr().RemoveAlbumItems(lstAlbumItems), FALSE);
		}
	}
	else
	{
		int nRet = m_view.getController().AddAlbumItems(lstMedias, *pTargetAlbum);
		__EnsureReturn(nRet > 0, FALSE);
		
		uCount = (UINT)nRet;
	}

	m_wndAlbumList.SelectObject(pTargetAlbum);

	_showAlbum(pTargetAlbum);

	m_wndAlbumItemList.SelectItems((UINT)m_wndAlbumItemList.GetItemCount()-uCount, uCount);

	(void)m_wndAlbumItemList.SetFocus();

	return TRUE;
}

DROPEFFECT CAlbumPage::OnMediaSetDragOver(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__EnsureReturn(pwndCtrl == &m_wndAlbumList, DROPEFFECT_NONE);
	
	__EnsureReturn(pMediaSet && E_MediaSetType::MST_Album == pMediaSet->type(), DROPEFFECT_NONE);
	
	int nDragingItem = m_wndAlbumList.GetObjectItem(pMediaSet);
	__AssertReturn(nDragingItem >= 0, DROPEFFECT_NONE);
	
	DROPEFFECT dwRet = DROPEFFECT_MOVE;
	handleDragOver(m_wndAlbumList, DragContext, [&](UINT& uTargetPos) {
		if (uTargetPos < 1)
		{
			uTargetPos = 1;
		}
	});
	return dwRet;
}

BOOL CAlbumPage::OnMediaSetDrop(CWnd *pwndCtrl, CMediaSet *pMediaSet, CDragContext& DragContext)
{
	__AssertReturn(pwndCtrl == &m_wndAlbumList && pMediaSet && E_MediaSetType::MST_Album == pMediaSet->type(), DROPEFFECT_NONE);

	int nDragingItem = m_wndAlbumList.GetObjectItem(pMediaSet);
	__AssertReturn(nDragingItem >= 0, FALSE);

	int nTargetPos = DragContext.uTargetPos;
	if (nTargetPos > nDragingItem)
	{
		nTargetPos--;
	}
	__EnsureReturn(nTargetPos != nDragingItem, FALSE);
	
	CRedrawLock RedrawLock(*this);
	_showAlbum(NULL);

	int nNewPos = nTargetPos - 1;
	if (nNewPos < 0)
	{
		nNewPos = 0;
	}
	CAlbum *pAlbum = m_view.getSingerMgr().RepositAlbum(*(CAlbum*)pMediaSet, (UINT)nNewPos);
	__EnsureReturn(pAlbum, FALSE);

	(void)m_wndAlbumList.DeleteItem(nDragingItem);
	(void)m_wndAlbumList.InsertObject(*pAlbum, nTargetPos);
	m_wndAlbumList.SelectItem(nTargetPos);
	
	return TRUE;
}

DROPEFFECT CAlbumPage::OnMediasDragOver(CWnd *pwndCtrl, const TD_IMediaList& lstMedias, CDragContext& DragContext)
{
	if (pwndCtrl == &m_wndAlbumList)
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
	if (pwndCtrl == &m_wndAlbumList)
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
	__Ensure(m_pAlbum);
	bool bNormalAlbum = E_AlbumType::AT_Normal == m_pAlbum->albumType();
	
	m_MenuGuard.EnableItem(ID_ADD_ALBUMITEM, bNormalAlbum);

	m_MenuGuard.EnableItem(ID_PLAY_ALBUMITEM, m_pAlbum->albumItems());

	int nSelCount = m_wndAlbumItemList.GetSelectedCount();
	m_MenuGuard.EnableItem(ID_FIND_ALBUMITEM, (1 == nSelCount) && bNormalAlbum);
	m_MenuGuard.EnableItem(ID_HITTEST_ALBUMITEM, (1 == nSelCount));
	m_MenuGuard.EnableItem(ID_SETALARMCLOCK, (nSelCount > 0));
	
	m_MenuGuard.EnableItem(ID_CopyTitle, (1 == nSelCount));
	m_MenuGuard.EnableItem(ID_EXPLORE_ALBUMITEM, (1 == nSelCount));
	m_MenuGuard.EnableItem(ID_EXPORT_ALBUMITEM, (m_wndAlbumItemList.GetItemCount() > 0) && bNormalAlbum);

	m_MenuGuard.EnableItem(ID_RENAME_ALBUMITEM, (1 == nSelCount) && bNormalAlbum);
	m_MenuGuard.EnableItem(ID_REMOVE_ALBUMITEM, (nSelCount > 0) && bNormalAlbum);

	(void)m_MenuGuard.Popup(this, m_view.m_globalSize.m_uMenuItemHeight, m_view.m_globalSize.m_fMidFontSize);
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
	__Ensure(lpNMList->iItem >= 0);

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
		__Ensure(m_wndAlbumItemList.GetSelectedCount()<=1 && !CMainApp::getKeyState(VK_SHIFT) && !CMainApp::getKeyState(VK_CONTROL));

		int iItem = lpNMList->iItem;
		m_wndAlbumItemList.AsyncLButtondown([=]{
			auto pAlbumItem = (CAlbumItem*)m_wndAlbumItemList.GetItemObject(iItem);
			__Ensure(pAlbumItem);
			(void)pAlbumItem->findRelatedMedia();
			m_wndAlbumItemList.UpdateItem(iItem, pAlbumItem);

			if (__Column_Playlist == iSubItem)
			{
				m_view.hittestRelatedPlaylist(*pAlbumItem);
			}
			else
			{
				this->OnMenuCommand_AlbumItem(ID_HITTEST_ALBUMITEM);
			}
		});
	}
}

void CAlbumPage::UpdateRelated(E_RelatedMediaSet eRmsType, const tagMediaSetChanged& MediaSetChanged)
{
	if (NULL != m_pSinger)
	{
		m_wndMediaResPanel.UpdateRelated(eRmsType, MediaSetChanged);

		if (m_pAlbum)
		{
			m_pAlbum->albumItems()([&](cauto t_AlbumItem, size_t uIdx) {
				auto& AlbumItem = (CAlbumItem&)t_AlbumItem;
				if (AlbumItem.UpdateRelatedMediaSet(eRmsType, MediaSetChanged))
				{
					m_wndAlbumItemList.UpdateItem(uIdx, &AlbumItem);
				}
				uIdx++;
			});
		}
	}
}

void CAlbumPage::_asyncTask()
{
	__async(10, [&]{
		if (m_pAlbum)
		{
			m_wndAlbumItemList.AsyncTask(__AsyncTaskElapse + m_pAlbum->albumItems().size() / 10, [](CListObject& object) {
				__checkMedia((CMedia&)object);
				return false;
			});
		}
	});
}
